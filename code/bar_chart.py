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
                try:
                    percentages.append(float(percentage))
                except ValueError:
                    print(f"Error converting value to float: {percentage}")
    return percentages

directory_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/straights_compress_rates'

# Get a list of all CSV files in the directory
csv_files = glob.glob(os.path.join(directory_path, '*.csv'))

# Prepare data for the plot
bars = []
mean_values = []

# Process each CSV file
for file_path in csv_files:
    percentages = read_csv(file_path)
    
    # Calculate the mean percentage value
    mean_percentage = np.mean(percentages)
    print(f"Mean percentage for {file_path}: {mean_percentage}")  # Debugging line
    print("Max value: ")
    print(np.max(percentages))
    
    # Extract the file name without extension to use as the bar label
    bar_label = os.path.splitext(os.path.basename(file_path))[0]
    
    # Append the values for plotting
    bars.append(bar_label)
    mean_values.append(mean_percentage)

# Plot the bar chart
fig, ax = plt.subplots()

# One bar for the mean percentage (color: red)
ax.bar(bars, mean_values, color='#EF3054')

# Add a horizontal line at 50%
ax.axhline(1, color='#F0CF65', linewidth=1, linestyle='--', label='50% Line')

# Set y-axis limit to go from zero to the maximum mean value
ax.set_ylim(0, max(mean_values) * 1.1)  # Adding 10% padding above the maximum value

ax.set_ylabel('Percentage')
ax.set_title('Bar Chart of Mean Percentages')
ax.legend()

plt.xticks(rotation=90)

# Save the plot as an image file or display it
#plt.savefig(os.path.join(directory_path, 'bar_chart.png'))
plt.show()
