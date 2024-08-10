import os
import numpy as np
import matplotlib.pyplot as plt

# Categories
categories = ['Natural Photos', 'Icons', 'Screenshots']

# Paths to the CSV files
new_files = {
    'Natural Photos': [
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_kodak.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_tecnick.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_wikipedia.csv'
    ],
    'Icons': [
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_icon_64.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_icon_512.csv'
    ],
    'Screenshots': [
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_web.csv'
    ]
}

old_files = {
    'Natural Photos': [
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_kodak.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_tecnick.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_wikipedia.csv'
    ],
    'Icons': [
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_icon_64.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_icon_512.csv'
    ],
    'Screenshots': [
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv',
        '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_web.csv'
    ]
}

# Function to read and concatenate data
def read_and_concatenate(file_paths):
    data = []
    for path in file_paths:
        data.append(np.genfromtxt(path, delimiter=','))
    return np.concatenate(data)

# Read data for each category
new_data = [read_and_concatenate(new_files[cat]) for cat in categories]
old_data = [read_and_concatenate(old_files[cat]) for cat in categories]

# Create a boxplot
fig, ax = plt.subplots()

# Plot boxplots for new and old compression rates side by side for each category
positions_new = np.arange(len(categories)) * 2.0 - 0.3  # Adjust positions for new data
positions_old = np.arange(len(categories)) * 2.0 + 0.3  # Adjust positions for old data

ax.boxplot(new_data, positions=positions_new, widths=0.5, patch_artist=True, boxprops=dict(facecolor='blue', color='blue'), medianprops=dict(color='yellow'), whiskerprops=dict(color='blue'), capprops=dict(color='blue'))
ax.boxplot(old_data, positions=positions_old, widths=0.5, patch_artist=True, boxprops=dict(facecolor='gray', color='gray'), medianprops=dict(color='red'), whiskerprops=dict(color='gray'), capprops=dict(color='gray'))

# Adding labels and title
ax.set_ylabel('Compression Rates')
ax.set_title('Boxplot of Compression Rates')
ax.set_xticks(np.arange(len(categories)) * 2.0)
ax.set_xticklabels(categories)
ax.legend(['New Compression Rates', 'Old Compression Rates'])
ax.set_yscale('log')

plt.show()
