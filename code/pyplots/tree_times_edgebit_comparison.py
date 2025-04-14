import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the two CSV files into DataFrames
df1 = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_noedgebitmap.csv")
df2 = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_edgebitmap.csv")

# Extract the relevant column for tree construction time
tree_construction_time_1 = df1['tree_construction_time']
tree_construction_time_2 = df2['tree_construction_time']
edge_bitmap_time_2 = df2['setEdgeBitsTime']

# Check that both DataFrames have the same length (i.e., same number of images)
assert len(tree_construction_time_1) == len(tree_construction_time_2), "CSV files do not have the same number of rows!"

# Prepare the x positions for the bars
x = np.arange(len(tree_construction_time_1))  # Image indices

# Bar width
bar_width = 0.35

# Create plot
fig, ax = plt.subplots(figsize=(12, 6))

# Plot bars for both files
ax.bar(x - bar_width/2, tree_construction_time_1, bar_width, label='Pixel Operations', color='blue')
ax.bar(x + bar_width/2, tree_construction_time_2, bar_width, label='Edge Bitmap Operations', color='red')
ax.bar(x + bar_width/2, edge_bitmap_time_2, bar_width, bottom=tree_construction_time_2, label='Edge Bitmap Time', color='red', alpha=0.5)

# Add labels and title
ax.set_xlabel("Image Index")
ax.set_ylabel("Tree Construction Time (seconds)")
ax.set_title("Comparison of Tree Construction Time from Two Files")
ax.set_xticks([])

# Add color-coded squares below x-axis to indicate which bar is the lowest
for i in range(len(tree_construction_time_1)):
    if tree_construction_time_1[i] < tree_construction_time_2[i] + edge_bitmap_time_2[i]:
        color = 'blue'
    else:
        color = 'red'
    ax.add_patch(plt.Rectangle((i - bar_width / 2, -2.02), bar_width, 2.02, color=color, transform=ax.transData, clip_on=False))

# Add a legend
ax.legend()

# Add gridlines and tight layout
ax.grid(True, axis='y', linestyle="--", alpha=0.5)
plt.tight_layout()

# Show the plot
plt.show()
