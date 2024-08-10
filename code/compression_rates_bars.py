import os
import csv
import numpy as np
import matplotlib.pyplot as plt

def read_compression_data(folder_path, categories):
    found_categories = []
    means = []
    min_values = []
    max_values = []

    # Read CSV files from the folder
    for filename in os.listdir(folder_path):
        if filename.endswith(".csv"):
            file_path = os.path.join(folder_path, filename)
            
            # Check if the filename contains any of the predefined categories
            category_name = None
            for category in categories:
                if category in filename:
                    category_name = category
                    break
            
            if category_name is None:
                continue  # Skip files that don't match any category

            with open(file_path, 'r') as file:
                reader = csv.reader(file)
                values = [float(row[0]) for row in reader]
                
                # Calculate mean, min, max
                mean_val = np.mean(values)
                min_val = np.min(values)
                max_val = np.max(values)
                
                found_categories.append(category_name)
                means.append(mean_val)
                min_values.append(mean_val - min_val)  # Error bar: mean - min
                max_values.append(max_val - mean_val)  # Error bar: max - mean

    return found_categories, means, min_values, max_values

categories = ['pngimg', 'icon_512', 'screenshot_game']

# Paths to the folders
new_compression_folder = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates'
old_compression_folder = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates'

# Read the data from both folders
new_categories, new_means, new_min_values, new_max_values = read_compression_data(new_compression_folder, categories)
old_categories, old_means, old_min_values, old_max_values = read_compression_data(old_compression_folder, categories)

# Ensure categories match for proper plotting
if set(new_categories) != set(old_categories):
    raise ValueError("Categories in both folders do not match.")

# Sort categories for consistency in plotting
new_categories_sorted = sorted(new_categories)
new_means_sorted = [new_means[new_categories.index(cat)] for cat in new_categories_sorted]
new_min_values_sorted = [new_min_values[new_categories.index(cat)] for cat in new_categories_sorted]
new_max_values_sorted = [new_max_values[new_categories.index(cat)] for cat in new_categories_sorted]

old_means_sorted = [old_means[old_categories.index(cat)] for cat in new_categories_sorted]
old_min_values_sorted = [old_min_values[old_categories.index(cat)] for cat in new_categories_sorted]
old_max_values_sorted = [old_max_values[old_categories.index(cat)] for cat in new_categories_sorted]

# Create the figure and axis objects
fig, ax = plt.subplots()

# Define bar width and offsets
bar_width = 0.4
offset = bar_width / 2

# Create positions for the bars
positions = np.arange(len(new_categories_sorted))

# Plotting the new compression rates
new_bars = ax.bar(positions - offset, new_means_sorted, width=bar_width, 
                  label='Path Representation', 
                  yerr=[new_min_values_sorted, new_max_values_sorted], capsize=5)

# Plotting the old compression rates
old_bars = ax.bar(positions + offset, old_means_sorted, width=bar_width/2, 
                  label='Edge Bitmap Representation', 
                  yerr=[old_min_values_sorted, old_max_values_sorted], capsize=5, color='gray')

# Set y-axis to symmetric logarithmic scale
ax.set_yscale('symlog')

# Adding labels and title
ax.set_ylabel('Compression Rates')
ax.set_title('Compression Rates Comparison')
ax.set_xticks(positions)
ax.set_xticklabels(new_categories_sorted)
ax.axhline(y=1, color='r', linestyle='--', label='No Compression Threshold')
ax.legend()

# Displaying the graph
plt.show()
