#!/usr/bin/env python3
"""Plot drifted endpoint density per track unit length for horizontal tracks.

The script reads the ROOT "anode" tree, keeps only endpoints assigned to valid
HAT pads, bins them along z, and reports electrons/cm for each event and z bin.
"""

from __future__ import annotations

import argparse
import csv
import os
from dataclasses import dataclass
from pathlib import Path

os.environ.setdefault("MPLCONFIGDIR", "/tmp/fc0-lasersim-matplotlib")

import numpy as np
import ROOT


PADS_Z_PER_ERAM = 36
PADS_Y_PER_ERAM = 32


@dataclass
class DensitySample:
    values: np.ndarray
    value_z_centers: np.ndarray
    z_centers: np.ndarray
    profile_mean: np.ndarray
    profile_rms: np.ndarray
    profile_entries: np.ndarray
    n_events: int
    n_bins: int
    z_min: float
    z_max: float
    bin_width_cm: float


def valid_pad(eram: int, pad_z: int, pad_y: int) -> bool:
    return (
        0 <= eram < 8
        and 0 <= pad_z < PADS_Z_PER_ERAM
        and 0 <= pad_y < PADS_Y_PER_ERAM
    )


def load_density(
    path: Path,
    z_min: float | None,
    z_max: float | None,
    bin_width_cm: float,
) -> DensitySample:
    root_file = ROOT.TFile.Open(str(path))
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"cannot open {path}")
    tree = root_file.Get("anode")
    if not tree:
        raise RuntimeError(f"{path} has no anode tree")

    all_z = []
    event_z = []
    for event in tree:
        zs = []
        for z, eram, pad_z, pad_y in zip(
            event.anode_z,
            event.anode_eram,
            event.anode_padZ,
            event.anode_padY,
        ):
            if not valid_pad(int(eram), int(pad_z), int(pad_y)):
                continue
            z_value = float(z)
            zs.append(z_value)
            all_z.append(z_value)
        event_z.append(np.asarray(zs, dtype=float))

    if not all_z:
        raise RuntimeError(f"{path} has no valid pad-assigned anode endpoints")

    z_lo = float(min(all_z) if z_min is None else z_min)
    z_hi = float(max(all_z) if z_max is None else z_max)
    if z_hi <= z_lo:
        raise RuntimeError(f"invalid z range: [{z_lo}, {z_hi}] mm")

    bin_width_mm = bin_width_cm * 10.0
    edges = np.arange(z_lo, z_hi + 0.5 * bin_width_mm, bin_width_mm)
    if len(edges) < 2 or edges[-1] < z_hi:
        edges = np.append(edges, z_hi)

    densities_by_bin = [[] for _ in range(len(edges) - 1)]
    values = []
    value_z_centers = []
    for zs in event_z:
        selected = zs[(zs >= z_lo) & (zs < z_hi)]
        counts, _ = np.histogram(selected, bins=edges)
        widths_cm = np.diff(edges) / 10.0
        density = counts / widths_cm
        for i, value in enumerate(density):
            z_center = float(0.5 * (edges[i] + edges[i + 1]))
            densities_by_bin[i].append(float(value))
            values.append(float(value))
            value_z_centers.append(z_center)

    profile_mean = np.asarray(
        [np.mean(v) if v else 0.0 for v in densities_by_bin],
        dtype=float,
    )
    profile_rms = np.asarray(
        [np.std(v) if v else 0.0 for v in densities_by_bin],
        dtype=float,
    )
    profile_entries = np.asarray([len(v) for v in densities_by_bin], dtype=int)
    z_centers = 0.5 * (edges[:-1] + edges[1:])

    return DensitySample(
        values=np.asarray(values, dtype=float),
        value_z_centers=np.asarray(value_z_centers, dtype=float),
        z_centers=z_centers,
        profile_mean=profile_mean,
        profile_rms=profile_rms,
        profile_entries=profile_entries,
        n_events=int(tree.GetEntries()),
        n_bins=len(edges) - 1,
        z_min=z_lo,
        z_max=z_hi,
        bin_width_cm=bin_width_cm,
    )


