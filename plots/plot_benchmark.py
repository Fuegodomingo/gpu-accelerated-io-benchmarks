#!/usr/bin/env python3
"""Plot benchmark CSV files produced by the MultiGPU benchmarks.

The parser is intentionally data-driven: it discovers allocation types and
write-strategy bandwidth columns from the CSV header, so the same script works
with posix_bench.csv, mpi_bench.csv, and hdf5_bench.csv.

Examples
--------
    python plot_benchmark.py mpi_bench.csv
    python plot_benchmark.py hdf5_bench.csv --write-scale log
    python plot_benchmark.py mpi_bench.csv \
        --d2h-reference 36 --d2h-reference-label "36 GB/s xGMI2 theoretical" \
        --write-reference 844 --write-reference-label "scratch theoretical ≈ 844 GB/s"
"""

from __future__ import annotations

import argparse
import csv
import math
import re
from collections import defaultdict
from pathlib import Path
from typing import Iterable

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np


# Shared visual style
BACKGROUND = "#1a1a2e"
TEXT = "#ccccdd"
MUTED = "#888899"

plt.rcParams.update({
    "figure.facecolor": BACKGROUND,
    "axes.facecolor": BACKGROUND,
    "axes.edgecolor": "#444466",
    "axes.labelcolor": TEXT,
    "xtick.color": TEXT,
    "ytick.color": TEXT,
    "grid.color": "#333355",
    "grid.linewidth": 0.8,
    "text.color": TEXT,
    "font.family": "DejaVu Sans",
    "font.size": 12,
})

COLORS = {
    # Host allocations
    "Pageable": "#e07b39",
    "Pinned": "#4caf7d",
    "Managed": "#7b68ee",

    # MPI / HDF5 strategies
    "Independent": "#e07b39",
    "Collective": "#4caf7d",
    "Direct I/O": "#e0559a",
    "Collective Chunked": "#e0b039",

    # POSIX strategies
    "Individual + merge": "#7b68ee",
    "pwrite offsets": "#4caf7d",
    "Mutex": "#e0559a",
}

FALLBACK_COLORS = [
    "#4a8fe0", "#e0b039", "#39c9c2", "#e0559a",
    "#7b68ee", "#e07b39", "#4caf7d", "#c9e039",
]

PREFIX_LABELS = {
    "Col": "Collective",
    "Dir": "Direct I/O",
    "Chk": "Collective Chunked",
    "PWrite": "pwrite offsets",
    "Mutex": "Mutex",
}


# CSV parsing
def parse_size_bytes(label: str) -> float:
    """Convert labels such as '64 MB', '4 GB', '64 MiB' to bytes for sorting."""
    match = re.fullmatch(r"\s*([0-9]+(?:\.[0-9]+)?)\s*([KMGT]?i?B)\s*", label, re.I)
    if not match:
        return math.inf

    value = float(match.group(1))
    unit = match.group(2).upper()
    factors = {
        "B": 1,
        "KB": 1000,
        "MB": 1024**2,      # benchmark labels mean binary sizes
        "GB": 1024**3,
        "TB": 1024**4,
        "KIB": 1024,
        "MIB": 1024**2,
        "GIB": 1024**3,
        "TIB": 1024**4,
    }
    return value * factors[unit]


def read_csv(path: Path) -> tuple[list[dict[str, str]], list[str]]:
    with path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        if reader.fieldnames is None:
            raise ValueError(f"{path} has no CSV header")
        rows = list(reader)
        return rows, reader.fieldnames


def discover_strategy_prefixes(fieldnames: Iterable[str]) -> list[str]:
    """Discover all write-bandwidth columns except D2H_GB/s."""
    prefixes = []
    for field in fieldnames:
        if not field.endswith("_GB/s") or field == "D2H_GB/s":
            continue
        prefixes.append(field.removesuffix("_GB/s"))
    return prefixes


def infer_benchmark_kind(prefixes: Iterable[str]) -> str:
    prefixes = set(prefixes)
    if {"PWrite", "Mutex"} & prefixes:
        return "POSIX"
    if "Chk" in prefixes:
        return "HDF5"
    return "MPI-IO"


