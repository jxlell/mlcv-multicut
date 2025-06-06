import pandas as pd
import matplotlib.pyplot as plt

# Load the CSVs
qoi_results = pd.read_csv('/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/parsed_qoi_results_new.csv')
mc_screenshots = pd.read_csv('/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_new.csv')
libpng_jxl = pd.read_csv('/Users/jalell/Documents/GitHub/lossless-benchmark/compression_results_total.csv')

# Define mapping
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
    # 'screenshot_game_reduced2': 'screenshots', 
    'screenshot_web': 'screenshots',
    'icon_64': 'icons', 
    'icon_512': 'icons', 
    'pngimg': None
}

# Map and filter QOI results
qoi_results['broad_category'] = qoi_results['category'].map(category_mapping)
qoi_results = qoi_results.dropna(subset=['rate'])
# Remove .png extension from filenames for consistent merging
qoi_results['filename'] = qoi_results['filename'].str.replace(r'\.png$', '', regex=True)

qoi_results_filtered = qoi_results[qoi_results['broad_category'].isin(['screenshots', 'icons'])]
# print(qoi_results_filtered.shape)
sorted_qoi = qoi_results_filtered.sort_values(by='rate', ascending=True).reset_index(drop=True)
# print(sorted_qoi)
count = qoi_results[qoi_results['broad_category'].isin(['screenshots', 'icons'])].shape[0]
# print("Number of lines with screenshots or icons:", count)

# Map and filter MC screenshots
mc_screenshots['broad_category'] = mc_screenshots['category'].map(category_mapping)
mc_screenshots = mc_screenshots.dropna(subset=['tree_rate'])
mc_screenshots['filename'] = mc_screenshots['filename'].str.replace(r'\.png$', '', regex=True)
mc_screenshots_filtered = mc_screenshots[mc_screenshots['broad_category'].isin(['screenshots', 'icons'])]
# print(mc_screenshots_filtered.shape)

# Map and filter libpng_jxl
libpng_jxl['broad_category'] = libpng_jxl['category'].map(category_mapping)
libpng_jxl = libpng_jxl.dropna(subset=['jxl_cr'])
count_libpng_jxl = libpng_jxl[libpng_jxl['broad_category'].isin(['screenshots', 'icons'])].shape[0]
# print("Number of lines with screenshots or icons in libpng_jxl:", count_libpng_jxl)
libpng_jxl_filtered = libpng_jxl[libpng_jxl['broad_category'].isin(['screenshots', 'icons'])]
# print(libpng_jxl_filtered)
# Ensure unique filenames before merging
libpng_jxl_filtered = libpng_jxl_filtered.drop_duplicates(subset='filename')
# Print duplicate filenames in each dataset
# print("Duplicate filenames in qoi_results_filtered:")
# print(qoi_results_filtered[qoi_results_filtered.duplicated('filename', keep=False)]['filename'].unique())

# print("Duplicate filenames in mc_screenshots_filtered:")
# print(mc_screenshots_filtered[mc_screenshots_filtered.duplicated('filename', keep=False)]['filename'].unique())

# print("Duplicate filenames in libpng_jxl_filtered:")
# print(libpng_jxl_filtered[libpng_jxl_filtered.duplicated('filename', keep=False)]['filename'].unique())

# Align all datasets to sorted QOI filenames
mc_sorted = sorted_qoi[['filename', 'category']].merge(mc_screenshots_filtered, on=['filename', 'category'], how='left')
jxl_sorted = sorted_qoi[['filename', 'category']].merge(libpng_jxl_filtered, on=['filename', 'category'], how='left')

# print("Size of jxl_sorted:", jxl_sorted.shape)
# print("Size of mc_sorted:", mc_sorted.shape)
# print("Size of sorted_qoi:", sorted_qoi.shape)
# # print(jxl_sorted)

# Plot
plt.figure(figsize=(10, 6))
plt.scatter(sorted_qoi.index, sorted_qoi["rate"], label="PNG", s=10, alpha=0.7, marker='x')
plt.scatter(mc_sorted.index, mc_sorted["tree_rate"], label="Multicut (DT)", s=10, alpha=0.7, marker='x')
plt.scatter(jxl_sorted.index, jxl_sorted["jxl_cr"], label="JPEG XL", s=10, alpha=0.7, marker='x')
plt.xlabel("Image (sorted by PNG rate)")
plt.ylabel("Compression Rate")
plt.title("Compression Rate Comparison")
plt.legend()
plt.tight_layout()
plt.yscale('log')
plt.show()
