import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Path to the folder containing CSV files
folder_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/mlcv-multicut/code/output_files'

# Initialize lists to store average compression rates and file names
avg_rates = []
file_names = []

# Iterate over files in the folder
for file_name in os.listdir(folder_path):
    if file_name.endswith('.csv'):
        # Read the contents of the CSV file
        with open(os.path.join(folder_path, file_name), 'r') as file:
            content = file.read()

        # Parse the comma-separated float values into a list of floats
        compression_rates = [float(rate) for rate in content.split(',')]

        # Compute the average compression rate
        avg_rate = np.mean(compression_rates)

        # Append the average compression rate and file name to lists
        avg_rates.append(avg_rate)
        file_names.append(file_name)

# Plot the bar chart
plt.figure(figsize=(10, 6))
plt.bar(file_names, avg_rates, color='skyblue')
plt.xlabel('File Name')
plt.ylabel('Average Compression Rate')
plt.title('Average Compression Rate by File')
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.show()