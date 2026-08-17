#!/usr/bin/env python3
"""Source-level laser and muon track profiles from the ROOT clusters tree.

The primary-electron profiles use generated electrons before drift. The laser
energy profiles use the Gaussian optics parameters from the JSON config.
"""

from __future__ import annotations

import argparse
import json
import math
import os
from dataclasses import dataclass
from pathlib import Path

os.environ.setdefault("MPLCONFIGDIR", "/tmp/fc0-lasersim-matplotlib")

import numpy as np
import ROOT


@dataclass
class TrackLine:
    origin: np.ndarray
    direction: np.ndarray


@dataclass
class PrimaryProfiles:
    n_events: int
    z_centers: np.ndarray
    z_density: np.ndarray
    r_centers: np.ndarray
    radial_density: np.ndarray
    z_min: float
    z_max: float
    r_max: float


@dataclass
class LaserEnergyProfiles:
    z_centers: np.ndarray
    on_axis_fluence_j_cm2: np.ndarray
    on_axis_intensity_w_cm2: np.ndarray
    r_centers: np.ndarray
    radial_fluence_j_cm2: np.ndarray
    radial_intensity_w_cm2: np.ndarray
    waist_z_mm: float


def deep_merge(base, override):
    if not isinstance(base, dict) or not isinstance(override, dict):
        return override
    result = dict(base)
    for key, value in override.items():
        if key == "extends":
            continue
        result[key] = deep_merge(result[key], value) if key in result else value
    return result


def load_config(path: Path):
    data = json.loads(path.read_text())
    if "extends" not in data:
        return data
    base_path = path.parent / data["extends"]
    return deep_merge(load_config(base_path.resolve()), data)


def unit(vector):
    arr = np.asarray(vector, dtype=float)
    mag = float(np.linalg.norm(arr))
    if mag <= 0.0:
        raise ValueError("zero-length direction")
    return arr / mag


def track_line_from_config(cfg, kind: str) -> TrackLine:
    gen = cfg["generator"]
    if kind == "laser":
        laser = gen["laser"]
        pos = laser["position"]
        direction = laser["direction"]
        origin = np.asarray([
            float(pos.get("x_start_mm", 0.0)),
            float(pos.get("y_mm", 0.0)),
            float(pos.get("z_mm", 0.0)),
        ])
    else:
        track = gen["track"]
        pos = track["position"]
        direction = track["direction"]
        origin = np.asarray([
            float(pos.get("x_mm", 0.0)),
            float(pos.get("y_mm", 0.0)),
            float(pos.get("z_mm", 0.0)),
        ])
    return TrackLine(
        origin=origin,
        direction=unit([
            float(direction.get("dx", 0.0)),
            float(direction.get("dy", 0.0)),
            float(direction.get("dz", 1.0)),
        ]),
    )


def read_cluster_points(path: Path) -> tuple[np.ndarray, int]:
    root_file = ROOT.TFile.Open(str(path))
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"cannot open {path}")
    tree = root_file.Get("clusters")
    if not tree:
        raise RuntimeError(f"{path} has no clusters tree")

    chunks = []
    for event in tree:
        if len(event.cl_x) == 0:
            continue
        chunks.append(np.column_stack((
            np.asarray(event.cl_x, dtype=float),
            np.asarray(event.cl_y, dtype=float),
            np.asarray(event.cl_z, dtype=float),
        )))

    if not chunks:
        return np.empty((0, 3), dtype=float), int(tree.GetEntries())
    return np.vstack(chunks), int(tree.GetEntries())


def radial_distance_xy(points: np.ndarray, line: TrackLine) -> np.ndarray:
    # This analysis is for horizontal z-directed tracks; radial transverse
    # distance is evaluated in the local x-y plane around the configured axis.
    dx = points[:, 0] - line.origin[0]
    dy = points[:, 1] - line.origin[1]
    return np.sqrt(dx * dx + dy * dy)


