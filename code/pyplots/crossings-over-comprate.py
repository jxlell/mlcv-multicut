import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots_new.csv',
    'code/output_files/mc_results_test_textures_new.csv',
    'code/output_files/mc_results_test_photos_new.csv',
    'code/output_files/mc_results_test_icons_new.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
df = pd.concat(data_frames, ignore_index=True)

# Calculate the difference between crossings and disc_comp
df['crossings_minus_disc_comp'] = df['crossings'] - df['disc_comp']

# Calculate the relative difference (factor) between tree_rate and 2bit_rate
df['rate_difference_factor'] = (df['tree_rate'] - df['2bit_rate']) / df['2bit_rate']

# Set up the plot
fig, ax = plt.subplots(figsize=(10, 6))

# Scatter plot for all points
ax.scatter(df['crossings'], df['rate_difference_factor'], alpha=0.6, color='black', marker='x', s=10)
# Add a horizontal line at y=0
ax.axhline(y=0, color='red', linestyle='--', linewidth=1)
# Set labels and title
ax.set_xlabel("Crossings")
ax.set_ylabel("Compression Rate Ratio: DT/DEC")
ax.set_title("Scatter Plot of DT-DEC Rate Difference Factor vs Crossings")

# Find the row with the maximum absolute rate difference
max_diff_row = df.loc[df['rate_difference_factor'].abs().idxmax()]
print(max_diff_row["tree_rate"], max_diff_row["2bit_rate"])
# Find the row with the minimum absolute rate difference
min_diff_row = df.loc[df['rate_difference_factor'].abs().idxmin()]
print(min_diff_row["tree_rate"], min_diff_row["2bit_rate"])
# Print the filename for the row with the smallest rate difference
print(f"Filename with the smallest rate difference: {min_diff_row['filename']}")
# Print the filename for the row with the biggest rate difference
print(f"Filename with the biggest rate difference: {max_diff_row['filename']}")

# Show the plot
plt.tight_layout()
plt.xscale('log')
plt.show()
