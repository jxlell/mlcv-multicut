import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load your CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_screenshots.csv")

# Sort by filename or another metric
df_sorted = df.sort_values(by="tree_rate")

# Set up x-axis positions
x = np.arange(len(df_sorted))

# Determine the best method for each image based on the highest compression rate
best_methods = []
for index, row in df_sorted.iterrows():
    rates = {
        "red_edgebits_rate": row["red_edgebits_rate"],
        "tree_rate": row["tree_rate"],
        # "2bit_rate": row["2bit_rate"],
        "rle_rate": row["rle_rate"],
        "straights_huffman_rate": row["straights_huffman_rate"]
    }
    best_method = max(rates, key=rates.get)
    best_methods.append(best_method)

# Color map for the methods
color_map = {
    "red_edgebits_rate": "tab:red",
    "tree_rate": "tab:purple",
    "rle_rate": "tab:orange",
    "straights_huffman_rate": "tab:brown"
    # "rle_rate": "tab:pink"
}

# Create the plot
fig, ax = plt.subplots(figsize=(14, 6))

label_map = {
    "tree_rate": "Directional Tree (DT)",
    # "2bit_rate": "2bit Rate",
    "rle_rate": "Directional Edge Chains (DEC)",
    "straights_huffman_rate": "Straight Line Segments (SLS)"
}

# Plot each compression rate as scatter points without connecting lines
for i, method in enumerate(["tree_rate", "rle_rate", "straights_huffman_rate"]):
    color = color_map[method]
    label = label_map[method]
    ax.scatter(x, df_sorted[method], label=label, color=color, marker='o', edgecolor='black', zorder=3, alpha=1, s=30)

# Color-code the points based on the best method
# for i, best_method in enumerate(best_methods):
#     color = color_map[best_method]
#     ax.scatter(x[i], df_sorted.loc[i, best_method], color=color, s=100, edgecolor='black', zorder=5)  # Larger marker for best method

# Add color-coded vertical lines at the bottom to represent best method
for i, best_method in enumerate(best_methods):
    color = color_map[best_method]
    ax.axvline(x=i, ymin=0, ymax=0.02, color=color, linewidth=6, zorder=1)


# Axis setup
ax.set_ylabel("Compression Rate")
ax.set_yscale('log')
ax.set_title("Comparison of Compression Rates with Best Method Highlighted")
ax.legend(loc="upper left")

# Optional tweaks
plt.xticks([])  # Hide x-axis labels if not needed
plt.grid(True, axis='y', linestyle='--', alpha=0.5)
plt.tight_layout()
plt.show()
