import pandas as pd

# Load the CSV
data = pd.read_csv('/Users/jalell/Documents/GitHub/lossless-benchmark/compression_results.csv')

# Define mapping from specific categories to broader groups
category_mapping = {
    'textures_photo': 'textures',
    'textures_pk': 'textures',
    'textures_pk01': 'textures',
    'textures_pk02': 'textures',
    'textures_plants': 'textures',
    'photo_kodak': 'photos',
    'photo_tecnick': 'photos',
    'photo_wikipedia': 'photos',
    'screenshot_game': 'screenshots',
    'screenshot_game_reduced2': 'screenshots',
    'screenshot_web': 'screenshots',
    'icon_64': 'icons',
    'icon_512': 'icons',
    'pngimg': None  # Exclude this category
}

# Apply mapping
data['group'] = data['category'].map(category_mapping)

# Filter out rows not assigned to a group (i.e., pngimg)
data_grouped = data.dropna(subset=['group'])

# Calculate means
mean_cr = data_grouped.groupby('group')[['png_cr', 'jxl_cr']].mean()
median_cr = data_grouped.groupby('group')[['png_cr', 'jxl_cr']].median()

# Print results
print("Mean Compression Rates:")
print(mean_cr)
print("\nMedian Compression Rates:")
print(median_cr)
