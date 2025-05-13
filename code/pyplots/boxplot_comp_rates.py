import pandas as pd

import matplotlib.pyplot as plt

# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots.csv'
    # ,'code/output_files/mc_results_test_textures_no_pk.csv'
    # ,'code/output_files/mc_results_test_textures_pk.csv'
    # ,'code/output_files/mc_results_test_photos.csv'
    # ,'code/output_files/mc_results_test_icons.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Specify the columns to compare
columns_to_compare = ['red_edgebits_rate', 'tree_rate', 'rle_rate', 'straights_huffman_rate']  # Replace with your column names

# Check if all specified columns exist in the data
for col in columns_to_compare:
    if col not in data.columns:
        raise ValueError(f"Column '{col}' not found in the CSV file.")

print("Median Compression Rates:")
medians = data[columns_to_compare].median()
print(medians)

# Create the boxplot
plt.figure(figsize=(8, 6))
data[columns_to_compare].boxplot()
plt.title('Comparison of Compression Rates (Icons)')
plt.ylabel('Compression Rate')
plt.xlabel('Columns')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization

# Save the plot or show it
# plt.savefig('/Users/jalell/Documents/GitHub/thesis-multicut/img/boxplot_rates-screenshots.pdf')  # Save the plot as a PNG file
plt.show()