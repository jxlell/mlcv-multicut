import pandas as pd

# Replace with your CSV file path
csv_path = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test.csv'
csv_files = [
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'
]

csv_screenshots = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv'
csv_textures = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'
csv_photos = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv'
csv_icons = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'

df_screenshots = pd.read_csv(csv_screenshots)
df_textures = pd.read_csv(csv_textures)
df_photos = pd.read_csv(csv_photos)
df_icons = pd.read_csv(csv_icons)

print("Mean tree_rate:", df_screenshots['tree_rate'].mean())
print("Mean tree_rate:", df_icons['tree_rate'].mean())
print("Mean tree_rate:", df_textures['tree_rate'].mean())
print("Mean tree_rate:", df_photos['tree_rate'].mean())

print("Mean edgebits_deflate_rate screen:", df_screenshots['edgebits_deflate_rate'].mean())
print("Mean edgebits_deflate_rate icons:", df_icons['edgebits_deflate_rate'].mean())
print("Mean edgebits_deflate_rate photos:", df_textures['edgebits_deflate_rate'].mean())
print("Mean edgebits_deflate_rate: textures", df_photos['edgebits_deflate_rate'].mean())

print("Median edgebits_deflate_rate (screenshots):", df_screenshots['edgebits_deflate_rate'].median())
print("Median edgebits_deflate_rate (icons):", df_icons['edgebits_deflate_rate'].median())
print("Median edgebits_deflate_rate (textures):", df_textures['edgebits_deflate_rate'].median())
print("Median edgebits_deflate_rate (photos):", df_photos['edgebits_deflate_rate'].median())

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


# Replace with your column names
col1 = 'tree_rate'
col2 = 'edgebits_deflate_rate'

df = pd.concat([pd.read_csv(file) for file in csv_files], ignore_index=True)
count_col1_higher = (df[col1] > df[col2]).sum()
count_col2_higher = (df[col2] > df[col1]).sum()
count_equal = (df[col1] == df[col2]).sum()

# Add broad category column
df['broad_category'] = df['category'].map(category_mapping)
df = df[df['broad_category'].notnull()]

for cat in sorted(df['broad_category'].unique()):
    subdf = df[df['broad_category'] == cat]
    c1_higher = (subdf[col1] > subdf[col2]).sum()
    c2_higher = (subdf[col2] > subdf[col1]).sum()
    equal = (subdf[col1] == subdf[col2]).sum()
    print(f"\nCategory: {cat}")
    print(f"{col1} > {col2}: {c1_higher} times")
    print(f"{col2} > {col1}: {c2_higher} times")
    print(f"{col1} == {col2}: {equal} times")
    print(f"Mean {col1}: {subdf[col1].mean()}")
    print(f"Mean {col2}: {subdf[col2].mean()}")
    print(f"Median {col1}: {subdf[col1].median()}")
    print(f"Median {col2}: {subdf[col2].median()}")

# print(f"{col1} > {col2}: {count_col1_higher} times")
# print(f"{col2} > {col1}: {count_col2_higher} times")
# print(f"{col1} == {col2}: {count_equal} times")

# print(f"Mean {col1}: {df[col1].mean()}")
# print(f"Mean {col2}: {df[col2].mean()}")

# print(f"Median {col1}: {df[col1].median()}")
# print(f"Median {col2}: {df[col2].median()}")