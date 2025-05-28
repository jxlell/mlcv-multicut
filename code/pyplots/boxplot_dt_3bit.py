import pandas as pd

import matplotlib.pyplot as plt

# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots_new.csv'
    ,'code/output_files/mc_results_test_textures_new.csv'
    ,'code/output_files/mc_results_test_photos_new.csv'
    ,'code/output_files/mc_results_test_icons_new.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Create two boxplots comparing two columns, e.g., 'column1' and 'column2'
columns_to_compare = ["3bit_paths_bits", "tree_path_bits"]


plt.figure(figsize=(8, 6))
data[columns_to_compare].boxplot()
plt.title('Comparing Bits for the DT design choices\nfixed 3-bit code words vs. code words omitting previously encoded edges\n(All Images)')
plt.xticks([1, 2], ['fixed 3-bit code words', 'reduced code words'])
plt.ylabel('Bit Count')
plt.yscale('log')  
plt.show()