def strategy_label(prefix: str, benchmark_kind: str) -> str:
    if prefix == "Ind":
        return "Individual + merge" if benchmark_kind == "POSIX" else "Independent"
    return PREFIX_LABELS.get(prefix, prefix.replace("_", " "))


def to_float(value: str | None) -> float:
    if value is None or value.strip() == "":
        return float("nan")
    try:
        return float(value)
    except ValueError:
        return float("nan")


def organize_rows(rows: list[dict[str, str]]) -> tuple[list[str], list[str], dict[str, dict[str, dict[str, float]]]]:
    """Return ordered sizes, allocations, and data[alloc][size][column]."""
    if not rows:
        raise ValueError("CSV contains no data rows")

    required = {"Size", "Alloc", "D2H_GB/s"}
    missing = required - rows[0].keys()
    if missing:
        raise ValueError(f"CSV is missing required column(s): {', '.join(sorted(missing))}")

    seen_sizes = []
    seen_allocs = []
    data: dict[str, dict[str, dict[str, float]]] = defaultdict(dict)

    for row in rows:
        size = row["Size"].strip()
        alloc = row["Alloc"].strip()
        if size not in seen_sizes:
            seen_sizes.append(size)
        if alloc not in seen_allocs:
            seen_allocs.append(alloc)
        data[alloc][size] = {key: to_float(value) for key, value in row.items() if key}

    # Sort parseable benchmark sizes numerically. Unknown labels retain their input order.
    original_index = {label: i for i, label in enumerate(seen_sizes)}
    sizes = sorted(seen_sizes, key=lambda s: (parse_size_bytes(s), original_index[s]))
    return sizes, seen_allocs, data


# Plot helpers
def series_color(label: str, index: int) -> str:
    return COLORS.get(label, FALLBACK_COLORS[index % len(FALLBACK_COLORS)])


def average_from_size(values: list[float], sizes: list[str], min_size_bytes: float) -> float:
    selected = [
        value for value, size in zip(values, sizes)
        if parse_size_bytes(size) >= min_size_bytes and np.isfinite(value)
    ]
    if not selected:
        selected = [value for value in values if np.isfinite(value)]
    return float(np.mean(selected)) if selected else float("nan")


def reference_dict(value: float | None, label: str | None) -> dict | None:
    if value is None:
        return None
    return {
        "y": value,
        "label": label or f"{value:g} GB/s reference",
    }


def choose_scale(requested: str, series: dict[str, list[float]], reference: dict | None) -> str:
    if requested != "auto":
        return requested

    finite = [v for values in series.values() for v in values if np.isfinite(v) and v > 0]
    if not finite:
        return "linear"
    high = max(finite)
    low = min(finite)
    if reference is not None:
        high = max(high, float(reference["y"]))
    return "log" if high / low >= 25 else "linear"


