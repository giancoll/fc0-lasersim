#!/usr/bin/env python3
"""Summarize laser-track source and drift metrics from fc0-lasersim ROOT output."""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path

import ROOT


def dot(a, b):
    return sum(x * y for x, y in zip(a, b))


def norm(v):
    return math.sqrt(dot(v, v))


def unit(v):
    n = norm(v)
    if n <= 0.0:
        raise ValueError("zero-length direction vector")
    return tuple(x / n for x in v)


def cross(a, b):
    return (
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    )


def mean(values):
    return sum(values) / len(values) if values else 0.0


def rms(values):
    if not values:
        return 0.0
    m = mean(values)
    return math.sqrt(sum((x - m) ** 2 for x in values) / len(values))


def percentile(values, q):
    if not values:
        return 0.0
    ordered = sorted(values)
    idx = min(len(ordered) - 1, max(0, int(round(q * (len(ordered) - 1)))))
    return ordered[idx]


def load_laser_line(config_path: Path):
    cfg = json.loads(config_path.read_text())
    laser = cfg["generator"]["laser"]
    pos = laser["position"]
    direction = laser["direction"]
    origin = (
        float(pos.get("x_start_mm", 0.0)),
        float(pos.get("y_mm", 0.0)),
        float(pos.get("z_mm", 0.0)),
    )
    d = unit((
        float(direction.get("dx", 1.0)),
        float(direction.get("dy", 0.0)),
        float(direction.get("dz", 0.0)),
    ))

    ref = (0.0, 1.0, 0.0)
    if abs(dot(d, ref)) > 0.9:
        ref = (1.0, 0.0, 0.0)
    e1 = unit(cross(d, ref))
    e2 = unit(cross(d, e1))
    return cfg, origin, d, e1, e2


def project(point, origin, d, e1, e2):
    rel = tuple(p - o for p, o in zip(point, origin))
    s = dot(rel, d)
    u = dot(rel, e1)
    v = dot(rel, e2)
    r = math.sqrt(u * u + v * v)
    return s, u, v, r


def summarize_clusters(root_file, origin, d, e1, e2):
    tree = root_file.Get("clusters")
    if not tree:
        print("[clusters] missing")
        return

    saved_counts = []
    total_counts = []
    all_s = []
    all_u = []
    all_v = []
    all_r = []

    for event in tree:
        xs = list(event.cl_x)
        ys = list(event.cl_y)
        zs = list(event.cl_z)
        saved_counts.append(len(xs))
        total_counts.append(int(event.nElectrons))
        for point in zip(xs, ys, zs):
            s, u, v, r = project(point, origin, d, e1, e2)
            all_s.append(s)
            all_u.append(u)
            all_v.append(v)
            all_r.append(r)

    if not all_s:
        print("[clusters] no saved source electrons")
        return

    length_cm = (max(all_s) - min(all_s)) / 10.0
    entries = tree.GetEntries()
    saved_per_event = mean(saved_counts)
    total_per_event = mean(total_counts)

    print("[clusters]")
    print(f"  events:              {entries}")
    print(f"  mean generated/event:{total_per_event:.3f}")
    print(f"  mean saved/event:    {saved_per_event:.3f}")
    print(f"  source length [cm]:  {length_cm:.3f}")
    if length_cm > 0.0:
        print(f"  generated e-/cm:     {total_per_event / length_cm:.6g}")
        print(f"  saved e-/cm:         {saved_per_event / length_cm:.6g}")
    print(f"  s range [mm]:        [{min(all_s):.3f}, {max(all_s):.3f}]")
    print(f"  transverse u rms [mm]: {rms(all_u):.6g}")
    print(f"  transverse v rms [mm]: {rms(all_v):.6g}")
    print(f"  radial rms [mm]:       {math.sqrt(mean([x * x for x in all_r])):.6g}")
    print(f"  radial p68 [mm]:       {percentile(all_r, 0.68):.6g}")
    print(f"  radial p95 [mm]:       {percentile(all_r, 0.95):.6g}")


def summarize_anode(root_file, origin, d, e1, e2):
    tree = root_file.Get("anode")
    if not tree:
        print("[anode] missing")
        return

    all_y = []
    all_z = []
    all_t = []
    statuses = {}
    nprim = []
    nhits = []

    # For drift endpoints, x is not stored; summarize pad-plane residual for
    # horizontal Z-directed laser tracks using configured Y/Z line.
    line_y = origin[1]
    for event in tree:
        nprim.append(int(event.nPrimaries))
        nhits.append(int(event.nAnodeHits))
        for y, z, t, status in zip(event.anode_y, event.anode_z, event.anode_t, event.anode_status):
            all_y.append(float(y))
            all_z.append(float(z))
            all_t.append(float(t))
            statuses[int(status)] = statuses.get(int(status), 0) + 1

    print("[anode]")
    print(f"  events:              {tree.GetEntries()}")
    print(f"  mean primaries/event:{mean(nprim):.3f}")
    print(f"  mean good hits/event:{mean(nhits):.3f}")
    print(f"  endpoints total:     {len(all_t)}")
    print(f"  status counts:       {statuses}")
    if all_t:
        print(f"  arrival t [ns]:      [{min(all_t):.3f}, {max(all_t):.3f}], mean={mean(all_t):.3f}, rms={rms(all_t):.3f}")
        print(f"  anode y rms about configured line [mm]: {rms([y - line_y for y in all_y]):.6g}")
        print(f"  anode z range [mm]:  [{min(all_z):.3f}, {max(all_z):.3f}]")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root_file", type=Path)
    parser.add_argument("--config", type=Path, required=True,
                        help="JSON config used to generate the ROOT file")
    args = parser.parse_args()

    _, origin, d, e1, e2 = load_laser_line(args.config)
    root_file = ROOT.TFile.Open(str(args.root_file))
    if not root_file or root_file.IsZombie():
        raise SystemExit(f"cannot open ROOT file: {args.root_file}")

    print(f"ROOT file: {args.root_file}")
    print(f"Config:    {args.config}")
    print(f"Line origin [mm]: {origin}")
    print(f"Line direction:   {d}")
    summarize_clusters(root_file, origin, d, e1, e2)
    summarize_anode(root_file, origin, d, e1, e2)


if __name__ == "__main__":
    main()
