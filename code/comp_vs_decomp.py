import csv
import os
import glob
import numpy as np
import matplotlib.pyplot as plt

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

# Known average times for the other compression method
known_mean_compression_time = 7.0  # replace with actual value
known_mean_decompression_time = 83.8  # replace with actual value

# Prepare data for the plot
methods = ['Multicut Compression', 'libpng']
compression_means = [mean_compression_time, known_mean_compression_time]
decompression_means = [mean_decompression_time, known_mean_decompression_time]

# Plot the bar chart
fig, ax = plt.subplots()

bar_width = 0.35
index = np.arange(len(methods))

bar1 = ax.bar(index, compression_means, bar_width, label='Compression Time', color='#EF3054')
bar2 = ax.bar(index + bar_width, decompression_means, bar_width, label='Decompression Time', color='#57A773')

ax.set_xlabel('Methods')
ax.set_ylabel('Time (units)')
ax.set_title('Average Compression and Decompression Times')
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(methods)
ax.legend()

# Save the plot as an image file or display it
# plt.savefig('comparison_bar_chart.png')
plt.show()
