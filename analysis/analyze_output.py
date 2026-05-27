#!/usr/bin/env python3
"""Quick PyROOT analysis for FC0 laser/TPC simulation output.

The script reads the ROOTOutput format written by tpcmc:
  - clusters tree: primary electrons from Heed or the laser generator
  - anode tree: drift endpoints on the anode plane

It uses PyROOT for I/O and matplotlib for plots. No C++ ROOT macro is needed.
"""

from __future__ import annotations

import argparse
import math
import os
import statistics
from collections import Counter
from pathlib import Path
from typing import Iterable

import ROOT

ROOT.gROOT.SetBatch(True)

os.environ.setdefault("MPLCONFIGDIR", "/tmp/fc0-lasersim-matplotlib")

try:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError:
    plt = None


def as_list(values: Iterable) -> list:
    """Convert a PyROOT std::vector branch to a normal Python list."""
    return [x for x in values]


def mean(values: list[float]) -> float:
    return statistics.fmean(values) if values else math.nan


def rms(values: list[float]) -> float:
    if not values:
        return math.nan
    mu = mean(values)
    return math.sqrt(statistics.fmean([(x - mu) ** 2 for x in values]))


def require_tree(root_file: ROOT.TFile, name: str):
    tree = root_file.Get(name)
    if not tree:
        print(f"[warn] Tree '{name}' not found.")
        return None
    return tree


def read_clusters(tree) -> dict:
    data = {
        "event_id": [],
        "n_electrons": [],
        "n_saved": [],
        "x": [],
        "y": [],
        "z": [],
        "t": [],
        "ekin": [],
    }

    if tree is None:
        return data

    for entry in tree:
        xs = as_list(entry.cl_x)
        ys = as_list(entry.cl_y)
        zs = as_list(entry.cl_z)
        ts = as_list(entry.cl_t)
        es = as_list(entry.cl_ekin)

        data["event_id"].append(int(entry.eventId))
        data["n_electrons"].append(int(entry.nElectrons))
        data["n_saved"].append(len(xs))
        data["x"].extend(xs)
        data["y"].extend(ys)
        data["z"].extend(zs)
        data["t"].extend(ts)
        data["ekin"].extend(es)

    return data


def read_anode(tree) -> dict:
    data = {
        "event_id": [],
        "n_primaries": [],
        "n_hits": [],
        "z": [],
        "y": [],
        "t": [],
        "status": [],
        "pad_z": [],
        "pad_y": [],
    }

    if tree is None:
        return data

    for entry in tree:
        zs = as_list(entry.anode_z)
        ys = as_list(entry.anode_y)
        ts = as_list(entry.anode_t)
        status = [int(x) for x in entry.anode_status]
        pad_z = [int(x) for x in entry.anode_padZ]
        pad_y = [int(x) for x in entry.anode_padY]

        data["event_id"].append(int(entry.eventId))
        data["n_primaries"].append(int(entry.nPrimaries))
        data["n_hits"].append(int(entry.nAnodeHits))
        data["z"].extend(zs)
        data["y"].extend(ys)
        data["t"].extend(ts)
        data["status"].extend(status)
        data["pad_z"].extend(pad_z)
        data["pad_y"].extend(pad_y)

    return data


def print_summary(clusters: dict, anode: dict) -> None:
    print("\n=== FC0 laser/TPC ROOT output summary ===")

    print("\n[clusters]")
    print(f"  events:          {len(clusters['event_id'])}")
    print(f"  saved entries:   {len(clusters['x'])}")
    if clusters["n_electrons"]:
        print(f"  branch total:    {sum(clusters['n_electrons'])}")
        print(f"  electrons/event: mean={mean(clusters['n_electrons']):.2f}, "
              f"rms={rms(clusters['n_electrons']):.2f}")
    if clusters["n_saved"]:
        print(f"  saved/event:     mean={mean(clusters['n_saved']):.2f}, "
              f"rms={rms(clusters['n_saved']):.2f}")
    if clusters["n_electrons"] and clusters["n_saved"]:
        if sum(clusters["n_electrons"]) != len(clusters["x"]):
            print("  note: nElectrons and saved vector entries differ; "
                  "use saved entries for plotted cluster positions.")
    if clusters["x"]:
        print(f"  x [mm]:          min={min(clusters['x']):.3f}, "
              f"max={max(clusters['x']):.3f}, mean={mean(clusters['x']):.3f}")
        print(f"  y [mm]:          min={min(clusters['y']):.3f}, "
              f"max={max(clusters['y']):.3f}, mean={mean(clusters['y']):.3f}")
        print(f"  z [mm]:          min={min(clusters['z']):.3f}, "
              f"max={max(clusters['z']):.3f}, mean={mean(clusters['z']):.3f}")
        print(f"  ekin [eV]:       mean={mean(clusters['ekin']):.3f}, "
              f"rms={rms(clusters['ekin']):.3f}")

    print("\n[anode]")
    print(f"  events:          {len(anode['event_id'])}")
    print(f"  endpoints total: {len(anode['z'])}")
    if anode["n_primaries"]:
        total_primaries = sum(anode["n_primaries"])
        total_hits = sum(anode["n_hits"])
        efficiency = total_hits / total_primaries if total_primaries else math.nan
        print(f"  primaries total: {total_primaries}")
        print(f"  good hits total: {total_hits}")
        print(f"  hit efficiency:  {efficiency:.6f}")
    if anode["status"]:
        print(f"  status counts:   {dict(Counter(anode['status']))}")
    if anode["t"]:
        print(f"  arrival t [ns]:  min={min(anode['t']):.3f}, "
              f"max={max(anode['t']):.3f}, mean={mean(anode['t']):.3f}")


