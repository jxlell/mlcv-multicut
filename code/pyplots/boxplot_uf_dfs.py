import pandas as pd

import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 19,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots_new.csv',
    # 'code/output_files/mc_results_test_textures_new.csv'
    # ,'code/output_files/mc_results_test_photos_new.csv'
    'code/output_files/mc_results_test_icons_new.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Create two boxplots comparing two columns, e.g., 'column1' and 'column2'
columns_to_compare = ["region_color_UF_time", "region_color_dfs_time"]

box_data = [
    data['region_color_UF_time'].dropna(),
    data['region_color_dfs_time'].dropna()
]

plt.figure(figsize=(8, 6))
plt.boxplot(box_data)
plt.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
plt.title('Comparing Running Times for Color Region \nDiscovery Approaches (Artificial Images)')
plt.xticks([1, 2], ['Union-Find', 'Depth-First Search'])
plt.ylabel('Time (ms)')
plt.yscale('log')  
plt.subplots_adjust(left=0.14, right=0.95, top=0.87, bottom=0.07)
plt.show()