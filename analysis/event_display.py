#!/usr/bin/env python3
"""Interactive event display for fc0-lasersim ROOT output.

The display shows one event at a time:
  - primary electrons projected on the pad plane (Z-Y);
  - drift endpoints on the pad plane;
  - active waveform pads coloured by peak ADC;
  - waveform for the pad under the cursor, or the pad selected by click.

Optional reconstructed tracks can be overlaid from a CSV file with columns:
event, track, z_mm, y_mm
"""

from __future__ import annotations

import argparse
import csv
import os
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable

os.environ.setdefault("MPLCONFIGDIR", "/tmp/fc0-lasersim-matplotlib")

import ROOT

ROOT.gROOT.SetBatch(True)


def as_list(values: Iterable) -> list:
    return [x for x in values]


def has_branch(tree, name: str) -> bool:
    return bool(tree and tree.GetBranch(name))


@dataclass(frozen=True)
class PadKey:
    eram: int
    pad_z: int
    pad_y: int


@dataclass
class EventData:
    event_id: int
    cl_z: list[float] = field(default_factory=list)
    cl_y: list[float] = field(default_factory=list)
    an_z: list[float] = field(default_factory=list)
    an_y: list[float] = field(default_factory=list)
    an_t: list[float] = field(default_factory=list)
    an_status: list[int] = field(default_factory=list)
    wf_peak: dict[PadKey, int] = field(default_factory=dict)
    wf_peak_sample: dict[PadKey, int] = field(default_factory=dict)
    waveforms: dict[PadKey, list[int]] = field(default_factory=dict)
    n_samples: int = 0
    sample_period_ns: float = 0.0


class HATGeometry:
    pad_width_z = 11.16
    pad_height_y = 10.05
    pad_gap = 0.12
    n_pads_z = 36
    n_pads_y = 32
    n_erams_per_row = 4
    n_eram_rows = 2
    pad_area_z = n_pads_z * pad_width_z + (n_pads_z - 1) * pad_gap
    pad_area_y = n_pads_y * pad_height_y + (n_pads_y - 1) * pad_gap
    eram_y = [170.5, -170.5]
    eram_z = [-631.5, -210.5, 210.5, 631.5]
    quartz_eram = 3

    def erams(self):
        for row in range(self.n_eram_rows):
            for col in range(self.n_erams_per_row):
                eram = row * self.n_erams_per_row + col
                yield eram, self.eram_y[row], self.eram_z[col]

    def pad_bounds(self, key: PadKey) -> tuple[float, float, float, float]:
        row = key.eram // self.n_erams_per_row
        col = key.eram % self.n_erams_per_row
        center_y = self.eram_y[row]
        center_z = self.eram_z[col]
        pitch_z = self.pad_width_z + self.pad_gap
        pitch_y = self.pad_height_y + self.pad_gap
        z_min = center_z - self.pad_area_z / 2.0 + key.pad_z * pitch_z
        z_max = z_min + self.pad_width_z
        y_min = center_y - self.pad_area_y / 2.0 + key.pad_y * pitch_y
        y_max = y_min + self.pad_height_y
        return z_min, z_max, y_min, y_max

    def pad_center(self, key: PadKey) -> tuple[float, float]:
        z_min, z_max, y_min, y_max = self.pad_bounds(key)
        return 0.5 * (z_min + z_max), 0.5 * (y_min + y_max)

    def locate_pad(self, z: float, y: float) -> PadKey | None:
        pitch_z = self.pad_width_z + self.pad_gap
        pitch_y = self.pad_height_y + self.pad_gap
        for eram, center_y, center_z in self.erams():
            if eram == self.quartz_eram:
                continue
            local_z = z - center_z + self.pad_area_z / 2.0
            local_y = y - center_y + self.pad_area_y / 2.0
            if local_z < 0 or local_z > self.pad_area_z:
                continue
            if local_y < 0 or local_y > self.pad_area_y:
                continue
            pad_z = int(local_z / pitch_z)
            pad_y = int(local_y / pitch_y)
            if 0 <= pad_z < self.n_pads_z and 0 <= pad_y < self.n_pads_y:
                z_min, z_max, y_min, y_max = self.pad_bounds(PadKey(eram, pad_z, pad_y))
                if z_min <= z <= z_max and y_min <= y <= y_max:
                    return PadKey(eram, pad_z, pad_y)
        return None


