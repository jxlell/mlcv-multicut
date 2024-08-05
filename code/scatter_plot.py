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
data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv')
data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv')

data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_times/output_icon_512.csv')
data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/pixel_sizes/output_icon_512.csv')

data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_times/output_screenshot_game.csv')
data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/decompression_times/output_screenshot_game.csv')

data1 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/old_compression_rates/output_screenshot_game.csv')
data2 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_screenshot_game.csv')
data3 = read_csv('/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/disconnected_components/output_screenshot_game.csv')
data3 = np.array(data3)
#inverted_sizes = 1 / (data3 + 0.01) * 100  # Adjust the scaling factor as needed

# Print the maximum value of each data array
print(f"Maximum value in data1 (compression_rates): {max(data1)}")
print(f"Maximum value in data2 (old_compression_rates): {max(data2)}")

plt.scatter(data1, data2, s=data3/30, alpha=0.5)
plt.xlabel('old_compresssion_rates')
plt.ylabel('compression_rates')
plt.title('Scatter Plot')
plt.show()