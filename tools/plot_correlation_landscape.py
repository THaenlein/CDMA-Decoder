#!/usr/bin/env python
"""Generate an insight-focused CDMA correlation landscape visualization.

Creates:
1) Static plot (PNG + PDF): heatmap of correlation by satellite and code phase,
   with decoded hits and peak margins.
2) Interactive plot (HTML): zoomable heatmap with decoded hit markers.
"""

from __future__ import annotations

import argparse
import pathlib
import re
import subprocess
from dataclasses import dataclass
from typing import Iterable, List

import numpy as np
import matplotlib.pyplot as plt

try:
    from plotly.subplots import make_subplots
    import plotly.graph_objects as go
except ImportError as exc:  # pragma: no cover
    raise SystemExit(
        "plotly is required for interactive HTML output. "
        "Install it with: pip install plotly"
    ) from exc


CHIP_SEQUENCE_LENGTH = 1023
NUM_SATELLITES = 24
MAX_DEVIATION = 65
REGISTER_LENGTH = 10
SHIFT_INDICES_TOP = (2,)
SHIFT_INDICES_BOTTOM = (1, 2, 5, 7, 8)

# Same satellite tap mapping used in the repository implementations.
SHIFT_REGISTER_SUM_INDICES = (
    (1, 5), (2, 6), (3, 7), (4, 8), (0, 8), (1, 9), (0, 7), (1, 8),
    (2, 9), (1, 2), (2, 3), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9),
    (0, 3), (1, 4), (2, 5), (3, 6), (4, 7), (5, 8), (0, 2), (3, 5),
)


@dataclass
class Detection:
    satellite_id: int
    offset: int
    bit: int
    corr_value: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--input",
        type=pathlib.Path,
        default=pathlib.Path("C++/gps_sequence.txt"),
        help="Chip sequence input file.",
    )
    parser.add_argument(
        "--decoder-exe",
        type=pathlib.Path,
        default=pathlib.Path("C++/build/Release/CDMA_Decoder_Cpp.exe"),
        help="Optional decoder executable for parity check.",
    )
    parser.add_argument(
        "--out-dir",
        type=pathlib.Path,
        default=pathlib.Path("artifacts/plots"),
        help="Directory for output plot files.",
    )
    parser.add_argument(
        "--prefix",
        type=str,
        default="correlation_landscape",
        help="Filename prefix for generated artifacts.",
    )
    return parser.parse_args()


def load_chip_sequence(path: pathlib.Path) -> np.ndarray:
    raw = path.read_text(encoding="utf-8")
    chip = np.fromstring(raw, sep=" ", dtype=np.int32)
    if chip.size != CHIP_SEQUENCE_LENGTH:
        raise ValueError(
            f"Expected {CHIP_SEQUENCE_LENGTH} chip values, got {chip.size} in {path}."
        )
    return chip


def shift_mother_sequence(seq: np.ndarray, xor_indices: Iterable[int]) -> np.ndarray:
    new_element = seq[-1]
    for idx in xor_indices:
        new_element ^= seq[idx]
    out = np.empty_like(seq)
    out[1:] = seq[:-1]
    out[0] = new_element
    return out


def generate_gold_sequence(tap_a: int, tap_b: int, length: int) -> np.ndarray:
    top = np.ones(REGISTER_LENGTH, dtype=np.int8)
    bottom = np.ones(REGISTER_LENGTH, dtype=np.int8)
    sequence_bits = np.empty(length, dtype=np.int8)

    for i in range(length):
        mother_first = top[-1]
        mother_second = bottom[tap_a] ^ bottom[tap_b]
        sequence_bits[i] = mother_first ^ mother_second

        top = shift_mother_sequence(top, SHIFT_INDICES_TOP)
        bottom = shift_mother_sequence(bottom, SHIFT_INDICES_BOTTOM)

    # Convert bool-like {0,1} to decoder-compatible {-1,+1}
    return np.where(sequence_bits == 1, 1, -1).astype(np.int32)


def generate_all_gold_sequences(length: int) -> np.ndarray:
    sequences = np.empty((NUM_SATELLITES, length), dtype=np.int32)
    for sat_idx, (tap_a, tap_b) in enumerate(SHIFT_REGISTER_SUM_INDICES):
        sequences[sat_idx] = generate_gold_sequence(tap_a, tap_b, length)
    return sequences


def correlation_landscape(chip: np.ndarray, sequences: np.ndarray) -> np.ndarray:
    sat_count, length = sequences.shape
    corr = np.empty((sat_count, length), dtype=np.int32)
    for sat_idx in range(sat_count):
        seq = sequences[sat_idx]
        for offset in range(length):
            corr[sat_idx, offset] = int(np.dot(np.roll(seq, -offset), chip))
    return corr


