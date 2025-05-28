import pandas as pd

import matplotlib.pyplot as plt

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


plt.figure(figsize=(8, 6))
data[columns_to_compare].boxplot()
plt.title('Comparing Running Times for Color Region Discovery Approaches (Artificial Images)')
plt.xticks([1, 2], ['Union-Find', 'Depth-First Search'])
plt.ylabel('Time (ms)')
plt.yscale('log')  
plt.show()