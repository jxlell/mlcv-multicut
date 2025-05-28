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
df = pd.concat(data_frames, ignore_index=True)

# Add a column to indicate the category (file source)
categories = [
    "screenshots",
    "textures",
    "photos",
    "icons"
]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)

# Calculate total bits per image
df["total_bits"] = df["total_tree_bits"] + df["deflate_bits"]

# Group by category and sum bits
grouped = df.groupby("category")[["total_tree_bits", "deflate_bits"]].sum()

# Calculate percentage shares per category
grouped["tree_bits_percent"] = grouped["total_tree_bits"] / (grouped["total_tree_bits"] + grouped["deflate_bits"]) * 100
grouped["deflate_bits_percent"] = grouped["deflate_bits"] / (grouped["total_tree_bits"] + grouped["deflate_bits"]) * 100

# Prepare data for stacked bar plot (one bar per category)
bar_width = 0.6
indices = range(len(grouped))

# Specify custom colors
tree_bits_color = "#FF8080"      # blue
deflate_bits_color = "#7a9dff"   # orange

plt.figure(figsize=(8, 6))
plt.bar(
    indices,
    grouped["tree_bits_percent"],
    bar_width,
    label="DT Bits (%)",
    color=tree_bits_color
)
plt.bar(
    indices,
    grouped["deflate_bits_percent"],
    bar_width,
    bottom=grouped["tree_bits_percent"],
    label="Color Bits (%)",
    color=deflate_bits_color
)

plt.xticks(indices, grouped.index)
plt.xlabel("Category")
plt.ylabel("Percent (%)")
plt.title("Stacked Bar: Percent Contribution of Tree Bits and Deflate Bits per Category")
plt.legend()
plt.tight_layout()
plt.show()