def primary_profiles(
    points: np.ndarray,
    n_events: int,
    line: TrackLine,
    z_min: float,
    z_max: float,
    z_bin_cm: float,
    r_max: float,
    r_bin_mm: float,
) -> PrimaryProfiles:
    z_edges = np.arange(z_min, z_max + 0.5 * z_bin_cm * 10.0, z_bin_cm * 10.0)
    if z_edges.size < 2 or z_edges[-1] < z_max:
        z_edges = np.append(z_edges, z_max)
    z_counts, _ = np.histogram(points[:, 2], bins=z_edges)
    z_widths_cm = np.diff(z_edges) / 10.0
    z_density = z_counts / max(n_events, 1) / z_widths_cm

    r = radial_distance_xy(points, line)
    r_edges = np.arange(0.0, r_max + 0.5 * r_bin_mm, r_bin_mm)
    if r_edges.size < 2 or r_edges[-1] < r_max:
        r_edges = np.append(r_edges, r_max)
    r_counts, _ = np.histogram(r, bins=r_edges)
    track_length_cm = max((z_max - z_min) / 10.0, 1.0e-9)
    annulus_area_mm2 = math.pi * (r_edges[1:] ** 2 - r_edges[:-1] ** 2)
    radial_density = r_counts / max(n_events, 1) / track_length_cm / annulus_area_mm2

    return PrimaryProfiles(
        n_events=n_events,
        z_centers=0.5 * (z_edges[:-1] + z_edges[1:]),
        z_density=z_density,
        r_centers=0.5 * (r_edges[:-1] + r_edges[1:]),
        radial_density=radial_density,
        z_min=z_min,
        z_max=z_max,
        r_max=r_max,
    )


def laser_energy_profiles(
    cfg,
    z_min: float,
    z_max: float,
    z_bin_cm: float,
    r_max: float,
    r_bin_mm: float,
) -> LaserEnergyProfiles:
    laser = cfg["generator"]["laser"]
    pos = laser["position"]
    optics = laser["optics"]
    z0 = float(pos["z_mm"])
    waist_z = z0 + float(optics["waist_s_mm"])
    wavelength_cm = float(optics["wavelength_nm"]) * 1.0e-7
    energy_j = float(optics.get("pulse_energy_J", optics.get("pulse_energy_mJ", 1.0) * 1.0e-3))
    pulse_duration_s = float(optics.get("pulse_duration_s", optics.get("pulse_duration_ns", 5.0) * 1.0e-9))
    w0x_cm = float(optics["waist_x_mm"]) * 0.1
    w0y_cm = float(optics["waist_y_mm"]) * 0.1
    m2x = float(optics.get("m2_x", 1.0))
    m2y = float(optics.get("m2_y", 1.0))
    zrx_cm = math.pi * w0x_cm * w0x_cm / max(m2x * wavelength_cm, 1.0e-30)
    zry_cm = math.pi * w0y_cm * w0y_cm / max(m2y * wavelength_cm, 1.0e-30)

    z_edges = np.arange(z_min, z_max + 0.5 * z_bin_cm * 10.0, z_bin_cm * 10.0)
    if z_edges.size < 2 or z_edges[-1] < z_max:
        z_edges = np.append(z_edges, z_max)
    z_centers = 0.5 * (z_edges[:-1] + z_edges[1:])

    s_from_waist_cm = (z_centers - waist_z) * 0.1
    wx_cm = w0x_cm * np.sqrt(1.0 + (s_from_waist_cm / zrx_cm) ** 2)
    wy_cm = w0y_cm * np.sqrt(1.0 + (s_from_waist_cm / zry_cm) ** 2)
    on_axis_fluence = 2.0 * energy_j / (math.pi * wx_cm * wy_cm)
    temporal_norm = math.sqrt(math.pi / (4.0 * math.log(2.0)))
    on_axis_intensity = on_axis_fluence / (pulse_duration_s * temporal_norm)

    r_edges = np.arange(0.0, r_max + 0.5 * r_bin_mm, r_bin_mm)
    if r_edges.size < 2 or r_edges[-1] < r_max:
        r_edges = np.append(r_edges, r_max)
    r_centers = 0.5 * (r_edges[:-1] + r_edges[1:])
    r_cm = r_centers * 0.1

    if abs(w0x_cm - w0y_cm) / max(w0x_cm, w0y_cm) > 1.0e-6:
        print("[warning] radial laser profile uses the geometric-mean radius for an elliptical beam")
    w_cm = np.sqrt(wx_cm * wy_cm)
    radial_fluence = np.asarray([
        float(np.mean(on_axis_fluence * np.exp(-2.0 * (radius / w_cm) ** 2)))
        for radius in r_cm
    ])
    radial_intensity = radial_fluence / (pulse_duration_s * temporal_norm)

    return LaserEnergyProfiles(
        z_centers=z_centers,
        on_axis_fluence_j_cm2=on_axis_fluence,
        on_axis_intensity_w_cm2=on_axis_intensity,
        r_centers=r_centers,
        radial_fluence_j_cm2=radial_fluence,
        radial_intensity_w_cm2=radial_intensity,
        waist_z_mm=waist_z,
    )


