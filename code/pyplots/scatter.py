import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots.csv'
    ,'code/output_files/mc_results_test_textures_no_pk.csv'
    ,'code/output_files/mc_results_test_textures_pk.csv'
    ,'code/output_files/mc_results_test_photos.csv'
    ,'code/output_files/mc_results_test_icons.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
df = pd.concat(data_frames, ignore_index=True)

# Choose two columns to plot
x = df["avg_straight_lenght"]          # X-axis
y = df["paths2bit_direction_bits"]/df["rle_direction_bits"]                   # Y-axis

# Create scatter plot
plt.scatter(x, y, color='black', alpha=0.5, marker='x', s=10)

# Labels and title
plt.xlabel("Average straight length")
plt.ylabel("DEC directional bit reduction ratio")
plt.title("DEC directional bit reduction ratio vs average straight length")
plt.grid(True)
plt.tight_layout()
plt.xscale('log')
plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
plt.show()
