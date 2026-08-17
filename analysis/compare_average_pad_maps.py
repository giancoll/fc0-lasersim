#!/usr/bin/env python3
"""Compare average drifted endpoint occupancy maps for two FC0/HAT ROOT files."""

from __future__ import annotations

import argparse
import os
from pathlib import Path

os.environ.setdefault("MPLCONFIGDIR", "/tmp/fc0-lasersim-matplotlib")

import numpy as np
import ROOT


PADS_Z_PER_ERAM = 36
PADS_Y_PER_ERAM = 32
ERAMS_PER_ROW = 4
GLOBAL_PADS_Z = PADS_Z_PER_ERAM * ERAMS_PER_ROW
GLOBAL_PADS_Y = PADS_Y_PER_ERAM * 2


def global_pad(eram: int, pad_z: int, pad_y: int):
    if eram < 0 or eram >= 8:
        return None
    if pad_z < 0 or pad_z >= PADS_Z_PER_ERAM:
        return None
    if pad_y < 0 or pad_y >= PADS_Y_PER_ERAM:
        return None
    row = eram // ERAMS_PER_ROW
    col = eram % ERAMS_PER_ROW
    return col * PADS_Z_PER_ERAM + pad_z, row * PADS_Y_PER_ERAM + pad_y


def load_average_map(path: Path) -> tuple[np.ndarray, int, float]:
    root_file = ROOT.TFile.Open(str(path))
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"cannot open {path}")
    tree = root_file.Get("anode")
    if not tree:
        raise RuntimeError(f"{path} has no anode tree")

    occupancy = np.zeros((GLOBAL_PADS_Y, GLOBAL_PADS_Z), dtype=float)
    total_endpoints = 0
    n_events = int(tree.GetEntries())

    for event in tree:
        for eram, pad_z, pad_y in zip(event.anode_eram, event.anode_padZ, event.anode_padY):
            gp = global_pad(int(eram), int(pad_z), int(pad_y))
            if gp is None:
                continue
            gz, gy = gp
            occupancy[gy, gz] += 1.0
            total_endpoints += 1

    if n_events > 0:
        occupancy /= n_events
    mean_endpoints = total_endpoints / n_events if n_events else 0.0
    return occupancy, n_events, mean_endpoints


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--muon", type=Path, required=True)
    parser.add_argument("--laser", type=Path, required=True)
    parser.add_argument("--out", type=Path, default=Path("analysis_plots/muon_vs_laser_avg_pad_maps.png"))
    parser.add_argument("--backend", default="Agg")
    args = parser.parse_args()

    import matplotlib
    matplotlib.use(args.backend)
    import matplotlib.pyplot as plt

    muon, n_mu, mu_mean = load_average_map(args.muon)
    laser, n_la, la_mean = load_average_map(args.laser)
    vmax = max(float(muon.max()), float(laser.max()), 1.0)

    args.out.parent.mkdir(parents=True, exist_ok=True)

    fig, axes = plt.subplots(1, 3, figsize=(17, 5.2), constrained_layout=True)
    panels = [
        (muon, f"Muon tracks\\n{n_mu} events, mean endpoints/event = {mu_mean:.1f}"),
        (laser, f"Laser tracks\\n{n_la} events, mean endpoints/event = {la_mean:.1f}"),
        (laser - muon, "Laser - muon\\nmean endpoints / pad / event"),
    ]

    for ax, (data, title) in zip(axes, panels):
        if "Laser - muon" in title:
            lim = max(abs(float(data.min())), abs(float(data.max())), 1.0)
            im = ax.imshow(data, origin="lower", aspect="auto", cmap="coolwarm",
                           vmin=-lim, vmax=lim)
        else:
            im = ax.imshow(data, origin="lower", aspect="auto", cmap="viridis",
                           vmin=0.0, vmax=vmax)
        ax.set_title(title)
        ax.set_xlabel("global pad Z column")
        ax.set_ylabel("global pad Y row")
        fig.colorbar(im, ax=ax, shrink=0.85)

    fig.savefig(args.out, dpi=160)
    print(args.out)


if __name__ == "__main__":
    main()
