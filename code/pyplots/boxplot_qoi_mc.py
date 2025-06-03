import pandas as pd

# Load the CSV
data = pd.read_csv('/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/parsed_qoi_results_new.csv')

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

# TODO: boxplot