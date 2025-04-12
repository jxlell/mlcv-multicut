import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Toggle this to True for a single average bar
show_average = True

# Load and sort the CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test.csv")
df_sorted = df.sort_values(by="filename")

# Define time components
compression_labels = [
    "read_img_time",
    "region_color_dfs_time",
    "dpcm_huffman_time",
    "dpcm_huffman_bitstring_time",
    "tree_construction_time",
    "tree_bitstring_time"
]

decompression_labels = [
    "rebuild_dpcm_huffman_time",
    "decode_colors_time",
    "assemble_tree_paths_time",
    "reconstruct_tree_edgebits_time",
    "dfs_reconstruction_time"
]

# Blue shades for compression, red for decompression
compression_colors = ['#1f77b4', '#aec7e8', '#ff7f0e', '#ffbb78', '#2ca02c', '#98df8a']
decompression_colors = ['#d62728', '#ff9896', '#9467bd', '#c5b0d5', '#8c564b']

# Prepare average data if needed
if show_average:
    df_avg = pd.DataFrame({label: [df_sorted[label].mean()] for label in compression_labels + decompression_labels})
    x = np.array([0])
    xtick_labels = ["Average"]
else:
    df_avg = df_sorted
    x = np.arange(len(df_sorted))
    xtick_labels = df_sorted["filename"]

bar_width = 0.6

# Plotting
fig, ax = plt.subplots(figsize=(14, 7))
bottom = np.zeros(len(df_avg))

# Compression bars
for label, color in zip(compression_labels, compression_colors):
    ax.bar(x, df_avg[label], bar_width, bottom=bottom, label=label, color=color)
    bottom += df_avg[label]

# Decompression bars
for label, color in zip(decompression_labels, decompression_colors):
    ax.bar(x, df_avg[label], bar_width, bottom=bottom, label=label, color=color)
    bottom += df_avg[label]

# Calculate total compression time per bar
compression_total = np.zeros(len(df_avg))
for label in compression_labels:
    compression_total += df_avg[label]

# Draw a horizontal line for compression total
ax.hlines(compression_total, x - bar_width / 2, x + bar_width / 2, colors='blue', linestyles='dashed', label="Compression Total")


# Aesthetics
ax.set_ylabel("Time (ms)")
title = "Average Compression and Decompression Time" if show_average else "Compression and Decompression Time per Image"
ax.set_title(title)
ax.set_xticks(x)
ax.set_xticklabels(xtick_labels, rotation=90 if not show_average else 0, fontsize=8)
ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0))
plt.tight_layout()
plt.grid(True, axis='y', linestyle='--', alpha=0.5)

plt.show()
