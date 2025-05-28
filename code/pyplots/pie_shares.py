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

# Reorder categories if desired
grouped = grouped.reindex(["screenshots", "icons", "photos", "textures"])

# Define pie chart parameters
colors = {
    "tree_path_bits": "#FF8080",
    "tree_start_bits": "#FF3333",
    "deflate_bits": "#7a9dff"
}
labels = ["DT Path Bits", "DT Start Bits", "Color Bits"]

# Plot one pie chart per category
fig, axes = plt.subplots(2, 2, figsize=(10, 8))
axes = axes.flatten()

for ax, (category, row) in zip(axes, grouped.iterrows()):
    bits = [row["tree_path_bits"], row["tree_start_bits"], row["deflate_bits"]]
    ax.pie(bits, labels=labels, colors=[colors["tree_path_bits"], colors["tree_start_bits"], colors["deflate_bits"]],
           autopct="%1.1f%%", startangle=90)
    ax.set_title(category.capitalize())

plt.suptitle("Compression Bit Composition per Category", fontsize=14)
plt.tight_layout()
plt.show()
