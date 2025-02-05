import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import csv

category = 'photo_wikipedia'
data = pd.read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/path_vs_rle/path_vs_rle_' + category + '.csv', header=None)

percentages = data.iloc[0]
percentages = percentages.to_list()
max_index = percentages.index(max(percentages))
min_index = percentages.index(min(percentages))
print(f'Index of highest improvement: {max_index}')
print(f'Index of highest deterioration: {min_index}')

disconnected_comp_file = pd.read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/disconnected_components/output_' + category + '.csv', header=None)
disconnected_comp = disconnected_comp_file.iloc[0]
disconnected_comp = disconnected_comp.to_list()
print(f'Disconnected components for highest improvement: {disconnected_comp[max_index]}')
print(f'Disconnected components for highest deterioration: {disconnected_comp[min_index]}')

two_bit_paths_amount_file = pd.read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/twobit_paths_amounts/output_' + category + '.csv', header=None)
two_bit_paths_amount = two_bit_paths_amount_file.iloc[0]
two_bit_paths_amount = two_bit_paths_amount.to_list()
print(f'Two-bit paths amount for highest improvement: {two_bit_paths_amount[max_index]}')
print(f'Two-bit paths amount for highest deterioration: {two_bit_paths_amount[min_index]}')




sorted_percentages = percentages.copy()
sorted_percentages.sort()

indices = range(len(sorted_percentages))

colors = ['red' if p < 0 else 'green' for p in sorted_percentages]

plt.figure(figsize=(10, 6))
plt.bar(indices, sorted_percentages, color=colors)
plt.xlabel('Change in Compression Rate (%) for using 2-bit paths and RLE (' + category + ')')
plt.ylabel('Percentage Difference')
plt.axhline(0, color='black', linewidth=0.8)
plt.savefig('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/plots/paths_vs_rle_' + category + '.png')
plt.show()
