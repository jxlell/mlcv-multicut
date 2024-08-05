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

# Define paths for the folders containing CSV files
compression_folder = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_times'
decompression_folder = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/decompression_times'
pixels_folder = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/pixel_sizes'

# Get a list of all CSV files in the folders
compression_files = glob.glob(os.path.join(compression_folder, '*.csv'))
decompression_files = glob.glob(os.path.join(decompression_folder, '*.csv'))
pixels_files = glob.glob(os.path.join(pixels_folder, '*.csv'))

# Prepare data for the plot
bars = []
compression_means = []
decompression_means = []
pixel_means = []

# Process each CSV file pair
for compression_file in compression_files:
    category = os.path.splitext(os.path.basename(compression_file))[0]
    decompression_file = os.path.join(decompression_folder, category + '.csv')
    pixel_file = os.path.join(pixels_folder, category + '.csv')
    
    if os.path.exists(decompression_file) and os.path.exists(pixel_file):
        compression_values = read_csv(compression_file)
        decompression_values = read_csv(decompression_file)
        pixel_values = read_csv(pixel_file)
        
        compression_mean = calculate_mean(compression_values)
        decompression_mean = calculate_mean(decompression_values)
        pixel_mean = calculate_mean(pixel_values)
        
        bars.append(category)
        compression_means.append(compression_mean)
        decompression_means.append(decompression_mean)
        pixel_means.append(pixel_mean)
    else:
        print(f"Decompression or pixel file for {category} not found. Skipping...")

# Plot the stacked bar chart
fig, ax1 = plt.subplots()

# Plot compression times (color: red) and decompression times (color: green) as stacked bars
ax1.bar(bars, compression_means, label='Compression Time', color='#EF3054')
ax1.bar(bars, decompression_means, bottom=compression_means, label='Decompression Time', color='#57A773')

ax1.set_ylabel('Time (units)')
ax1.set_title('Stacked Bar Chart of Compression and Decompression Times with Pixel Amounts')
ax1.legend(loc='upper left')

# Add a secondary y-axis for pixel amounts
ax2 = ax1.twinx()
ax2.plot(bars, pixel_means, color='#0077BB', marker='o', linestyle='-', label='Pixel Amount')
ax2.set_ylabel('Pixel Amount')
ax2.legend(loc='upper right')

plt.xticks(rotation=90)

# Save the plot as an image file or display it
# plt.savefig('stacked_bar_chart_with_pixels.png')
plt.show()