def save_profiles_csv(path: Path, label: str, profiles: PrimaryProfiles) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w") as f:
        f.write("sample,axis,center,primary_density\n")
        for z, value in zip(profiles.z_centers, profiles.z_density):
            f.write(f"{label},z_mm,{z:.8g},{value:.8g}\n")
        for r, value in zip(profiles.r_centers, profiles.radial_density):
            f.write(f"{label},r_mm,{r:.8g},{value:.8g}\n")


def save_laser_energy_csv(path: Path, energy: LaserEnergyProfiles) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w") as f:
        f.write("axis,center,fluence_J_per_cm2,intensity_W_per_cm2\n")
        for z, fluence, intensity in zip(
            energy.z_centers,
            energy.on_axis_fluence_j_cm2,
            energy.on_axis_intensity_w_cm2,
        ):
            f.write(f"z_mm,{z:.8g},{fluence:.8g},{intensity:.8g}\n")
        for r, fluence, intensity in zip(
            energy.r_centers,
            energy.radial_fluence_j_cm2,
            energy.radial_intensity_w_cm2,
        ):
            f.write(f"r_mm,{r:.8g},{fluence:.8g},{intensity:.8g}\n")


def plot_profiles(muon: PrimaryProfiles, laser: PrimaryProfiles,
                  energy: LaserEnergyProfiles, out_prefix: Path) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    out_prefix.parent.mkdir(parents=True, exist_ok=True)

    fig, ax1 = plt.subplots(figsize=(9.4, 5.7), constrained_layout=True)
    ax1.plot(muon.z_centers, muon.z_density, label="muon primaries", linewidth=1.7)
    ax1.plot(laser.z_centers, laser.z_density, label="laser primaries", linewidth=1.7)
    ax1.set_xlabel("z [mm]")
    ax1.set_ylabel("primary electrons / event / cm")
    ax2 = ax1.twinx()
    ax2.plot(energy.z_centers, energy.on_axis_fluence_j_cm2,
             color="black", linestyle="--", label="laser fluence")
    ax2.axvline(energy.waist_z_mm, color="black", linewidth=1.0, alpha=0.45,
                label="laser waist")
    ax2.set_ylabel("laser on-axis fluence [J/cm2]")
    lines = [line for line in ax1.get_lines() + ax2.get_lines()
             if not line.get_label().startswith("_")]
    ax1.legend(lines, [line.get_label() for line in lines], loc="upper right")
    ax1.set_title("Longitudinal source profiles")
    fig.savefig(out_prefix.with_name(out_prefix.name + "_longitudinal.png"), dpi=160)

    fig, ax1 = plt.subplots(figsize=(9.4, 5.7), constrained_layout=True)
    ax1.semilogy(muon.r_centers, np.maximum(muon.radial_density, 1.0e-12),
                 label="muon primaries", linewidth=1.7)
    ax1.semilogy(laser.r_centers, np.maximum(laser.radial_density, 1.0e-12),
                 label="laser primaries", linewidth=1.7)
    ax1.set_xlabel("radial distance in x-y plane [mm]")
    ax1.set_ylabel("primary electrons / event / cm / mm2")
    ax2 = ax1.twinx()
    ax2.semilogy(energy.r_centers, np.maximum(energy.radial_fluence_j_cm2, 1.0e-30),
                 color="black", linestyle="--", label="laser fluence, z-averaged")
    ax2.set_ylabel("laser fluence [J/cm2]")
    lines = [line for line in ax1.get_lines() + ax2.get_lines()
             if not line.get_label().startswith("_")]
    ax1.legend(lines, [line.get_label() for line in lines], loc="upper right")
    ax1.set_title("Transverse source profiles")
    fig.savefig(out_prefix.with_name(out_prefix.name + "_transverse.png"), dpi=160)


