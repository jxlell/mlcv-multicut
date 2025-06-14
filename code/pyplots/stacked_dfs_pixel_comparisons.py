import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

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


# Load CSV files
csv_file1 = 'code/output_files/mc_results_test_screenshot_reduced_region_dfs.csv'
csv_file2 = 'code/output_files/mc_results_test_screenshot_reduced2_pixel_comparisons.csv'
df1 = pd.read_csv(csv_file1)
df2 = pd.read_csv(csv_file2)

# Columns to stack
col1 = "region_color_dfs_time"
col2 = "tree_construction_time"

# Compute averages
avg1_col1 = df1[col1].mean()
avg1_col2 = df1[col2].mean()
avg2_col1 = df2[col1].mean()
avg2_col2 = df2[col2].mean()

# Compute standard error of the mean (SEM)
sem1_col1 = df1[col1].sem()
sem1_col2 = df1[col2].sem()
sem2_col1 = df2[col1].sem()
sem2_col2 = df2[col2].sem()

# Swapped data: Pixel-Based first, DFS-Based second
bar_labels = ['Pixel-Based', 'CMV-Based']
bar1 = [avg2_col1, avg2_col2]  # Pixel-Based
bar2 = [avg1_col1, avg1_col2]  # DFS-Based


x = np.arange(len(bar_labels))
width = 0.4

# Shared colors
bottom_color = '#007EE3'  # blue
top_color = "#007DE376"     # orange

# Plot
fig, ax = plt.subplots(figsize=(8, 6))

# Pixel-based (left bar) – x[0]
ax.bar(
    x[0],
    bar1[0],
    width,
    yerr=sem2_col1,  # sem for avg2_col1 (Pixel-Based)
    label=col1,
    color=bottom_color,
    capsize=6
)

ax.bar(
    x[0],
    bar1[1],
    width,
    yerr=sem2_col2,  # sem for avg2_col2 (Pixel-Based)
    bottom=bar1[0],
    label=col2,
    color=top_color,
    capsize=6
)

# DFS-based (right bar) – x[1]
ax.bar(
    x[1],
    bar2[0],
    width,
    yerr=sem1_col1,  # sem for avg1_col1 (DFS-Based)
    color=bottom_color,
    capsize=6
)

ax.bar(
    x[1],
    bar2[1],
    width,
    yerr=sem1_col2,  # sem for avg1_col2 (DFS-Based)
    bottom=bar2[0],
    color=top_color,
    capsize=6
)


# Labels
ax.set_ylabel('Average Time (ms)')
ax.set_title('Comparing average region discovery and DT construction times - \n pixel accesses vs. CMV accesses (Sample Image Dataset)')
ax.set_xticks(x)
ax.set_xticklabels(bar_labels)
ax.legend([plt.Rectangle((0, 0), 1, 1, color=bottom_color),
           plt.Rectangle((0, 0), 1, 1, color=top_color)],
          ["Region Discovery", "DT Construction"])
ax.grid(axis='y', linestyle='--', alpha=0.7)
plt.tight_layout()
plt.show()
