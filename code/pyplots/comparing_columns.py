import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Choose the column to compare
# column_name = "region_color_dfs_time"
column_name = "dpcm-huffman_bits"

# Load the two CSV files
df1 = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_dpcmv1.csv")
df2 = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_dpcmv2.csv")

# Get values
values1 = df1[column_name].values
values2 = df2[column_name].values

# X-axis
x = np.arange(len(values1))
bar_width = 0.35

# Plot
fig, ax = plt.subplots(figsize=(12, 6))
ax.bar(x - bar_width/2, values1, bar_width, label="dpcm delta all channels", color="skyblue")
ax.bar(x + bar_width/2, values2, bar_width, label="delta red channel", color="salmon")

# Labels and formatting
ax.set_title(f"Comparison of '{column_name}' between Two CSV Files")
ax.set_xlabel("Image Index")
ax.set_ylabel(column_name.replace("_", " ").title())
ax.legend()
ax.grid(True, axis='y', linestyle="--", alpha=0.5)
plt.tight_layout()
plt.xticks([])  # Remove x-ticks for readability

plt.show()
