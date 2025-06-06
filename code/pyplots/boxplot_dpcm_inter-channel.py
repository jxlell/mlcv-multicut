import pandas as pd

import matplotlib.pyplot as plt

# Replace with your CSV file path
csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_sample.csv'


# Load CSV
df = pd.read_csv(csv_file)

# Replace with your actual column names
col1 = 'dpcm_deflate_bits'
col2 = 'dpcm_deflate_bits_no_inter'

# Prepare data for boxplot
data = [df[col1].dropna(), df[col2].dropna()]

print(f"Mean of {col1}: {df[col1].mean()}")
print(f"Mean of {col2}: {df[col2].mean()}")

print(f"Median of {col1}: {df[col1].median()}")
print(f"Median of {col2}: {df[col2].median()}")

plt.boxplot(data, labels=[col1, col2])
plt.xticks([1, 2], ["Channel-wise DPCM", "Inter-channel DPCM"])
plt.ylabel('Bits for DPCM Encoding + Deflate stage')
plt.title('Comparing DPCM Encoding with and without Inter-channel Compression')
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
plt.show()