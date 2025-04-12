import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV data
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_reduced.csv")

# Calculate pixels per region
df["pixels_per_region"] = df["pixels"] / df["regions"]

# Sort by pixels per region
df_sorted = df.sort_values(by="pixels_per_region")

# Calculate total tree bits
tree_total = df_sorted["tree_path_bits"] + df_sorted["tree_start_bits"]
huffman_bits = df_sorted["dpcm-huffman_bits"]

# Use the sorted pixels_per_region values
pixels_per_region = df_sorted["pixels_per_region"]

# Prepare x-axis
x = np.arange(len(df_sorted))
bar_width = 0.6

# Create plot
fig, ax1 = plt.subplots(figsize=(14, 6))

# Primary y-axis: stacked bar chart
ax1.bar(x, tree_total, bar_width, label="tree_path + tree_start", color="tab:blue")
ax1.bar(x, huffman_bits, bar_width, bottom=tree_total, label="dpcm-huffman_bits", color="tab:orange")
ax1.set_ylabel("Bit Count", color='black')
ax1.tick_params(axis='y', labelcolor='black')

# Secondary y-axis: pixels per region
ax2 = ax1.twinx()
ax2.plot(x, pixels_per_region, label="Pixels per Region", color="tab:red", marker='o', linewidth=2)
ax2.set_ylabel("Pixels per Region", color='tab:red')
ax2.tick_params(axis='y', labelcolor='tab:red')

# Title and legend
plt.title("Stacked Bit Count with Pixels per Region")
fig.legend(loc="upper right", bbox_to_anchor=(1, 1), bbox_transform=ax1.transAxes)

# Clean x-axis
ax1.set_xticks([])
ax1.grid(True, axis="y", linestyle="--", alpha=0.5)

# Layout
plt.tight_layout()
plt.show()
