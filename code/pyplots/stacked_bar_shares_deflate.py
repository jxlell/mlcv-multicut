import pandas as pd
import matplotlib.pyplot as plt

# Load and combine CSVs
csv_files = [
    # 'code/output_files/mc_results_test_screenshots_final.csv',
    # 'code/output_files/mc_results_test_textures_final.csv',
    # 'code/output_files/mc_results_test_photos_final.csv',
    'code/output_files/mc_results_test_icons_final.csv'
]

data_frames = [pd.read_csv(file) for file in csv_files]
categories = ["screenshots", "textures", "photos", "icons"]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)

# Group by category and sum bits
grouped = df.groupby("category")[["tree_path_bits", "tree_start_bits", "deflate_bits"]].sum()

# Compute total bits
grouped["total_bits"] = grouped["tree_path_bits"] + grouped["tree_start_bits"] + grouped["deflate_bits"]
grouped["total_bits"] = grouped["treeMCBits"] + grouped["deflate_bits"]

# Calculate percentage shares
grouped["path_bits_percent"] = grouped["tree_path_bits"] / grouped["total_bits"] * 100
# grouped["start_bits_percent"] = grouped["tree_start_bits"] / grouped["total_bits"] * 100
grouped["deflate_bits_percent"] = grouped["deflate_bits"] / grouped["total_bits"] * 100

# Reorder categories
grouped = grouped.reindex(["screenshots", "icons", "photos", "textures"])

# Plot
bar_width = 0.6
indices = range(len(grouped))

fig, ax = plt.subplots(figsize=(8, 6))
ax.bar(
    indices,
    grouped["path_bits_percent"],
    bar_width,
    label="DT Directional Bits (%)",
    color="#007EE3"
)
ax.bar(
    indices,
    grouped["start_bits_percent"],
    bar_width,
    bottom=grouped["path_bits_percent"],
    label="DT Start Edge Bits (%)",
    color="#00335D"
)
ax.bar(
    indices,
    grouped["deflate_bits_percent"],
    bar_width,
    bottom=grouped["path_bits_percent"] + grouped["start_bits_percent"],
    label="Color Bits (%)",
    color="#007DE376"
)

current_ylim = ax.get_ylim()
ax.set_ylim(current_ylim[0], current_ylim[1] * 1.05)

ax.set_xticks(indices)
ax.set_xticklabels(grouped.index)
ax.set_xlabel("Category")
ax.set_ylabel("Percent (%)")
ax.set_title("Stacked Bar: Percent Contribution of Tree Bits and Deflate Bits per Category")
ax.legend(loc='upper center', ncol=3)
plt.tight_layout()
plt.show()