def print_summary(label: str, profiles: PrimaryProfiles) -> None:
    print(f"[{label} primaries]")
    print(f"  events:             {profiles.n_events}")
    print(f"  z range [mm]:       [{profiles.z_min:.3f}, {profiles.z_max:.3f}]")
    print(f"  mean z density:     {float(np.mean(profiles.z_density)):.6g} e-/event/cm")
    print(f"  peak z density:     {float(np.max(profiles.z_density)):.6g} e-/event/cm")
    print(f"  peak radial density:{float(np.max(profiles.radial_density)):.6g} e-/event/cm/mm2")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--muon", type=Path, required=True)
    parser.add_argument("--laser", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=Path("config/laser_2photon_horizontal_zR50cm_100_anode.json"))
    parser.add_argument("--out-prefix", type=Path, default=Path("analysis_plots/source_track_profiles"))
    parser.add_argument("--z-min", type=float, default=-820.0)
    parser.add_argument("--z-max", type=float, default=834.0)
    parser.add_argument("--z-bin-cm", type=float, default=1.0)
    parser.add_argument("--r-max-mm", type=float, default=8.0)
    parser.add_argument("--r-bin-mm", type=float, default=0.1)
    args = parser.parse_args()

    cfg = load_config(args.config.resolve())
    muon_line = track_line_from_config(cfg, "track")
    laser_line = track_line_from_config(cfg, "laser")
    muon_points, muon_events = read_cluster_points(args.muon)
    laser_points, laser_events = read_cluster_points(args.laser)
    if muon_points.size == 0 or laser_points.size == 0:
        raise SystemExit("missing source primary electrons in one of the ROOT files")

    muon = primary_profiles(
        muon_points, muon_events, muon_line,
        args.z_min, args.z_max, args.z_bin_cm, args.r_max_mm, args.r_bin_mm,
    )
    laser = primary_profiles(
        laser_points, laser_events, laser_line,
        args.z_min, args.z_max, args.z_bin_cm, args.r_max_mm, args.r_bin_mm,
    )
    energy = laser_energy_profiles(
        cfg, args.z_min, args.z_max, args.z_bin_cm, args.r_max_mm, args.r_bin_mm,
    )

    plot_profiles(muon, laser, energy, args.out_prefix)
    save_profiles_csv(args.out_prefix.with_name(args.out_prefix.name + "_muon_primaries.csv"), "muon", muon)
    save_profiles_csv(args.out_prefix.with_name(args.out_prefix.name + "_laser_primaries.csv"), "laser", laser)
    save_laser_energy_csv(args.out_prefix.with_name(args.out_prefix.name + "_laser_energy.csv"), energy)

    print_summary("muon", muon)
    print_summary("laser", laser)
    print("[laser energy]")
    print(f"  waist z [mm]:       {energy.waist_z_mm:.3f}")
    print(f"  peak fluence:       {float(np.max(energy.on_axis_fluence_j_cm2)):.6g} J/cm2")
    print(f"  peak intensity:     {float(np.max(energy.on_axis_intensity_w_cm2)):.6g} W/cm2")
    print(f"[output] {args.out_prefix}_longitudinal.png")
    print(f"[output] {args.out_prefix}_transverse.png")
    print(f"[output] {args.out_prefix}_muon_primaries.csv")
    print(f"[output] {args.out_prefix}_laser_primaries.csv")
    print(f"[output] {args.out_prefix}_laser_energy.csv")


if __name__ == "__main__":
    main()
