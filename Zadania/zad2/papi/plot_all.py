#!/usr/bin/python3

import csv
import os
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure
import numpy as np
import re

OUTPUT_FILES_DIR = "."
COLUMNS = ["Time (s)", "Total Cycles", "Total Instructions", "L1 Cache Misses", "L2 Cache Hits"]

def get_series_entries(fname_pattern):
    entries = []
    for entry in os.scandir(OUTPUT_FILES_DIR):
        if entry.is_file() and fname_pattern.match(entry.name):
            entries.append(entry)
    return entries


def get_legend_str(fname):
    return fname.replace("output_", "").replace(".csv", "")


def get_labels_order(labels):
    A = [(int(re.search(r'\d+', labels[i]).group()), i) for i in range(len(labels))]
    return [e[1] for e in sorted(A)]


def plot_column(data, column_name, xlabel, ylabel, save_fname):
    figure(figsize=(9, 6), dpi=80)
    colormap = plt.cm.nipy_spectral
    sorted_labels = sorted(data.keys(), key=lambda x: int(re.search(r'\d+', x).group()))
    colors = colormap(np.linspace(0, 1, len(sorted_labels)))
    markers = ['o', 's', 'D', '^', 'v', '<', '>', 'p']
    ax = plt.gca()
    plt.title(f"{ylabel} vs {xlabel}")

    for i, label in enumerate(sorted_labels):
        x, y = data[label]
        plt.plot(x, y, label=label, color=colors[i], linestyle='-', marker=markers[i % len(markers)])

    max_value = max(max(y) for x, y in data.values()) if data else 0
    ax.set_xlim([min(min(x) for x, y in data.values()), max(max(x) for x, y in data.values())])
    ax.set_ylim([0, max_value])
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)

    ax.grid(True, which='both', linestyle='--', linewidth=0.5)

    for line in np.linspace(0, max_value, 10):
        plt.text(ax.get_xlim()[1] * 1.01, line, s=f'{int(line)}', verticalalignment='center', color='gray')

    handles, labels = ax.get_legend_handles_labels()
    order = get_labels_order(labels)
    plt.legend([handles[idx] for idx in order], [labels[idx] for idx in order], loc="upper left", bbox_to_anchor=(1.04, 1))
    plt.subplots_adjust(right=0.75)

    plt.savefig(save_fname, bbox_inches="tight")
    print(f"saved as {save_fname}")


def plot(fname_pattern_str, save_fnames):
    print(f"plotting {fname_pattern_str}")
    fname_pattern = re.compile(fname_pattern_str)
    series_entries = get_series_entries(fname_pattern)

    data = {col: {} for col in COLUMNS}

    for entry in series_entries:
        with open(entry.path) as f:
            line_reader = csv.reader(f, delimiter=',')
            next(line_reader)  # skip the first line (headers)
            for row in line_reader:
                size = int(row[0])
                for i, col in enumerate(COLUMNS, start=1):
                    if get_legend_str(entry.name) not in data[col]:
                        data[col][get_legend_str(entry.name)] = ([], [])
                    data[col][get_legend_str(entry.name)][0].append(size)
                    data[col][get_legend_str(entry.name)][1].append(float(row[i]))

    for col, save_fname in save_fnames.items():
        plot_column(data[col], col, "Matrix size", col, save_fname)


plot("^output_ge[1-8].csv$", {
    "Time (s)": "plot_ge_time.png",
    "Total Cycles": "plot_ge_total_cycles.png",
    "Total Instructions": "plot_ge_total_instructions.png",
    "L1 Cache Misses": "plot_ge_l1_cache_misses.png",
    "L2 Cache Hits": "plot_ge_l2_cache_hits.png",
})
plot("^output_o2_ge[1-8].csv$", {
    "Time (s)": "plot_o2_ge_time.png",
    "Total Cycles": "plot_o2_ge_total_cycles.png",
    "Total Instructions": "plot_o2_ge_total_instructions.png",
    "L1 Cache Misses": "plot_o2_ge_l1_cache_misses.png",
    "L2 Cache Hits": "plot_o2_ge_l2_cache_hits.png",
})