def save_csv(sample: DensitySample, path: Path, label: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([
            "sample",
            "z_center_mm",
            "mean_electrons_per_cm",
            "rms_electrons_per_cm",
            "entries",
        ])
        for z, mean, rms, entries in zip(
            sample.z_centers,
            sample.profile_mean,
            sample.profile_rms,
            sample.profile_entries,
        ):
            writer.writerow([label, f"{z:.6g}", f"{mean:.6g}", f"{rms:.6g}", int(entries)])


def select_fit_values(
    sample: DensitySample,
    z_min: float | None,
    z_max: float | None,
) -> np.ndarray:
    mask = np.ones(sample.values.shape, dtype=bool)
    if z_min is not None:
        mask &= sample.value_z_centers >= z_min
    if z_max is not None:
        mask &= sample.value_z_centers <= z_max
    return sample.values[mask]


def fit_density(
    values: np.ndarray,
    fit_type: str,
    hist_name: str,
    fit_name: str,
):
    positive = values[np.isfinite(values) & (values > 0.0)]
    if positive.size < 10:
        return None
    lo = 0.0
    hi = float(np.percentile(positive, 99.5) * 1.15)
    if hi <= lo:
        return None

    hist = ROOT.TH1F(hist_name, "Drifted endpoint density", 100, lo, hi)
    hist.SetDirectory(0)
    for value in positive:
        hist.Fill(float(value))

    if fit_type == "landau":
        func = ROOT.TF1(fit_name, "landau", lo, hi)
        fit_status = hist.Fit(func, "Q0S")
        if not func:
            return None
        return {
            "hist": hist,
            "func": func,
            "status": int(fit_status.Status()),
            "constant": float(func.GetParameter(0)),
            "mpv": float(func.GetParameter(1)),
            "sigma": float(func.GetParameter(2)),
            "lo": lo,
            "hi": hi,
            "fit_type": fit_type,
        }

    if fit_type == "gaussian":
        func = ROOT.TF1(fit_name, "gaus", lo, hi)
        mean = float(np.mean(positive))
        sigma = float(np.std(positive))
        func.SetParameters(float(hist.GetMaximum()), mean, max(sigma, 1.0e-3))
        fit_status = hist.Fit(func, "Q0S")
        if not func:
            return None
        return {
            "hist": hist,
            "func": func,
            "status": int(fit_status.Status()),
            "constant": float(func.GetParameter(0)),
            "mean": float(func.GetParameter(1)),
            "sigma": abs(float(func.GetParameter(2))),
            "lo": lo,
            "hi": hi,
            "fit_type": fit_type,
        }

    raise ValueError(f"unsupported fit type: {fit_type}")


def save_plots(
    muon: DensitySample,
    laser: DensitySample,
    out_prefix: Path,
    muon_fit_type: str,
    laser_fit_type: str,
    muon_fit_z_min: float | None,
    muon_fit_z_max: float | None,
    laser_fit_z_min: float | None,
    laser_fit_z_max: float | None,
) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    out_prefix.parent.mkdir(parents=True, exist_ok=True)

    fig, ax = plt.subplots(figsize=(8.8, 5.6), constrained_layout=True)
    bins_hi = max(
        float(np.percentile(muon.values, 99.5)) if muon.values.size else 1.0,
        float(np.percentile(laser.values, 99.5)) if laser.values.size else 1.0,
        1.0,
    )
    bins = np.linspace(0.0, bins_hi * 1.1, 90)
    ax.hist(muon.values, bins=bins, histtype="step", linewidth=1.8,
            label=f"muon, {muon.bin_width_cm:g} cm bins")
    ax.hist(laser.values, bins=bins, histtype="step", linewidth=1.8,
            label=f"laser, z=[{laser.z_min:.0f},{laser.z_max:.0f}] mm")
    muon_fit = fit_density(
        select_fit_values(muon, muon_fit_z_min, muon_fit_z_max),
        muon_fit_type,
        hist_name="h_muon_density_overlay",
        fit_name="f_muon_density_overlay",
    )
    if muon_fit is not None:
        x = np.linspace(max(bins[0], muon_fit["lo"]), min(bins[-1], muon_fit["hi"]), 400)
        y = np.asarray([muon_fit["func"].Eval(float(v)) for v in x], dtype=float)
        if muon_fit_type == "landau":
            muon_label = (
                f"muon Landau fit: MPV={muon_fit['mpv']:.2f}, sigma={muon_fit['sigma']:.2f}"
            )
        else:
            muon_label = (
                f"muon Gaussian fit: mean={muon_fit['mean']:.2f}, sigma={muon_fit['sigma']:.2f}"
            )
        ax.plot(
            x,
            y,
            color="tab:blue",
            linestyle="--",
            linewidth=1.5,
            label=muon_label,
        )
    laser_fit = fit_density(
        select_fit_values(laser, laser_fit_z_min, laser_fit_z_max),
        laser_fit_type,
        hist_name="h_laser_density_overlay",
        fit_name="f_laser_density_overlay",
    )
    if laser_fit is not None:
        x = np.linspace(max(bins[0], laser_fit["lo"]), min(bins[-1], laser_fit["hi"]), 400)
        y = np.asarray([laser_fit["func"].Eval(float(v)) for v in x], dtype=float)
        if laser_fit_type == "landau":
            laser_label = (
                f"laser Landau fit: MPV={laser_fit['mpv']:.2f}, sigma={laser_fit['sigma']:.2f}"
            )
        else:
            laser_label = (
                f"laser Gaussian fit: mean={laser_fit['mean']:.2f}, sigma={laser_fit['sigma']:.2f}"
            )
        ax.plot(
            x,
            y,
            color="tab:orange",
            linestyle="--",
            linewidth=1.5,
            label=laser_label,
        )
    ax.set_xlabel("drifted endpoints / cm")
    ax.set_ylabel("event-bin entries")
    ax.set_title("Track-unit drifted endpoint density")
    ax.legend()
    fig.savefig(out_prefix.with_name(out_prefix.name + "_density_overlay.png"), dpi=160)

    fig, ax = plt.subplots(figsize=(9.2, 5.6), constrained_layout=True)
    ax.errorbar(muon.z_centers, muon.profile_mean, yerr=muon.profile_rms,
                fmt="o", markersize=2.8, linewidth=1.0, label="muon")
    ax.errorbar(laser.z_centers, laser.profile_mean, yerr=laser.profile_rms,
                fmt="o", markersize=2.8, linewidth=1.0, label="laser")
    ax.axvspan(laser.z_min, laser.z_max, color="tab:orange", alpha=0.12,
               label="laser selected z window")
    ax.set_xlabel("z [mm]")
    ax.set_ylabel("mean drifted endpoints / cm")
    ax.set_title("Longitudinal density profile")
    ax.legend()
    fig.savefig(out_prefix.with_name(out_prefix.name + "_z_profile.png"), dpi=160)


def save_fit_plot(
    sample: DensitySample,
    out_path: Path,
    fit_type: str,
    fit_z_min: float | None,
    fit_z_max: float | None,
) -> dict | None:
    values = select_fit_values(sample, fit_z_min, fit_z_max)
    fit = fit_density(
        values,
        fit_type,
        hist_name=f"h_{fit_type}_density",
        fit_name=f"f_{fit_type}_density",
    )
    if fit is None:
        return None

    canvas = ROOT.TCanvas(f"c_{fit_type}_density", f"{fit_type.capitalize()} density", 900, 650)
    hist = fit["hist"]
    hist.GetXaxis().SetTitle("drifted endpoints / cm")
    hist.GetYaxis().SetTitle("event-bin entries")
    hist.Draw()
    func = fit["func"]
    if func:
        func.Draw("same")
    canvas.SaveAs(str(out_path))
    return fit


def print_summary(label: str, sample: DensitySample) -> None:
    values = sample.values
    print(f"[{label}]")
    print(f"  events:          {sample.n_events}")
    print(f"  z window [mm]:   [{sample.z_min:.3f}, {sample.z_max:.3f}]")
    print(f"  bin width [cm]:  {sample.bin_width_cm:.3f}")
    print(f"  event-bin values:{values.size}")
    print(f"  mean e-/cm:      {np.mean(values):.6g}")
    print(f"  rms e-/cm:       {np.std(values):.6g}")
    print(f"  median e-/cm:    {np.median(values):.6g}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--muon", type=Path, required=True)
    parser.add_argument("--laser", type=Path, required=True)
    parser.add_argument("--out-prefix", type=Path,
                        default=Path("analysis_plots/track_unit_density"))
    parser.add_argument("--bin-width-cm", type=float, default=1.0)
    parser.add_argument("--z-min", type=float, default=None,
                        help="lower z cut for the laser distribution [mm]")
    parser.add_argument("--z-max", type=float, default=None,
                        help="upper z cut for the laser distribution [mm]")
    parser.add_argument("--muon-z-min", type=float, default=None,
                        help="optional lower z cut for the muon distribution [mm]")
    parser.add_argument("--muon-z-max", type=float, default=None,
                        help="optional upper z cut for the muon distribution [mm]")
    parser.add_argument("--muon-fit-type", choices=("landau", "gaussian"), default="landau")
    parser.add_argument("--laser-fit-type", choices=("landau", "gaussian"), default="gaussian")
    parser.add_argument("--muon-fit-z-min", type=float, default=None,
                        help="lower z cut used to fill the muon fit histogram [mm]")
    parser.add_argument("--muon-fit-z-max", type=float, default=None,
                        help="upper z cut used to fill the muon fit histogram [mm]")
    parser.add_argument("--laser-fit-z-min", type=float, default=None,
                        help="lower z cut used to fill the laser fit histogram [mm]")
    parser.add_argument("--laser-fit-z-max", type=float, default=None,
                        help="upper z cut used to fill the laser fit histogram [mm]")
    args = parser.parse_args()

    if args.bin_width_cm <= 0.0:
        raise SystemExit("--bin-width-cm must be positive")

    muon = load_density(args.muon, args.muon_z_min, args.muon_z_max, args.bin_width_cm)
    laser = load_density(args.laser, args.z_min, args.z_max, args.bin_width_cm)

    save_plots(
        muon,
        laser,
        args.out_prefix,
        args.muon_fit_type,
        args.laser_fit_type,
        args.muon_fit_z_min,
        args.muon_fit_z_max,
        args.laser_fit_z_min,
        args.laser_fit_z_max,
    )
    save_csv(muon, args.out_prefix.with_name(args.out_prefix.name + "_muon_profile.csv"), "muon")
    save_csv(laser, args.out_prefix.with_name(args.out_prefix.name + "_laser_profile.csv"), "laser")
    fit = save_fit_plot(
        muon,
        args.out_prefix.with_name(args.out_prefix.name + f"_muon_{args.muon_fit_type}.png"),
        args.muon_fit_type,
        args.muon_fit_z_min,
        args.muon_fit_z_max,
    )
    laser_fit = save_fit_plot(
        laser,
        args.out_prefix.with_name(args.out_prefix.name + f"_laser_{args.laser_fit_type}.png"),
        args.laser_fit_type,
        args.laser_fit_z_min,
        args.laser_fit_z_max,
    )

    print_summary("muon", muon)
    if fit:
        if args.muon_fit_type == "landau":
            print(f"  Landau fit status: {fit['status']}")
            print(f"  Landau MPV e-/cm:  {fit['mpv']:.6g}")
            print(f"  Landau sigma:      {fit['sigma']:.6g}")
        else:
            print(f"  Gaussian fit status: {fit['status']}")
            print(f"  Gaussian mean e-/cm: {fit['mean']:.6g}")
            print(f"  Gaussian sigma:      {fit['sigma']:.6g}")
    print_summary("laser", laser)
    if laser_fit:
        if args.laser_fit_type == "landau":
            print(f"  Landau fit status: {laser_fit['status']}")
            print(f"  Landau MPV e-/cm:  {laser_fit['mpv']:.6g}")
            print(f"  Landau sigma:      {laser_fit['sigma']:.6g}")
        else:
            print(f"  Gaussian fit status: {laser_fit['status']}")
            print(f"  Gaussian mean e-/cm: {laser_fit['mean']:.6g}")
            print(f"  Gaussian sigma:      {laser_fit['sigma']:.6g}")
    print(f"[output] {args.out_prefix}_density_overlay.png")
    print(f"[output] {args.out_prefix}_z_profile.png")
    print(f"[output] {args.out_prefix}_muon_{args.muon_fit_type}.png")
    print(f"[output] {args.out_prefix}_laser_{args.laser_fit_type}.png")
    print(f"[output] {args.out_prefix}_muon_profile.csv")
    print(f"[output] {args.out_prefix}_laser_profile.csv")


if __name__ == "__main__":
    main()
