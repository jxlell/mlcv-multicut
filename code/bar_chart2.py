import matplotlib.pyplot as plt
import numpy as np

# Categories
categories = ['Natural Photos', 'Icons', 'Screenshots']

# New Compression Rates
values = [] #[0.85, 16.7, 18.4]
min_values = [0.8, 2.52, 0.8]
max_values = [1.07, 94.3, 621]
values_std = []

# Old Compression Rates
old_values = [] #[0.98, 7.6, 4.57]
old_min_values = [0.925, 2.55, 0.927]
old_max_values = [1.23, 11.6, 12]
old_std = []

#create natural photos values
data = np.concatenate((np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_kodak.csv', delimiter=','), 
                       np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_tecnick.csv', delimiter=','),
                       np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_photo_wikipedia.csv', delimiter=',')))
mean_value = np.mean(data)
std_deviation = np.std(data)
max = np.max(data)
print("Mean value:", mean_value)
print("Standard deviation:", std_deviation)
print("Max value:", max)
values.append(mean_value)
values_std.append(std_deviation)

#create icons values
data = np.concatenate((np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_icon_64.csv', delimiter=','), 
                       np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_icon_512.csv', delimiter=',')))
mean_value = np.mean(data)
std_deviation = np.std(data)
max = np.max(data)
print("Mean value:", mean_value)
print("Standard deviation:", std_deviation)
print("Max value:", max)
values.append(mean_value)
values_std.append(std_deviation)

#create screenshots values
data = np.concatenate((np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv', delimiter=','), np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_web.csv', delimiter=',')))
mean_value = np.mean(data)
std_deviation = np.std(data)
max = np.max(data)
print("Mean value:", mean_value)
print("Standard deviation:", std_deviation)
values.append(mean_value)
values_std.append(std_deviation)


#create natural photos values
data = np.concatenate((np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_kodak.csv', delimiter=','), 
                       np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_tecnick.csv', delimiter=','),
                       np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_photo_wikipedia.csv', delimiter=',')))
mean_value = np.mean(data)
std_deviation = np.std(data)
max = np.max(data)
print("Mean value:", mean_value)
print("Standard deviation:", std_deviation)
print("Max value:", max)
old_values.append(mean_value)
old_std.append(std_deviation)

#create icons values
data = np.concatenate((np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_icon_64.csv', delimiter=','), 
                       np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_icon_512.csv', delimiter=',')))
mean_value = np.mean(data)
std_deviation = np.std(data)
max = np.max(data)
print("Mean value:", mean_value)
print("Standard deviation:", std_deviation)
print("Max value:", max)
old_values.append(mean_value)
old_std.append(std_deviation)

#create screenshots values
data = np.concatenate((np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv', delimiter=','), np.genfromtxt('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_web.csv', delimiter=',')))
mean_value = np.mean(data)
std_deviation = np.std(data)
max = np.max(data)
print("Mean value:", mean_value)
print("Standard deviation:", std_deviation)
old_values.append(mean_value)
old_std.append(std_deviation)


# Create the figure and axis objects
fig, ax = plt.subplots()

# Define bar width and offsets
bar_width = 0.4
offset = bar_width / 2

# Create positions for the bars
positions = np.arange(len(categories))

# Plotting the new compression rates
new_bars = ax.bar(positions - offset, values, width=bar_width, label='Path Representation', #yerr=values_std, 
                  capsize=5)

# Plotting the old compression rates
old_bars = ax.bar(positions + offset, old_values, width=bar_width/2, label='Edge Bitmap Representation', #yerr=old_std, 
                  capsize=5, color='gray')

ax.set_yscale('symlog')

# Adding labels and title
ax.set_ylabel('Compression Rates')
ax.set_title('Compression Rates Comparison')
ax.set_xticks(positions)
ax.set_xticklabels(categories)
ax.axhline(y=1, color='r', linestyle='--', label='No Compression Threshold')
ax.legend()

# Displaying the graph
plt.show()