def make_plot(
    *,
    title: str,
    series: dict[str, list[float]],
    sizes: list[str],
    out_path: Path,
    yscale: str = "linear",
    reference: dict | None = None,
    x_label: str = "Transfer size per rank / GPU",
    avg_min_size_bytes: float = 64 * 1024**2,
    avg_min_size_label: str = "64 MB",
    note: str | None = None,
) -> None:
    if not series:
        return

    x = np.arange(len(sizes))
    fig, ax = plt.subplots(figsize=(13, 7))
    fig.patch.set_facecolor(BACKGROUND)

    finite_data = [v for vals in series.values() for v in vals if np.isfinite(v) and v > 0]
    if not finite_data:
        plt.close(fig)
        return

    all_for_limits = list(finite_data)
    if reference is not None and reference["y"] > 0:
        all_for_limits.append(reference["y"])

    if yscale == "log":
        ax.set_yscale("log")
        ymin = max(min(all_for_limits) / 1.7, 1e-4)
        ymax = max(all_for_limits) * 1.35
    else:
        ymin = 0.0
        ymax = max(all_for_limits) * 1.18
    ax.set_ylim(ymin, ymax)

    if reference is not None:
        ref_y = float(reference["y"])
        ax.axhline(ref_y, color=MUTED, linewidth=1.4, linestyle="--", zorder=1)
        y_text = ref_y * 1.08 if yscale == "log" else ref_y + (ymax - ymin) * 0.015
        ax.text(
            len(sizes) - 0.05,
            y_text,
            reference["label"],
            ha="right",
            va="bottom",
            fontsize=10,
            color=MUTED,
        )

    avg_entries = []
    for index, (label, values) in enumerate(series.items()):
        color = series_color(label, index)
        vals = np.asarray(values, dtype=float)
        ax.plot(
            x,
            vals,
            marker="o",
            markersize=6,
            linewidth=2.2,
            color=color,
            label=label,
            zorder=3,
        )

        avg = average_from_size(values, sizes, avg_min_size_bytes)
        if np.isfinite(avg):
            suffix = f"{avg:.2f} GB/s avg"
            if reference is not None and reference["y"] > 0:
                suffix += f" · {avg / reference['y'] * 100.0:.2f}%"
            avg_entries.append((suffix, color))

    for i, (text, color) in enumerate(avg_entries):
        ax.text(
            0.985,
            0.965 - i * 0.055,
            text,
            transform=ax.transAxes,
            ha="right",
            va="top",
            fontsize=10,
            color=color,
            fontweight="bold",
        )

    ax.set_xticks(x)
    ax.set_xticklabels(sizes, fontsize=11)
    ax.set_xlim(-0.3, len(sizes) - 0.7 + 1.2)
    ax.yaxis.set_major_locator(
        ticker.LogLocator(base=10) if yscale == "log" else ticker.MaxNLocator(nbins=10)
    )
    ax.set_xlabel(x_label, fontsize=13)
    ax.set_ylabel("Bandwidth (GB/s)" + (", log scale" if yscale == "log" else ""), fontsize=13)
    ax.set_title(title, fontsize=14, pad=14, color="white")
    ax.grid(True, axis="both", which="both" if yscale == "log" else "major", alpha=0.45)
    ax.legend(
        loc="upper left",
        framealpha=0.25,
        edgecolor="#555577",
        facecolor="#22224a",
        fontsize=11,
    )

    footer = f"avg over ≥ {avg_min_size_label}"
    if note:
        footer = f"{note}\n{footer}"
    ax.text(
        0.99,
        0.02,
        footer,
        transform=ax.transAxes,
        ha="right",
        va="bottom",
        fontsize=9,
        color=MUTED,
    )

    fig.tight_layout()
    fig.savefig(out_path, dpi=150, bbox_inches="tight", facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"Saved: {out_path}")


