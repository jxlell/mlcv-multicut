import pandas as pd
import matplotlib.pyplot as plt

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots.csv',
    'code/output_files/mc_results_test_textures_no_pk.csv',
    'code/output_files/mc_results_test_textures_pk.csv',
    'code/output_files/mc_results_test_photos.csv',
    'code/output_files/mc_results_test_icons.csv'
]

data_frames = [pd.read_csv(file) for file in csv_files]
df = pd.concat(data_frames, ignore_index=True)

# Define x and y values
x = df["mc_percentage"]
ys = {
    "Tree Rate": df["tree_rate"],
    "RLE Rate": df["rle_rate"],
    "Red Edgebits Rate": df["red_edgebits_rate"],
    "Straights Huffman Rate": df["straights_huffman_rate"]
}

# Color map by category
custom_colors = {
    "textures_pk": "#ff99c8",       
    "textures": "#ff99c8",          
    "textures_plants": "#ff99c8",    
    "textures_pk01": "#ff99c8",     
    "textures_pk02": "#ff99c8",     
    "textures_photo": "#ff99c8",    
    "screenshot_game": "#FFC0CB", 
    "screenshot_web": "#FFC0CB",    
    "icon_64": "#a9def9",           
    "icon_512": "#a9def9",          
    "photo_kodak": "#fcf6bd",       
    "photo_tecnick": "#fcf6bd",    
    "photo_wikipedia": "#fcf6bd",  
}

categories = df["category"]

# Create 2×2 subplots
fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(12, 8), sharex=True)
axes = axes.flatten()  # Flatten to index with a single loop

# Plot
for ax, (title, y_data) in zip(axes, ys.items()):
    for category in categories.unique():
        mask = categories == category
        ax.scatter(
            x[mask], y_data[mask],
            color=custom_colors.get(category, "#000000"),
            alpha=0.8, marker='x', s=10,
            label=category if title == "Tree Rate" else None  # show legend once
        )
    ax.set_title(title)
    ax.set_yscale('log')
    ax.grid(True)

# Final formatting
axes[2].set_xlabel("Share of edges that belong to the multicut")
axes[3].set_xlabel("Share of edges that belong to the multicut")
axes[0].set_ylabel("Compression Rate")
axes[2].set_ylabel("Compression Rate")

axes[0].legend(title="Category", bbox_to_anchor=(1.05, 1), loc='upper left')
fig.suptitle("Compression Rate vs. Multicut Share (per Method)", fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()
