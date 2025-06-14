import pandas as pd
import numpy as np

import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 12,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Load the CSV files and combine them into one DataFrame
csv_files = [
    # 'code/output_files/mc_results_test_screenshots_new.csv',
    # 'code/output_files/mc_results_test_textures_new.csv'
    # ,'code/output_files/mc_results_test_photos_new.csv'
    # 'code/output_files/mc_results_test_icons_new.csv'
    'code/output_files/mc_results_test_sample.csv',
]

csv_file1 = 'code/output_files/mc_results_test_screenshot_reduced_region_dfs.csv'
csv_file2 = 'code/output_files/mc_results_test_screenshot_reduced2_pixel_comparisons.csv'
df1 = pd.read_csv(csv_file1)
df2 = pd.read_csv(csv_file2)


# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Create two boxplots comparing two columns, e.g., 'column1' and 'column2'
columns_to_compare = ["region_color_UF_time", "region_color_dfs_time"]

box_data = [
    df2['region_color_UF_time'].dropna(),
    df2['region_color_dfs_time'].dropna()
]

# Compute means
means = [arr.mean() for arr in box_data]

# Compute standard deviation (or use .sem() for standard error)
errors = [arr.sem() for arr in box_data]

# Labels for bars
labels = ['UF', 'DFS']

# 007EE3 007DE376


# Plot
plt.figure(figsize=(6, 4))
plt.bar(labels, means, yerr=errors, capsize=8, color=['#007EE3', '#007DE376'], width=0.4)

plt.ylabel('Time (ms)')
plt.title('Comparing Running Times for Color Region \nDiscovery Approaches (Sample Image Data)')
plt.grid(axis='y', linestyle='--', linewidth=0.5, alpha=0.7)
plt.tight_layout()
plt.show()


# plt.figure(figsize=(8, 6))
# plt.boxplot(box_data)
# plt.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
# plt.title('Comparing Running Times for Color Region \nDiscovery Approaches (Artificial Images)')
# plt.xticks([1, 2], ['Union-Find', 'Depth-First Search'])
# plt.ylabel('Time (ms)')
# plt.yscale('log')  
# plt.subplots_adjust(left=0.14, right=0.95, top=0.87, bottom=0.07)
# plt.show()