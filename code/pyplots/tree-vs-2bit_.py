import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as patches

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 15,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')

# Toggle between individual bars and averaged bar
show_average = False  # <<<< CHANGE THIS TO True for average-only view

# Load your CSV
df = pd.read_csv("/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_new.csv")

# Sort by filename or other metric
df_sorted = df.sort_values(by="filename")

# Filter to only 15 images
df_sorted = df_sorted.head(15)

# Remove the first 5 rows from the sorted DataFrame
df_sorted = df_sorted.iloc[5:]

# Prepare data
if show_average:
    # Compute mean values
    tree_path = [df_sorted["tree_path_bits"].mean()]
    tree_start = [df_sorted["tree_start_bits"].mean()]
    paths_dir = [df_sorted["paths2bit_direction_bits"].mean()]
    paths_start = [df_sorted["paths2bit_start_bits"].mean()]
    new2bit_dir = [df_sorted["new2bitDirectionBits"].mean()]
    x = np.array([0])  # Single bar for average
else:
    tree_path = df_sorted["tree_path_bits"].values
    tree_start = df_sorted["tree_start_bits"].values
    paths_dir = df_sorted["paths2bit_direction_bits"].values
    paths_start = df_sorted["paths2bit_start_bits"].values
    new2bit_dir = df_sorted["new2bitDirectionBits"].values
    x = np.arange(len(df_sorted))

bar_width = 0.25

# tree_path = tree_path / df_sorted["edgebits"].values
# tree_start = tree_start / df_sorted["edgebits"].values
# paths_dir = paths_dir / df_sorted["edgebits"].values
# paths_start = paths_start / df_sorted["edgebits"].values

# Determine the best (lowest total bits) method for each image
if not show_average:
    total_tree = tree_path + tree_start
    total_2bit = paths_dir + paths_start
    total_new2bit = new2bit_dir + paths_start

    best_method = np.argmin(np.stack([total_tree, total_2bit]), axis=0)
    method_colors = ['tab:blue', 'tab:green', 'tab:red']

# Create the plot
fig, ax1 = plt.subplots(figsize=(14, 6))

# Tree bars (leftmost)
ax1.bar(x - bar_width, tree_path, bar_width, label="DT directional bits", color='tab:blue')
ax1.bar(x - bar_width, tree_start, bar_width, bottom=tree_path, label="DT start edge bits", color='tab:cyan')

# 2bit bars (center)
ax1.bar(x, paths_dir, bar_width, label="DEC directional bits", color='tab:green')
ax1.bar(x, paths_start, bar_width, bottom=paths_dir, label="DEC start edge bits", color='lightgreen')

# new2bit bars (rightmost)
# ax1.bar(x + bar_width, new2bit_dir, bar_width, label="new2bit_direction_bits", color='tab:red')
# ax1.bar(x + bar_width, paths_start, bar_width, bottom=new2bit_dir, label="2bit_start_bits (again)", color='salmon')

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
        # ax1.add_patch(rect)

# Draw a dotted red rectangle around the last two bars
if not show_average:
    # Get positions of the last two bars
    left_edge = x[-2] - bar_width * 1.5 - 0.14  # start of leftmost bar group
    total_width = (x[-1] - x[-2]) + bar_width * 3  # include space for three bars (left, center, right)

    rect = patches.Rectangle(
        (left_edge, 0),         # (x, y) of bottom-left corner
        total_width,            # width
        ax1.get_ylim()[1]/8,      # height = current y max
        linewidth=2.5,
        edgecolor='red',
        linestyle='dotted',
        facecolor='none',
        transform=ax1.transData
    )
    ax1.add_patch(rect)


# Axis setup
ax1.set_ylabel("Bit Count")
ax1.set_yscale('log')
ax1.legend(loc="upper left")

# Title and layout
title = "Average Bit Distribution" if show_average else "Bit Distribution per Image (10 Screenshots) - Stacking DT and DEC start and direction bits"
plt.title(title)
plt.tight_layout()
plt.grid(True, axis='y', linestyle='--', alpha=0.5)

# plt.xticks(x, df_sorted["filename"], ha='right')
plt.xticks(x, x)
print(df_sorted["filename"].values)

# Make space for bottom indicators
# plt.subplots_adjust(bottom=0.15)
plt.show()