def detect_from_landscape(corr: np.ndarray, threshold: int) -> List[Detection]:
    detections: List[Detection] = []
    for sat_idx, row in enumerate(corr):
        for offset, value in enumerate(row):
            if abs(int(value)) > threshold:
                detections.append(
                    Detection(
                        satellite_id=sat_idx + 1,
                        offset=offset,
                        bit=1 if value > 0 else 0,
                        corr_value=int(value),
                    )
                )
                break
    return detections


def best_peak_margins(corr: np.ndarray, threshold: int) -> np.ndarray:
    best_abs = np.max(np.abs(corr), axis=1)
    return best_abs - threshold


def render_static_plot(
    corr: np.ndarray,
    detections: List[Detection],
    threshold: int,
    out_png: pathlib.Path,
    out_pdf: pathlib.Path,
    sender_count: int,
) -> None:
    max_abs = int(np.max(np.abs(corr)))
    sat_ids = np.arange(1, NUM_SATELLITES + 1)
    margins = best_peak_margins(corr, threshold)

    fig = plt.figure(figsize=(16, 9))
    gs = fig.add_gridspec(
        nrows=2,
        ncols=2,
        width_ratios=(3.5, 1.6),
        height_ratios=(2.0, 1.3),
        wspace=0.25,
        hspace=0.25,
    )

    ax_heat = fig.add_subplot(gs[:, 0])
    im = ax_heat.imshow(
        corr,
        aspect="auto",
        cmap="coolwarm",
        vmin=-max_abs,
        vmax=max_abs,
        origin="lower",
        interpolation="nearest",
    )
    ax_heat.set_title("CDMA Correlation Landscape (satellite x code phase)")
    ax_heat.set_xlabel("Code phase offset")
    ax_heat.set_ylabel("Satellite ID")
    ax_heat.set_yticks(np.arange(NUM_SATELLITES))
    ax_heat.set_yticklabels(sat_ids)
    cbar = fig.colorbar(im, ax=ax_heat, fraction=0.03, pad=0.02)
    cbar.set_label("Correlation score")

    if detections:
        xs = [d.offset for d in detections]
        ys = [d.satellite_id - 1 for d in detections]
        ax_heat.scatter(
            xs,
            ys,
            s=60,
            c="black",
            marker="x",
            linewidths=1.8,
            label="Decoded hit",
        )
        ax_heat.legend(loc="upper right")

    ax_text = fig.add_subplot(gs[0, 1])
    ax_text.axis("off")
    lines = [
        f"Input length: {CHIP_SEQUENCE_LENGTH}",
        f"Inferred sending satellites: {sender_count}",
        f"Threshold: {threshold}",
        f"Decoded hits: {len(detections)}",
        "",
        "Decoded satellites:",
    ]
    if detections:
        lines.extend(
            f"  S{d.satellite_id:02d}: bit={d.bit}, offset={d.offset:3d}, corr={d.corr_value:4d}"
            for d in detections
        )
    else:
        lines.append("  (none)")
    ax_text.text(
        0.0,
        1.0,
        "\n".join(lines),
        va="top",
        ha="left",
        family="monospace",
        fontsize=10,
    )

    ax_margin = fig.add_subplot(gs[1, 1])
    colors = ["tab:orange" if m > 0 else "tab:gray" for m in margins]
    ax_margin.barh(sat_ids, margins, color=colors)
    ax_margin.axvline(0, color="black", linewidth=1)
    ax_margin.set_title("Peak margin above threshold")
    ax_margin.set_xlabel("max(|corr|) - threshold")
    ax_margin.set_ylabel("Satellite")
    ax_margin.invert_yaxis()

    fig.suptitle(
        "Insight view: active satellites emerge as clear, threshold-exceeding correlation peaks",
        fontsize=12,
    )

    out_png.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_png, dpi=200, bbox_inches="tight")
    fig.savefig(out_pdf, bbox_inches="tight")
    plt.close(fig)


