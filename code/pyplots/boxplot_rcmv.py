import pandas as pd

import matplotlib.pyplot as plt

# Load the CSV files and combine them into one DataFrame
csv_files = [
    # 'code/output_files/mc_results_test_screenshots_new.csv'
    # ,'code/output_files/mc_results_test_textures_new.csv'
    # ,'code/output_files/mc_results_test_photos_new.csv'
    'code/output_files/mc_results_test_icons_new.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Create two side-by-side boxplots comparing values of two columns
columns_to_compare = ['edgebits', 'rcmv_bits']  # Replace with your actual column names

# Calculate medians
median_edgebits = data['edgebits'].median()
median_rcmv_bits = data['rcmv_bits'].median()

# Calculate reduction rate
reduction_rate = (median_edgebits - median_rcmv_bits) / median_edgebits * 100
# Calculate reduction factor
reduction_factor = median_edgebits / median_rcmv_bits
print(f"Reduction factor: {reduction_factor:.2f}")

print(f"Median edgebits: {median_edgebits:.2f}")
print(f"Median rCMV bits: {median_rcmv_bits:.2f}")
print(f"Reduction rate: {reduction_rate:.2f}%")

plt.figure(figsize=(8, 6))
data[columns_to_compare].boxplot()
plt.title('Comparing CMV and rCMV Bits (Icons)')
plt.xticks([1, 2], ['CMV', 'rCMV'])
plt.ylabel('Bits')
plt.show()