# Main plotting workflow
def plot_csv(args: argparse.Namespace) -> None:
    csv_path = Path(args.csv).expanduser().resolve()
    rows, fieldnames = read_csv(csv_path)
    prefixes = discover_strategy_prefixes(fieldnames)
    if not prefixes:
        raise ValueError("No write-strategy '*_GB/s' columns were found")

    kind = infer_benchmark_kind(prefixes)
    sizes, allocs, data = organize_rows(rows)

    out_dir = Path(args.output_dir) if args.output_dir else csv_path.parent / "plots" / csv_path.stem
    out_dir.mkdir(parents=True, exist_ok=True)

    d2h_ref = reference_dict(args.d2h_reference, args.d2h_reference_label)
    write_ref = reference_dict(args.write_reference, args.write_reference_label)

    avg_min_size_bytes = parse_size_bytes(args.avg_min_size)
    if not np.isfinite(avg_min_size_bytes):
        raise ValueError(f"Could not parse --avg-min-size {args.avg_min_size!r}")

    benchmark_title = args.title or f"MI250X — {kind} benchmark"
    x_label = args.x_label or ("Transfer size per GPU" if kind == "POSIX" else "Transfer size per MPI rank")

    # Plot 1: D2H allocation comparison.
    d2h_series = {
        alloc: [data[alloc].get(size, {}).get("D2H_GB/s", float("nan")) for size in sizes]
        for alloc in allocs
    }
    d2h_scale = choose_scale(args.d2h_scale, d2h_series, d2h_ref)
    make_plot(
        title=f"{benchmark_title}\nD2H bandwidth by host allocation",
        series=d2h_series,
        sizes=sizes,
        out_path=out_dir / "d2h_by_allocation.png",
        yscale=d2h_scale,
        reference=d2h_ref,
        x_label=x_label,
        avg_min_size_bytes=avg_min_size_bytes,
        avg_min_size_label=args.avg_min_size,
    )

    # Plot 2+: write strategies for each allocation type.
    strategy_names = {prefix: strategy_label(prefix, kind) for prefix in prefixes}
    for alloc in allocs:
        write_series = {
            strategy_names[prefix]: [
                data[alloc].get(size, {}).get(f"{prefix}_GB/s", float("nan"))
                for size in sizes
            ]
            for prefix in prefixes
        }
        write_scale = choose_scale(args.write_scale, write_series, write_ref)
        filename = re.sub(r"[^a-z0-9]+", "_", alloc.lower()).strip("_")
        make_plot(
            title=f"{benchmark_title}\n{alloc} write bandwidth by strategy",
            series=write_series,
            sizes=sizes,
            out_path=out_dir / f"{filename}_write_bandwidth.png",
            yscale=write_scale,
            reference=write_ref,
            x_label=x_label,
            avg_min_size_bytes=avg_min_size_bytes,
            avg_min_size_label=args.avg_min_size,
            note=("filesystem reference is aggregate" if write_ref is not None else None),
        )

    # Optional complementary view: one strategy at a time, comparing allocations.
    if args.strategy_plots:
        for prefix in prefixes:
            label = strategy_names[prefix]
            alloc_series = {
                alloc: [
                    data[alloc].get(size, {}).get(f"{prefix}_GB/s", float("nan"))
                    for size in sizes
                ]
                for alloc in allocs
            }
            scale = choose_scale(args.write_scale, alloc_series, write_ref)
            filename = re.sub(r"[^a-z0-9]+", "_", label.lower()).strip("_")
            make_plot(
                title=f"{benchmark_title}\n{label}: host allocation comparison",
                series=alloc_series,
                sizes=sizes,
                out_path=out_dir / f"strategy_{filename}.png",
                yscale=scale,
                reference=write_ref,
                x_label=x_label,
                avg_min_size_bytes=avg_min_size_bytes,
                avg_min_size_label=args.avg_min_size,
            )

    print(f"\nDetected benchmark: {kind}")
    print(f"Allocations: {', '.join(allocs)}")
    print("Strategies: " + ", ".join(strategy_names[p] for p in prefixes))
    print(f"Plots written to: {out_dir}")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Generate consistent benchmark plots from MultiGPU CSV output."
    )
    parser.add_argument("csv", help="Path to posix_bench.csv, mpi_bench.csv, or hdf5_bench.csv")
    parser.add_argument("-o", "--output-dir", help="Output directory (default: <csv dir>/plots/<csv stem>)")
    parser.add_argument("--title", help="Custom first line for plot titles")
    parser.add_argument("--x-label", help="Override the x-axis label")
    parser.add_argument(
        "--avg-min-size",
        default="64 MB",
        help="Only sizes at/above this label contribute to the displayed average (default: 64 MB)",
    )
    parser.add_argument(
        "--d2h-scale", choices=("auto", "linear", "log"), default="auto",
        help="D2H y-axis scale (default: auto)",
    )
    parser.add_argument(
        "--write-scale", choices=("auto", "linear", "log"), default="auto",
        help="Write-bandwidth y-axis scale (default: auto)",
    )
    parser.add_argument("--d2h-reference", type=float, help="Optional D2H reference bandwidth in GB/s")
    parser.add_argument("--d2h-reference-label", help="Label for --d2h-reference")
    parser.add_argument("--write-reference", type=float, help="Optional filesystem reference bandwidth in GB/s")
    parser.add_argument("--write-reference-label", help="Label for --write-reference")
    parser.add_argument(
        "--strategy-plots",
        action="store_true",
        help="Also generate one plot per write strategy comparing host allocations",
    )
    return parser


def main() -> None:
    args = build_parser().parse_args()
    plot_csv(args)


if __name__ == "__main__":
    main()
