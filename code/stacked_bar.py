import csv
import os
import glob
import numpy as np

import matplotlib.pyplot as plt

def read_csv(file_path):
    percentages = []
    with open(file_path, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            for percentage in row:
                percentages.append(float(percentage))
    return percentages

# Replace 'your_directory' with the actual directory path containing the CSV files
directory_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/multicut_percentages'

# Get a list of all CSV files in the directory
csv_files = glob.glob(os.path.join(directory_path, '*.csv'))

# Prepare data for the plot
bars = []
mean_values = []
remainder_values = []

# Process each CSV file
for file_path in csv_files:
    percentages = read_csv(file_path)
    
    # Calculate the mean percentage value
    mean_percentage = np.mean(percentages)
    remainder = 100 - mean_percentage
    
    # Extract the file name without extension to use as the bar label
    bar_label = os.path.splitext(os.path.basename(file_path))[0]#.split('_')[1]
    
    # Append the values for plotting
    bars.append(bar_label)
    mean_values.append(mean_percentage)
    remainder_values.append(remainder)

# Sort the bars/categories by the mean percentage in ascending order
sorted_indices = np.argsort(mean_values)
bars = [bars[i] for i in sorted_indices]
mean_values = [mean_values[i] for i in sorted_indices]
remainder_values = [remainder_values[i] for i in sorted_indices]

# Plot the stacked bar chart
fig, ax = plt.subplots()

# One bar for the mean percentage (red) and another for the remainder to 100% (green)
ax.bar(bars, mean_values, label='multicut edges', color='#57A773')
ax.bar(bars, remainder_values, bottom=mean_values, label='remaining edges', color='#EF3054')

# Add a horizontal line at 50%
ax.axhline(50, color='black', linewidth=1, linestyle='--', label='50%')

ax.set_ylim(0, 100)
ax.set_ylabel('Percentage of Multicut Edges')
ax.set_title('Stacked Bar Chart of Mean Percentages')
ax.legend()

plt.xticks(rotation=45, ha='right')

# Save the plot as an image file or display it
#plt.savefig(os.path.join(directory_path, 'stacked_bar_chart.png'))
plt.show()
