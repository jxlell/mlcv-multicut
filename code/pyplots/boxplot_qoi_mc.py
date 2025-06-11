
import pandas as pd
import matplotlib.pyplot as plt
plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 15,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')

# Load the CSV
data = pd.read_csv('/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/parsed_qoi_results_new.csv')
data2 = pd.read_csv('/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_new.csv')
data3 = pd.read_csv('/Users/jalell/Documents/GitHub/lossless-benchmark/compression_results_total.csv')

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

# Map categories to broader groups
data['broad_category'] = data['category'].map(category_mapping)
data2['broad_category'] = data2['category'].map(category_mapping)
data3['broad_category'] = data3['category'].map(category_mapping)

# Filter for 'screenshots' and 'icons' categories
screenshots_data = data[data['broad_category'] == 'screenshots']
screenshots_data2 = data2[data2['broad_category'] == 'screenshots']
screenshots_data3 = data3[data3['broad_category'] == 'screenshots']

icons_data = data[data['broad_category'] == 'icons']
icons_data2 = data2[data2['broad_category'] == 'icons']
icons_data3 = data3[data3['broad_category'] == 'icons']

photo_data = data[data['broad_category'] == 'photos']
photo_data2 = data2[data2['broad_category'] == 'photos']
photo_data3 = data3[data3['broad_category'] == 'photos']

textures_data = data[data['broad_category'] == 'textures']
textures_data2 = data2[data2['broad_category'] == 'textures']
textures_data3 = data3[data3['broad_category'] == 'textures']

screenshots_data = pd.concat([screenshots_data, icons_data, photo_data, textures_data], ignore_index=True)
screenshots_data2 = pd.concat([screenshots_data2, icons_data2, photo_data2, textures_data2], ignore_index=True)
screenshots_data3 = pd.concat([screenshots_data3, icons_data3, photo_data3, textures_data3], ignore_index=True)

# Prepare data for boxplot
boxplot_data = [
    screenshots_data3['png_cr'].dropna(),
    screenshots_data2['tree_rate'].dropna(),
    screenshots_data3['jxl_cr'].dropna()
]

plt.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)

plt.boxplot(boxplot_data, labels=['PNG', 'MC (DT)', 'JPEG XL'])
plt.ylabel('Compression Rate')
plt.title('Compression Rates - Comparing Standards \n and Multicut-based Approach (DT) (All Images)')
plt.yscale('log')
plt.show()

