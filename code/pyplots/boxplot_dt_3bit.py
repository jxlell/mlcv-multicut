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


# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv'
    ,'code/output_files/mc_results_test_textures_final.csv'
    ,'code/output_files/mc_results_test_photos_final.csv'
    ,'code/output_files/mc_results_test_icons_final.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Create two boxplots comparing two columns, e.g., 'column1' and 'column2'
columns_to_compare = ["3bit_paths_bits", "tree_path_bits"]

data['3bit_bpp'] = data['3bit_paths_bits'] / data['edgebits']
data['tree_bpp'] = data['tree_path_bits'] / data['edgebits']

box_data = [
    data['3bit_bpp'].dropna(),
    data['tree_bpp'].dropna()
]

plt.figure(figsize=(8, 6))
# data[columns_to_compare].boxplot()
plt.boxplot(box_data)
plt.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
plt.title('Comparing Bits per edge (bpe) for the DT design choices\nfixed 3-bit code words vs. code words omitting previously encoded edges\n(All Images)')
plt.xticks([1, 2], ['fixed 3-bit code words', 'reduced code words'])
plt.ylabel('Bits per edge (bpe)')
plt.yscale('log')  
plt.subplots_adjust(left=0.1, right=0.95, top=0.85, bottom=0.07)
plt.show()