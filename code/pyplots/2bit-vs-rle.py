import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV data
df = pd.read_csv("code/output_files/mc_results_test_icons.csv")

# Calculate the factor (ratio) between 2bit_rate and rle_rate
df['rate_factor'] = df['2bit_rate'] / df['rle_rate']

# Sort the data by rate_factor in ascending order
df_sorted = df.sort_values(by='rate_factor', ascending=True)

# Set up the plot
fig, ax = plt.subplots(figsize=(10, 6))

# Scatter plot showing the rate factor (2bit_rate / rle_rate)
ax.scatter(df_sorted['filename'], df_sorted['rate_factor'], alpha=0.7, color='b')

# Add a vertical line at rate factor = 1
ax.axhline(y=1, color='r', linestyle='--', linewidth=1)
# Remove x-axis labels
ax.set_xticks([])
# Add labels and title
ax.set_xlabel("Image Filename")
ax.set_ylabel("Rate Factor (2bit_rate / rle_rate)")
ax.set_title("Rate Factor Comparison (2bit_rate / rle_rate) for Each Image")

# Rotate x-axis labels for better readability
plt.xticks(rotation=90)

# Show the plot
plt.tight_layout()
plt.show()
