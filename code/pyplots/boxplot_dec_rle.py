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
columns_to_compare = ["paths2bit_direction_bits", "rle_direction_bits"]

data['dec_bpe'] = data['paths2bit_direction_bits'] / data['edgebits']
data['rle_bpe'] = data['rle_direction_bits'] / data['edgebits']

box_data = [
    data['dec_bpe'].dropna(),
    data['rle_bpe'].dropna()
]

plt.figure(figsize=(8, 6))
plt.boxplot(box_data)
plt.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
plt.title('Comparing directional bits per edge (bpe) \nfor the DEC method - non-RLE vs RLE (All Images)')
plt.xticks([1, 2], ['Non-RLE', 'RLE'])
plt.ylabel('Bits per edge (bpe)')
plt.yscale('log')  
plt.show()