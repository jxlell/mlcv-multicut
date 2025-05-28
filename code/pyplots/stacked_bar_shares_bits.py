import pandas as pd
import matplotlib.pyplot as plt

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_new.csv',
    'code/output_files/mc_results_test_textures_new.csv',
    'code/output_files/mc_results_test_photos_new.csv',
    'code/output_files/mc_results_test_icons_new.csv'
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

# Calculate percentage shares
grouped["path_bits_percent"] = grouped["tree_path_bits"] / grouped["total_bits"] * 100
grouped["start_bits_percent"] = grouped["tree_start_bits"] / grouped["total_bits"] * 100
grouped["deflate_bits_percent"] = grouped["deflate_bits"] / grouped["total_bits"] * 100

# Reorder categories
grouped = grouped.reindex(["screenshots", "icons", "photos", "textures"])

# Plot
bar_width = 0.6
indices = range(len(grouped))

plt.figure(figsize=(8, 6))
plt.bar(
    indices,
    grouped["path_bits_percent"],
    bar_width,
    label="DT Path Bits (%)",
    color="#FF8080"
)
plt.bar(
    indices,
    grouped["start_bits_percent"],
    bar_width,
    bottom=grouped["path_bits_percent"],
    label="DT Start Bits (%)",
    color="#FF3333"
)
plt.bar(
    indices,
    grouped["deflate_bits_percent"],
    bar_width,
    bottom=grouped["path_bits_percent"] + grouped["start_bits_percent"],
    label="Color Bits (%)",
    color="#7a9dff"
)

plt.xticks(indices, grouped.index)
plt.xlabel("Category")
plt.ylabel("Percent (%)")
plt.title("Stacked Bar: Percent Contribution of Tree Bits and Deflate Bits per Category")
plt.legend()
plt.tight_layout()
plt.show()
