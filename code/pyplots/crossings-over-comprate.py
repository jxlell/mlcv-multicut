import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv',
    'code/output_files/mc_results_test_textures_final.csv',
    'code/output_files/mc_results_test_photos_final.csv',
    'code/output_files/mc_results_test_icons_final.csv',
    # 'code/output_files/mc_results_test_sample.csv',
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
df = pd.concat(data_frames, ignore_index=True)

# Calculate the difference between crossings and disc_comp
df['crossings_minus_disc_comp'] = df['crossings'] - df['disc_comp']
df['cross_coefficient'] = df['crossings'] / df['disc_comp'] 

# Calculate the relative difference (factor) between tree_rate and 2bit_rate
df['rate_difference_factor'] = (df['tree_rate'] - df['2bit_rate']) / df['2bit_rate']
df['rate_quotient'] = df['tree_rate'] / df['2bit_rate']

df['crossings_per_pixel'] = df['crossings'] / df['pixels']

x = df['crossings_per_pixel'].values
y = df['rate_difference_factor'].values

# Fit a polynomial of degree 2
coeffs = np.polyfit(x, y, deg=2)
poly_eq = np.poly1d(coeffs)

# Create smooth x values for plotting
x_fit = np.linspace(x.min(), x.max(), 500)
y_fit = poly_eq(x_fit)

# Set up the plot
fig, ax = plt.subplots(figsize=(10, 6))

# ax.plot(x_fit, y_fit, color='blue', label='Quadratic Fit')

# Scatter plot for all points
ax.scatter(df['cross_coefficient'], df['rate_quotient'], alpha=0.6, color='black', marker='x', s=10)
# ax.scatter(df['crossings_minus_disc_comp'], df['tree_rate'], alpha=0.6, color='blue', marker='o', s=10)
# Add a horizontal line at y=0
ax.axhline(y=0, color='red', linestyle='--', linewidth=1)
# Set labels and title
ax.set_xlabel("Crossings per component of connected edges")
ax.set_ylabel("Compression Rate Ratio: DT/DEC")
ax.set_title("Scatter Plot of DT-DEC Rate Difference Factor vs Crossings per Component\n(All Images)")

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
# plt.yscale('log')
plt.show()