def save_plots(clusters: dict, anode: dict, outdir: Path) -> None:
    if plt is None:
        print("[warn] matplotlib is not installed; skipping plots.")
        return

    outdir.mkdir(parents=True, exist_ok=True)

    if clusters["z"] and clusters["y"]:
        plt.figure(figsize=(7, 6))
        plt.scatter(clusters["z"], clusters["y"], s=2, alpha=0.35)
        plt.xlabel("cluster z [mm]")
        plt.ylabel("cluster y [mm]")
        plt.title("Primary electron positions at generation")
        plt.grid(True, alpha=0.25)
        plt.tight_layout()
        plt.savefig(outdir / "clusters_yz.png", dpi=160)
        plt.close()

    if clusters["x"]:
        plt.figure(figsize=(7, 4))
        plt.hist(clusters["x"], bins=80, histtype="stepfilled", alpha=0.75)
        plt.xlabel("cluster x [mm]")
        plt.ylabel("entries")
        plt.title("Primary electron x distribution")
        plt.grid(True, alpha=0.25)
        plt.tight_layout()
        plt.savefig(outdir / "clusters_x_hist.png", dpi=160)
        plt.close()

    if anode["z"] and anode["y"]:
        plt.figure(figsize=(7, 6))
        if anode["t"]:
            points = plt.scatter(anode["z"], anode["y"], c=anode["t"], s=3,
                                 alpha=0.45, cmap="viridis")
            cbar = plt.colorbar(points)
            cbar.set_label("arrival time [ns]")
        else:
            plt.scatter(anode["z"], anode["y"], s=3, alpha=0.45)
        plt.xlabel("anode z [mm]")
        plt.ylabel("anode y [mm]")
        plt.title("Anode drift endpoints")
        plt.grid(True, alpha=0.25)
        plt.tight_layout()
        plt.savefig(outdir / "anode_hits_yz.png", dpi=160)
        plt.close()

    if anode["t"]:
        plt.figure(figsize=(7, 4))
        plt.hist(anode["t"], bins=80, histtype="stepfilled", alpha=0.75)
        plt.xlabel("arrival time [ns]")
        plt.ylabel("entries")
        plt.title("Anode arrival-time distribution")
        plt.grid(True, alpha=0.25)
        plt.tight_layout()
        plt.savefig(outdir / "anode_time_hist.png", dpi=160)
        plt.close()

    if anode["pad_z"] and anode["pad_y"]:
        plt.figure(figsize=(7, 6))
        plt.hist2d(anode["pad_z"], anode["pad_y"], bins=60, cmap="magma")
        plt.xlabel("pad z index")
        plt.ylabel("pad y index")
        plt.title("Direct pad occupancy")
        cbar = plt.colorbar()
        cbar.set_label("entries")
        plt.tight_layout()
        plt.savefig(outdir / "pad_occupancy.png", dpi=160)
        plt.close()

    print(f"[plots] Wrote plots to {outdir}")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Analyze fc0-lasersim output.root with PyROOT and matplotlib."
    )
    parser.add_argument("root_file", nargs="?", default="output.root",
                        help="input ROOT file written by tpcmc")
    parser.add_argument("-o", "--outdir", default="analysis_plots",
                        help="directory for PNG plots")
    args = parser.parse_args()

    root_path = Path(args.root_file)
    if not root_path.exists():
        raise FileNotFoundError(f"Input ROOT file not found: {root_path}")

    root_file = ROOT.TFile.Open(str(root_path), "READ")
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"Cannot open ROOT file: {root_path}")

    clusters_tree = require_tree(root_file, "clusters")
    anode_tree = require_tree(root_file, "anode")

    clusters = read_clusters(clusters_tree)
    anode = read_anode(anode_tree)

    print_summary(clusters, anode)
    save_plots(clusters, anode, Path(args.outdir))

    root_file.Close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