def render_interactive_plot(
    corr: np.ndarray,
    detections: List[Detection],
    threshold: int,
    out_html: pathlib.Path,
    sender_count: int,
) -> None:
    sat_ids = np.arange(1, NUM_SATELLITES + 1)
    margins = best_peak_margins(corr, threshold)

    fig = make_subplots(
        rows=2,
        cols=1,
        row_heights=[0.78, 0.22],
        vertical_spacing=0.10,
        subplot_titles=(
            "CDMA Correlation Landscape (zoom/hover)",
            "Peak margin above threshold",
        ),
    )

    fig.add_trace(
        go.Heatmap(
            z=corr,
            x=np.arange(corr.shape[1]),
            y=sat_ids,
            colorscale="RdBu_r",
            zmid=0,
            colorbar=dict(title="corr"),
            hovertemplate="Satellite %{y}<br>Offset %{x}<br>Corr %{z}<extra></extra>",
        ),
        row=1,
        col=1,
    )

    if detections:
        fig.add_trace(
            go.Scatter(
                x=[d.offset for d in detections],
                y=[d.satellite_id for d in detections],
                mode="markers",
                marker=dict(color="black", symbol="x", size=9),
                name="Decoded hit",
                hovertext=[
                    f"S{d.satellite_id:02d}, bit={d.bit}, corr={d.corr_value}"
                    for d in detections
                ],
                hoverinfo="text+x+y",
            ),
            row=1,
            col=1,
        )

    fig.add_trace(
        go.Bar(
            x=margins,
            y=sat_ids,
            orientation="h",
            marker_color=["#ff7f0e" if m > 0 else "#808080" for m in margins],
            hovertemplate="Satellite %{y}<br>Margin %{x}<extra></extra>",
            name="Margin",
        ),
        row=2,
        col=1,
    )

    fig.update_yaxes(title_text="Satellite", row=1, col=1)
    fig.update_xaxes(title_text="Code phase offset", row=1, col=1)
    fig.update_yaxes(title_text="Satellite", row=2, col=1, autorange="reversed")
    fig.update_xaxes(title_text="max(|corr|) - threshold", row=2, col=1)

    fig.update_layout(
        title=(
            "CDMA Insight Plot: correlation structure, threshold separability, and decoded hits"
            f" | senders={sender_count}, threshold={threshold}"
        ),
        height=900,
        template="plotly_white",
    )

    out_html.parent.mkdir(parents=True, exist_ok=True)
    fig.write_html(str(out_html), include_plotlyjs="cdn")


def parse_decoder_output(output: str) -> List[Detection]:
    pattern = re.compile(
        r"Satellite\s+(\d+)\s+has sent bit\s+([01])\s+\(delta =\s*(\d+)\)"
    )
    detections: List[Detection] = []
    for line in output.splitlines():
        m = pattern.search(line)
        if not m:
            continue
        detections.append(
            Detection(
                satellite_id=int(m.group(1)),
                bit=int(m.group(2)),
                offset=int(m.group(3)),
                corr_value=0,
            )
        )
    return detections


def maybe_validate_with_decoder(
    decoder_exe: pathlib.Path, input_path: pathlib.Path, py_detections: List[Detection]
) -> None:
    if not decoder_exe.exists():
        print(f"[INFO] Decoder executable not found at {decoder_exe}; skipping parity check.")
        return

    result = subprocess.run(
        [str(decoder_exe), str(input_path)],
        capture_output=True,
        text=True,
        check=True,
    )
    exe_detections = parse_decoder_output(result.stdout)

    py_triplets = [(d.satellite_id, d.bit, d.offset) for d in py_detections]
    exe_triplets = [(d.satellite_id, d.bit, d.offset) for d in exe_detections]

    if py_triplets == exe_triplets:
        print("[OK] Python detections match decoder output.")
    else:
        print("[WARN] Python detections differ from decoder output.")
        print(f"  Python: {py_triplets}")
        print(f"  Decoder: {exe_triplets}")


def main() -> None:
    args = parse_args()

    chip = load_chip_sequence(args.input)
    sender_count = int(np.max(np.abs(chip)))
    threshold = int(CHIP_SEQUENCE_LENGTH - MAX_DEVIATION * (sender_count - 1))

    sequences = generate_all_gold_sequences(CHIP_SEQUENCE_LENGTH)
    corr = correlation_landscape(chip, sequences)
    detections = detect_from_landscape(corr, threshold)

    out_dir = args.out_dir
    out_png = out_dir / f"{args.prefix}.png"
    out_pdf = out_dir / f"{args.prefix}.pdf"
    out_html = out_dir / f"{args.prefix}.html"

    render_static_plot(corr, detections, threshold, out_png, out_pdf, sender_count)
    render_interactive_plot(corr, detections, threshold, out_html, sender_count)
    maybe_validate_with_decoder(args.decoder_exe, args.input, detections)

    print(f"[OK] Wrote static PNG: {out_png}")
    print(f"[OK] Wrote static PDF: {out_pdf}")
    print(f"[OK] Wrote interactive HTML: {out_html}")


if __name__ == "__main__":
    main()
