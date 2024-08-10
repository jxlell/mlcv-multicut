import matplotlib.pyplot as plt
import numpy as np

# Categories
categories = ['Natural Photos', 'Icons', 'Screenshots']

# Initialize lists for storing values and IQRs
values = []
values_iqr = []
old_values = []
old_iqr = []

# Helper function to compute mean and IQR
def compute_stats(file_paths):
    data = np.concatenate([np.genfromtxt(file, delimiter=',') for file in file_paths])
    mean_value = np.mean(data)
    q75, q25 = np.percentile(data, [75 ,25])
    iqr = q75 - q25
    return mean_value, iqr

# Create natural photos values
file_paths = [
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_kodak.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_tecnick.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_wikipedia.csv'
]
mean_value, iqr = compute_stats(file_paths)
values.append(mean_value)
values_iqr.append(iqr)

# Create icons values
file_paths = [
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_icon_64.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_icon_512.csv'
]
mean_value, iqr = compute_stats(file_paths)
values.append(mean_value)
values_iqr.append(iqr)

# Create screenshots values
file_paths = [
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_web.csv'
]
mean_value, iqr = compute_stats(file_paths)
values.append(mean_value)
values_iqr.append(iqr)

# Create old compression values
file_paths = [
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_kodak.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_tecnick.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_wikipedia.csv'
]
mean_value, iqr = compute_stats(file_paths)
old_values.append(mean_value)
old_iqr.append(iqr)

# Create icons values
file_paths = [
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_icon_64.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_icon_512.csv'
]
mean_value, iqr = compute_stats(file_paths)
old_values.append(mean_value)
old_iqr.append(iqr)

# Create screenshots values
file_paths = [
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv',
    '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_web.csv'
]
mean_value, iqr = compute_stats(file_paths)
old_values.append(mean_value)
old_iqr.append(iqr)

# Convert lists to NumPy arrays for element-wise operations
values = np.array(values)
values_iqr = np.array(values_iqr)
old_values = np.array(old_values)
old_iqr = np.array(old_iqr)

# Create the figure and axis objects
fig, ax = plt.subplots()

# Define bar width and offsets
bar_width = 0.4
offset = bar_width / 2

# Create positions for the bars
positions = np.arange(len(categories))

# Plotting the new compression rates
new_bars = ax.bar(positions - offset, values, width=bar_width, label='Path Representation', yerr=[values_iqr / 2, values_iqr / 2], capsize=5)

# Plotting the old compression rates
old_bars = ax.bar(positions + offset, old_values, width=bar_width / 2, label='Edge Bitmap Representation', yerr=[old_iqr / 2, old_iqr / 2], capsize=5, color='gray')

ax.set_yscale('symlog')

# Adding labels and title
ax.set_ylabel('Compression Rates')
ax.set_title('Compression Rates Comparison')
ax.set_xticks(positions)
ax.set_xticklabels(categories)
ax.axhline(y=1, color='r', linestyle='--', label='No Compression Threshold')
ax.legend()


# Add plot caption
plt.figtext(0.5, 0.01, 'Comparing Compression Rates of Bitmap and Path Approach with IQR', ha='center', fontsize=10)


# Displaying the graph
plt.show()
