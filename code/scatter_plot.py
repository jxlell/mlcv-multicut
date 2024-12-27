import csv

import matplotlib.pyplot as plt
import numpy as np

def read_csv(file_path):
    numbers = []
    with open(file_path, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            for number in row:
                numbers.append(float(number))
    return numbers

# Replace 'file1.csv' and 'file2.csv' with the actual file paths
#data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv')
#data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv')

#data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_times/output_icon_512.csv')
#data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/pixel_sizes/output_icon_512.csv')

#data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_times/output_screenshot_game.csv')
#data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/decompression_times/output_screenshot_game.csv')

path1 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv'
path2 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_web.csv'

path3 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv'
path4 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_web.csv'

file_paths = [path1, path2]
data1 = np.concatenate([np.genfromtxt(file, delimiter=',') for file in file_paths])
file_paths = [path3, path4]
data2 = np.concatenate([np.genfromtxt(file, delimiter=',') for file in file_paths])
print(f"Size of data2: {data2.size}")
path5 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/disconnected_components/output_screenshot_game.csv'
path6 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/disconnected_components/output_screenshot_web.csv'
file_paths = [path5, path6]
data3 = np.concatenate([np.genfromtxt(file, delimiter=',') for file in file_paths])
print(f"Size of data3: {data3.size}")
#data3 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/disconnected_components/output_screenshot_game.csv')
#data3 = np.array(data3)
#inverted_sizes = 1 / (data3 + 0.01) * 100  # Adjust the scaling factor as needed

# Print the maximum value of each data array
print(f"Maximum value in data1 (compression_rates): {max(data1)}")
print(f"Maximum value in data2 (old_compression_rates): {max(data2)}")

plt.scatter(data1, data2, #s=7,
            s=data3/30, 
            alpha=0.5
            )
xname = (path1.split('/')[-2]).replace('_', ' ')
yname = (path2.split('/')[-2]).replace('_', ' ')
plt.axhline(y=1, color='r', linestyle='--', label='Compression Threshold')
plt.axvline(x=1, color='r', linestyle='--')
sizes = [50, 100, 500]  # Example sizes for legend
labels = [' ', 'Number of Disconnected Components', ' ']  # Corresponding labels

# Create legend entries
for size, label in zip(sizes, labels):
    plt.scatter([], [], s=size / 30, alpha=0.5, label=f'{label}')

# Adding the rest of the legend
plt.legend()

plt.legend()

plt.xlabel("Edge Bitmap Compression Rates")
plt.ylabel("Path Compression Rates")
plt.yscale('log')
#plt.xscale('log')
plt.title('Edge Bitmap Compression Rates over Path Compression Rates - Screenshots')
plt.show()