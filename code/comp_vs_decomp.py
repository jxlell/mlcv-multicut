import csv
import os
import glob
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.lines import Line2D

def read_csv(file_path):
    values = []
    with open(file_path, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            for value in row:
                try:
                    values.append(float(value))
                except ValueError:
                    print(f"Error converting value to float: {value}")
    return values

def calculate_mean(values):
    if len(values) == 0:
        return 0
    return np.mean(values)

# Paths to the directories containing the CSV files
compression_dir = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_times'
decompression_dir = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/decompression_times'

# Get a list of all CSV files in the directories
compression_files = glob.glob(os.path.join(compression_dir, '*.csv'))
decompression_files = glob.glob(os.path.join(decompression_dir, '*.csv'))

# Calculate the mean compression and decompression times for your method
compression_times = []
decompression_times = []

for file_path in compression_files:
    compression_times.extend(read_csv(file_path))

for file_path in decompression_files:
    decompression_times.extend(read_csv(file_path))

mean_compression_time = calculate_mean(compression_times)
mean_decompression_time = calculate_mean(decompression_times)

mc_max_encode = np.max(compression_times)
mc_min_encode = np.min(compression_times)
mc_max_decode = np.max(decompression_times)
mc_min_decode = np.min(decompression_times)

# Known average times for the other compression method


df = pd.read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/parsed_qoi_results_sorted.csv')
known_mean_compression_time = 7.0 #df['encode_ms'].mean() 
known_mean_decompression_time = 83.8  #df['decode_ms'].mean()  
print(f"Mean compression time: {mean_compression_time}")
print(f"Mean decompression time: {mean_decompression_time}")
# Calculate the minimum and maximum for encode_ms and decode_ms
min_encode_ms = df['encode_ms'].min()
max_encode_ms = df['encode_ms'].max()
min_decode_ms = df['decode_ms'].min()
max_decode_ms = df['decode_ms'].max()

print(f"Min encode_ms: {min_encode_ms}")
print(f"Max encode_ms: {max_encode_ms}")
print(f"Min decode_ms: {min_decode_ms}")
print(f"Max decode_ms: {max_decode_ms}")

# Prepare data for the plot
methods = ['Multicut Compression', 'PNG (libpng)']
compression_means = [mean_compression_time, known_mean_compression_time]
decompression_means = [mean_decompression_time, known_mean_decompression_time]
print(compression_means)
print(decompression_means)
# Calculate the error values
compression_error = [[mean_compression_time - mc_min_encode, known_mean_compression_time - min_encode_ms], 
                     [mc_max_encode - mean_compression_time, max_encode_ms - known_mean_compression_time]]

decompression_error = [[mean_decompression_time - mc_min_decode, known_mean_decompression_time - min_decode_ms], 
                       [mc_max_decode - mean_decompression_time, max_decode_ms - known_mean_decompression_time]]

# Plot the bar chart
fig, ax = plt.subplots()

bar_width = 0.35
index = np.arange(len(methods))

bar1 = ax.bar(index, compression_means, bar_width, label='Compression Time', 
              yerr=compression_error, capsize=5)
bar2 = ax.bar(index + bar_width, decompression_means, bar_width, label='Decompression Time', 
              color='gray', yerr=decompression_error, capsize=5)

error_bar_legend = Line2D([0], [0], color='black', lw=1, marker='_', markersize=10, markeredgewidth=1, linestyle='None', label='Error Bar: Min-Max Range')


ax.set_xlabel('Methods')
ax.set_ylabel('Time (ms)')
ax.set_title('Average Compression and Decompression Times')
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(methods)
ax.legend(handles=[bar1, bar2, error_bar_legend],loc='upper right')
ax.set_yscale('symlog')

# Save the plot as an image file or display it
# plt.savefig('comparison_bar_chart.png')
plt.show()
