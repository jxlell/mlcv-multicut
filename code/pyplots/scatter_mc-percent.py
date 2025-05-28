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

# Define x and y values
x = df["mc_percentage"]
ys = {
    "rCMV": df["red_edgebits_rate"],
    "DT": df["tree_rate"],
    "DEC": df["rle_rate"],
    "SLS": df["straights_huffman_rate"]
}

# Color map by subcategory
custom_colors = {
    "textures_pk": "#eb87b6",       
    "textures": "#eb87b6",          
    "textures_plants": "#eb87b6",    
    "textures_pk01": "#eb87b6",     
    "textures_pk02": "#eb87b6",     
    "textures_photo": "#eb87b6",    
    "screenshot_game": "#F6BC6B", 
    "screenshot_web": "#F6BC6B",    
    "icon_64": "#4fbcf3",           
    "icon_512": "#4fbcf3",          
    "photo_kodak": "#b7fa64",       
    "photo_tecnick": "#b7fa64",    
    "photo_wikipedia": "#b7fa64",  
}

# Group map
category_groups = {
    "Textures": ["textures_pk", "textures", "textures_plants", "textures_pk01", "textures_pk02", "textures_photo"],
    "Screenshots": ["screenshot_game", "screenshot_web"],
    "Icons": ["icon_64", "icon_512"],
    "Photos": ["photo_kodak", "photo_tecnick", "photo_wikipedia"]
}

def get_group(category):
    for group, subcats in category_groups.items():
        if category in subcats:
            return group
    return None

categories = df["category"]
labeled_groups = set()

# Create 2×2 subplots
fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(12, 8), sharex=True)
axes = axes.flatten()

# Plot
for ax, (title, y_data) in zip(axes, ys.items()):
    for category in categories.unique():
        mask = categories == category
        group = get_group(category)
        label = None
        if ax == axes[0] and group and group not in labeled_groups:
            label = group
            labeled_groups.add(group)
        ax.scatter(
            x[mask], y_data[mask],
            color=custom_colors.get(category, "#000000"),
            alpha=0.8, marker='x', s=10,
            label=label
        )
    ax.set_title(title)
    ax.set_yscale('log')
    ax.grid(True)

# Final formatting
axes[2].set_xlabel("Share of edges that belong to the multicut in %")
axes[3].set_xlabel("Share of edges that belong to the multicut in %")
axes[0].set_ylabel("Compression Rate")
axes[2].set_ylabel("Compression Rate")

axes[0].legend(title="Category", bbox_to_anchor=(1.05, 1), loc='upper left')
fig.suptitle("Compression Rate vs. Multicut Share (per Method)", fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()
