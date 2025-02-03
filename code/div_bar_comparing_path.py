import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import csv

category = 'icon_64'
data = pd.read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/path_vs_rle/path_vs_rle_' + category + '.csv', header=None)

percentages = data.iloc[0]
percentages = percentages.to_list()
max_index = percentages.index(max(percentages))
min_index = percentages.index(min(percentages))
print(f'Index of highest percentage: {max_index}')
print(f'Index of lowest percentage: {min_index}')
percentages.sort()

indices = range(len(percentages))

colors = ['red' if p < 0 else 'green' for p in percentages]

plt.figure(figsize=(10, 6))
plt.bar(indices, percentages, color=colors)
plt.xlabel('Change in Compression Rate (%) for using 2-bit paths and RLE (' + category + ')')
plt.ylabel('Percentage Difference')
plt.axhline(0, color='black', linewidth=0.8)
plt.savefig('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/plots/paths_vs_rle_' + category + '.png')
plt.show()