class RootEventReader:
    def __init__(self, path: Path):
        self.path = path
        self.root_file = ROOT.TFile.Open(str(path), "READ")
        if not self.root_file or self.root_file.IsZombie():
            raise OSError(f"Cannot open ROOT file: {path}")

        self.clusters = self.root_file.Get("clusters")
        self.anode = self.root_file.Get("anode")
        self.waveforms = self.root_file.Get("waveforms")

        self.cluster_index = self._build_index(self.clusters)
        self.anode_index = self._build_index(self.anode)
        self.waveform_index = self._build_index(self.waveforms)
        self.event_ids = sorted(
            set(self.cluster_index) | set(self.anode_index) | set(self.waveform_index)
        )
        if not self.event_ids:
            raise RuntimeError(f"No readable events in {path}")

    @staticmethod
    def _build_index(tree) -> dict[int, int]:
        if not tree or not has_branch(tree, "eventId"):
            return {}
        index = {}
        for entry_index in range(tree.GetEntries()):
            tree.GetEntry(entry_index)
            index[int(tree.eventId)] = entry_index
        return index

    def read_event(self, event_id: int) -> EventData:
        data = EventData(event_id=event_id)

        if self.clusters and event_id in self.cluster_index:
            self.clusters.GetEntry(self.cluster_index[event_id])
            data.cl_z = [float(x) for x in self.clusters.cl_z]
            data.cl_y = [float(x) for x in self.clusters.cl_y]

        if self.anode and event_id in self.anode_index:
            self.anode.GetEntry(self.anode_index[event_id])
            data.an_z = [float(x) for x in self.anode.anode_z]
            data.an_y = [float(x) for x in self.anode.anode_y]
            data.an_t = [float(x) for x in self.anode.anode_t]
            data.an_status = [int(x) for x in self.anode.anode_status]

        if self.waveforms and event_id in self.waveform_index:
            self.waveforms.GetEntry(self.waveform_index[event_id])
            data.n_samples = int(self.waveforms.nSamples)
            data.sample_period_ns = float(self.waveforms.samplePeriodNs)
            erams = [int(x) for x in self.waveforms.wf_eram]
            pad_z = [int(x) for x in self.waveforms.wf_padZ]
            pad_y = [int(x) for x in self.waveforms.wf_padY]
            peaks = [int(x) for x in self.waveforms.wf_peakAdc]
            peak_samples = [int(x) for x in self.waveforms.wf_peakSample]
            starts = [int(x) for x in self.waveforms.wf_adcStart]
            lengths = [int(x) for x in self.waveforms.wf_adcLength]
            adc = [int(x) for x in self.waveforms.wf_adc]

            for i, eram in enumerate(erams):
                key = PadKey(eram, pad_z[i], pad_y[i])
                start = starts[i]
                stop = start + lengths[i]
                data.wf_peak[key] = peaks[i]
                data.wf_peak_sample[key] = peak_samples[i]
                data.waveforms[key] = adc[start:stop]

        return data


def load_reco_tracks(path: Path | None) -> dict[int, dict[str, list[tuple[float, float]]]]:
    tracks: dict[int, dict[str, list[tuple[float, float]]]] = defaultdict(lambda: defaultdict(list))
    if path is None:
        return tracks

    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            event_id = int(row["event"])
            track_id = row.get("track") or row.get("track_id") or "track"
            z = float(row.get("z_mm", row.get("z", "nan")))
            y = float(row.get("y_mm", row.get("y", "nan")))
            tracks[event_id][track_id].append((z, y))
    return tracks


