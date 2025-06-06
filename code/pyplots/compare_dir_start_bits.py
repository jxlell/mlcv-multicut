import pandas as pd

# Replace 'your_file.csv' with your CSV file path
csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'

csv_files = [
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'
]

screenshots_csv = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv'
textures_csv = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'
photos_csv = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv'
icons_csv = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'

df_screenshots = pd.read_csv(screenshots_csv)
df_textures = pd.read_csv(textures_csv)
df_photos = pd.read_csv(photos_csv)
df_icons = pd.read_csv(icons_csv)

# Build stacks for rle, tree, and sls, then print their means
rle_stack = df_rle = df_screenshots['rle_direction_bits'] + df_screenshots['paths2bit_start_bits']
tree_stack = df_tree = df_screenshots['tree_path_bits'] + df_screenshots['tree_start_bits']
sls_stack = df_sls = df_screenshots['sls_mc_bits']

# print("Means for Screenshots:")
# print(f"RLE stack mean: {rle_stack.mean()}")
# print(f"Tree stack mean: {tree_stack.mean()}")
# print(f"SLS mean: {sls_stack.mean()}")

# rle_stack = df_textures['rle_direction_bits'] + df_textures['paths2bit_start_bits']
# tree_stack = df_textures['tree_path_bits'] + df_textures['tree_start_bits']
# sls_stack = df_textures['sls_mc_bits']

# print("\nMeans for Textures:")
# print(f"RLE stack mean: {rle_stack.mean()}")
# print(f"Tree stack mean: {tree_stack.mean()}")
# print(f"SLS mean: {sls_stack.mean()}")

# rle_stack = df_photos['rle_direction_bits'] + df_photos['paths2bit_start_bits']
# tree_stack = df_photos['tree_path_bits'] + df_photos['tree_start_bits']
# sls_stack = df_photos['sls_mc_bits']

# print("\nMeans for Photos:")
# print(f"RLE stack mean: {rle_stack.mean()}")
# print(f"Tree stack mean: {tree_stack.mean()}")
# print(f"SLS mean: {sls_stack.mean()}")

# rle_stack = df_icons['rle_direction_bits'] + df_icons['paths2bit_start_bits']
# tree_stack = df_icons['tree_path_bits'] + df_icons['tree_start_bits']
# sls_stack = df_icons['sls_mc_bits']

# print("\nMeans for Icons:")
# print(f"RLE stack mean: {rle_stack.mean()}")
# print(f"Tree stack mean: {tree_stack.mean()}")
# print(f"SLS mean: {sls_stack.mean()}")

# List the four columns you want to calculate the mean for
columns = ['total_tree_bits', 'rle_bits']

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

# Read the CSV
# df = pd.read_csv(csv_file)
df = pd.concat([pd.read_csv(file) for file in csv_files], ignore_index=True)
df['rle_bits'] = df['rle_direction_bits'] + df['paths2bit_start_bits']

df['tree_stack'] = df['tree_path_bits'] + df['tree_start_bits']
df['rle_stack'] = df['rle_direction_bits'] + df['paths2bit_start_bits']

means = df[['tree_stack', 'rle_stack', 'sls_mc_bits']].mean()
# print(means)

# Calculate mean bits per pixel for each method
df['tree_stack_bpp'] = df['tree_stack'] / df['pixels']
df['rle_stack_bpp'] = df['rle_stack'] / df['pixels']
df['sls_mc_bits_bpp'] = df['sls_mc_bits'] / df['pixels']

# Map categories to broad categories
df['category'] = df['category'].map(category_mapping)
df = df[df['category'].notnull()]

mean_bpp = df[['tree_stack_bpp', 'rle_stack_bpp', 'sls_mc_bits_bpp']].mean()
print("\nMean bits per pixel:")
print(mean_bpp)

# Print mean bits per pixel by broad category
category_bpp_means = df.groupby('category')[['tree_stack_bpp', 'rle_stack_bpp', 'sls_mc_bits_bpp']].mean()
print("\nMean bits per pixel by broad category:")
print(category_bpp_means)

# # Map categories
# df['category'] = df['category'].map(category_mapping)
# df = df[df['category'].notnull()]

# # Calculate means by category
# category_means = df.groupby('category')[['tree_stack', 'rle_stack', 'sls_mc_bits']].mean()
# print("\nMean bits by category:")
# print(category_means)

# Decide which method has the lowest mean bits
lowest_method = means.idxmin()
lowest_value = means.min()
print(f"Lowest mean bits: {lowest_method} ({lowest_value})")
