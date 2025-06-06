import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as patches

# Toggle between individual bars and averaged bar
show_average = False  # <<<< CHANGE THIS TO True for average-only view

# Load your CSV
df = pd.read_csv("/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_new.csv")

# Sort by filename or other metric
df_sorted = df.sort_values(by="filename")

# Prepare data
if show_average:
    # Compute mean values
    tree_path = [df_sorted["tree_path_bits"].mean()]
    tree_start = [df_sorted["tree_start_bits"].mean()]
    paths_dir = [df_sorted["paths2bit_direction_bits"].mean()]
    paths_start = [df_sorted["paths2bit_start_bits"].mean()]
    rle_dir = [df_sorted["rle_direction_bits"].mean()]
    x = np.array([0])  # Single bar for average
else:
    tree_path = df_sorted["tree_path_bits"].values
    tree_start = df_sorted["tree_start_bits"].values
    paths_dir = df_sorted["paths2bit_direction_bits"].values
    paths_start = df_sorted["paths2bit_start_bits"].values
    rle_dir = df_sorted["rle_direction_bits"].values
    x = np.arange(len(df_sorted))

bar_width = 0.25

# Determine the best (lowest total bits) method for each image
if not show_average:
    total_tree = tree_path + tree_start
    total_2bit = paths_dir + paths_start
    total_rle = rle_dir + paths_start

    best_method = np.argmin(np.stack([total_tree, total_2bit, total_rle]), axis=0)
    method_colors = ['tab:blue', 'tab:green', 'purple']

# Create the plot
fig, ax1 = plt.subplots(figsize=(14, 6))

# Tree bars (leftmost)
ax1.bar(x - bar_width, tree_path, bar_width, label="tree_path_bits", color='tab:blue')
ax1.bar(x - bar_width, tree_start, bar_width, bottom=tree_path, label="tree_start_bits", color='tab:cyan')

# 2bit bars (center)
ax1.bar(x, paths_dir, bar_width, label="2bit_direction_bits", color='tab:green')
ax1.bar(x, paths_start, bar_width, bottom=paths_dir, label="2bit_start_bits", color='lightgreen')

# 4th bar stacking rle_direction_bits and paths2bit_direction_bits
ax1.bar(x + bar_width, rle_dir, bar_width, label="rle_direction_bits", color='purple')
ax1.bar(x + bar_width, paths_start, bar_width, bottom=rle_dir, label="2bit_start_bits", color='violet')

# Add color-coded method indicator below x-axis
if not show_average:
    for i, method_idx in enumerate(best_method):
        color = method_colors[method_idx]
        rect = patches.Rectangle(
            (x[i] - bar_width * 1.5, -0.2),  # x pos and slight below-axis y pos
            bar_width * 3,                  # span of the group
            0.1,                            # height of bar
            transform=ax1.get_xaxis_transform(),
            clip_on=False,
            color=color
        )
        ax1.add_patch(rect)

# Axis setup
ax1.set_ylabel("Bit Count")
ax1.set_yscale('log')
ax1.legend(loc="upper left")

# Title and layout
title = "Average Bit Distribution" if show_average else "Bit Distribution per Image"
plt.title(title)
plt.tight_layout()
plt.grid(True, axis='y', linestyle='--', alpha=0.5)

# X-axis ticks
if not show_average:
    plt.xticks([])
else:
    plt.xticks([0], ["Average"])

# Make space for bottom indicators
plt.subplots_adjust(bottom=0.15)
plt.show()