class EventDisplay:
    def __init__(self, reader: RootEventReader, reco_tracks, plt, patches, collections, widgets):
        self.reader = reader
        self.reco_tracks = reco_tracks
        self.plt = plt
        self.patches = patches
        self.collections = collections
        self.widgets = widgets
        self.geometry = HATGeometry()
        self.event_pos = 0
        self.event: EventData | None = None
        self.selected_key: PadKey | None = None
        self.selected_patch = None
        self.colorbar = None

        self.fig = plt.figure(figsize=(13.5, 7.5))
        grid = self.fig.add_gridspec(
            2, 2, width_ratios=(1.35, 1.0), height_ratios=(1.0, 0.12),
            hspace=0.24, wspace=0.35
        )
        self.ax_event = self.fig.add_subplot(grid[0, 0])
        self.ax_waveform = self.fig.add_subplot(grid[0, 1])
        self.ax_prev = self.fig.add_subplot(grid[1, 0])
        self.ax_next = self.fig.add_subplot(grid[1, 1])
        self.prev_button = widgets.Button(self.ax_prev, "Previous event")
        self.next_button = widgets.Button(self.ax_next, "Next event")
        self.prev_button.on_clicked(lambda event: self.previous_event())
        self.next_button.on_clicked(lambda event: self.next_event())
        self.fig.canvas.mpl_connect("motion_notify_event", self.on_motion)
        self.fig.canvas.mpl_connect("button_press_event", self.on_click)
        self.fig.canvas.mpl_connect("key_press_event", self.on_key)

    def show(self, save_path: Path | None = None) -> None:
        self.draw_event()
        if save_path:
            self.fig.savefig(save_path, dpi=160)
            print(f"[event-display] Wrote {save_path}")
        else:
            self.plt.show()

    def current_event_id(self) -> int:
        return self.reader.event_ids[self.event_pos]

    def draw_event(self) -> None:
        self.event = self.reader.read_event(self.current_event_id())
        self.selected_key = None
        self.ax_event.clear()
        self.ax_waveform.clear()
        if self.colorbar is not None:
            self.colorbar.remove()
            self.colorbar = None

        self.draw_geometry()
        self.draw_waveform_pads()
        self.draw_electrons()
        self.draw_reco_tracks()

        event_label = f"event {self.event.event_id}"
        self.ax_event.set_title(
            f"{event_label}: pad-plane projection"
        )
        self.ax_event.set_xlabel("Z [mm]")
        self.ax_event.set_ylabel("Y [mm]")
        self.ax_event.set_xlim(-940, 940)
        self.ax_event.set_ylim(-420, 420)
        self.ax_event.set_aspect("equal", adjustable="box")
        self.ax_event.grid(True, alpha=0.18)
        self.ax_event.legend(loc="upper left", fontsize=8)

        if self.event.waveforms:
            first_key = max(self.event.wf_peak, key=self.event.wf_peak.get)
            self.update_waveform(first_key)
        else:
            self.draw_empty_waveform("No waveform tree/pads for this event")

        self.fig.suptitle(
            f"{self.reader.path}  |  event {self.event_pos + 1}/{len(self.reader.event_ids)}"
        )
        self.fig.canvas.draw_idle()

    def draw_geometry(self) -> None:
        for eram, center_y, center_z in self.geometry.erams():
            z_min = center_z - self.geometry.pad_area_z / 2.0
            y_min = center_y - self.geometry.pad_area_y / 2.0
            rect = self.patches.Rectangle(
                (z_min, y_min), self.geometry.pad_area_z, self.geometry.pad_area_y,
                fill=False, edgecolor="0.45", linewidth=0.8, linestyle="--"
            )
            self.ax_event.add_patch(rect)
            self.ax_event.text(center_z, y_min - 16, f"ERAM {eram}",
                               ha="center", va="top", fontsize=8, color="0.35")

    def draw_waveform_pads(self) -> None:
        if not self.event or not self.event.waveforms:
            return
        rects = []
        peaks = []
        for key, peak in self.event.wf_peak.items():
            z_min, z_max, y_min, y_max = self.geometry.pad_bounds(key)
            rects.append(self.patches.Rectangle((z_min, y_min), z_max - z_min, y_max - y_min))
            peaks.append(peak)
        collection = self.collections.PatchCollection(
            rects, cmap="magma", alpha=0.52, edgecolor="none"
        )
        collection.set_array(peaks)
        self.ax_event.add_collection(collection)
        self.colorbar = self.fig.colorbar(collection, ax=self.ax_event, fraction=0.032, pad=0.012)
        self.colorbar.set_label("peak ADC", labelpad=8)

    def draw_electrons(self) -> None:
        if not self.event:
            return
        if self.event.cl_z and self.event.cl_y:
            self.ax_event.scatter(
                self.event.cl_z, self.event.cl_y, s=12, marker=".",
                color="tab:blue", alpha=0.35, label="primary e- projected"
            )
        if self.event.an_z and self.event.an_y:
            if self.event.an_t:
                self.ax_event.scatter(
                    self.event.an_z, self.event.an_y, s=18, marker="x",
                    c=self.event.an_t, cmap="viridis", alpha=0.75,
                    label="drift endpoints"
                )
            else:
                self.ax_event.scatter(
                    self.event.an_z, self.event.an_y, s=18, marker="x",
                    color="tab:green", alpha=0.75, label="drift endpoints"
                )

    def draw_reco_tracks(self) -> None:
        if not self.event:
            return
        tracks = self.reco_tracks.get(self.event.event_id, {})
        first = True
        for points in tracks.values():
            if len(points) < 2:
                continue
            zs = [p[0] for p in points]
            ys = [p[1] for p in points]
            self.ax_event.plot(
                zs, ys, color="black", linewidth=1.8,
                label="reconstructed track" if first else None
            )
            first = False

    def update_waveform(self, key: PadKey) -> None:
        if not self.event:
            return
        self.selected_key = key
        waveform = self.event.waveforms.get(key)
        if waveform is None:
            self.draw_empty_waveform(
                f"ERAM {key.eram}, padZ {key.pad_z}, padY {key.pad_y}: no waveform"
            )
            return

        self.ax_waveform.clear()
        dt = self.event.sample_period_ns or 1.0
        times = [i * dt for i in range(len(waveform))]
        self.ax_waveform.plot(times, waveform, color="tab:red", linewidth=1.4)
        self.ax_waveform.set_xlabel("time [ns]")
        self.ax_waveform.set_ylabel("ADC")
        self.ax_waveform.set_title(
            f"ERAM {key.eram}, padZ {key.pad_z}, padY {key.pad_y} | "
            f"peak {self.event.wf_peak.get(key, 0)} ADC"
        )
        self.ax_waveform.grid(True, alpha=0.25)
        self.mark_selected_pad(key)
        self.fig.canvas.draw_idle()

    def draw_empty_waveform(self, message: str) -> None:
        self.ax_waveform.clear()
        self.ax_waveform.text(0.5, 0.5, message, ha="center", va="center",
                              transform=self.ax_waveform.transAxes)
        self.ax_waveform.set_axis_off()
        self.fig.canvas.draw_idle()

    def mark_selected_pad(self, key: PadKey) -> None:
        if self.selected_patch is not None:
            self.selected_patch.remove()
            self.selected_patch = None
        z_min, z_max, y_min, y_max = self.geometry.pad_bounds(key)
        self.selected_patch = self.patches.Rectangle(
            (z_min, y_min), z_max - z_min, y_max - y_min,
            fill=False, edgecolor="cyan", linewidth=2.2
        )
        self.ax_event.add_patch(self.selected_patch)

    def on_motion(self, event) -> None:
        if event.inaxes != self.ax_event or event.xdata is None or event.ydata is None:
            return
        key = self.geometry.locate_pad(event.xdata, event.ydata)
        if key and self.event and key in self.event.waveforms and key != self.selected_key:
            self.update_waveform(key)

    def on_click(self, event) -> None:
        if event.inaxes != self.ax_event or event.xdata is None or event.ydata is None:
            return
        key = self.geometry.locate_pad(event.xdata, event.ydata)
        if key:
            self.update_waveform(key)

    def on_key(self, event) -> None:
        if event.key in ("right", "n"):
            self.next_event()
        elif event.key in ("left", "p"):
            self.previous_event()

    def next_event(self) -> None:
        self.event_pos = (self.event_pos + 1) % len(self.reader.event_ids)
        self.draw_event()

    def previous_event(self) -> None:
        self.event_pos = (self.event_pos - 1) % len(self.reader.event_ids)
        self.draw_event()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Interactive event display for fc0-lasersim ROOT output."
    )
    parser.add_argument("root_file", nargs="?", default="output.root",
                        help="ROOT file written by tpcmc")
    parser.add_argument("--event", type=int, default=None,
                        help="event id to show first")
    parser.add_argument("--reco-csv", type=Path, default=None,
                        help="optional reconstructed-track CSV: event,track,z_mm,y_mm")
    parser.add_argument("--backend", default=None,
                        help="matplotlib backend, for example TkAgg or QtAgg")
    parser.add_argument("--save", type=Path, default=None,
                        help="write a PNG snapshot instead of opening the GUI")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    root_path = Path(args.root_file)
    if not root_path.exists():
        raise FileNotFoundError(f"Input ROOT file not found: {root_path}")

    import matplotlib

    if args.save:
        matplotlib.use("Agg")
    elif args.backend:
        matplotlib.use(args.backend)

    import matplotlib.pyplot as plt
    import matplotlib.collections as collections
    import matplotlib.patches as patches
    import matplotlib.widgets as widgets

    reader = RootEventReader(root_path)
    reco_tracks = load_reco_tracks(args.reco_csv)
    display = EventDisplay(reader, reco_tracks, plt, patches, collections, widgets)
    if args.event is not None:
        if args.event not in reader.event_ids:
            raise ValueError(f"Event {args.event} not found in {root_path}")
        display.event_pos = reader.event_ids.index(args.event)
    display.show(save_path=args.save)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
