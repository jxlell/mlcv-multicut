import pandas as pd
import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 14,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Load the CSV files and combine them into one DataFrame
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv'
    ,'code/output_files/mc_results_test_textures_final.csv'
    ,'code/output_files/mc_results_test_photos_final.csv'
    ,'code/output_files/mc_results_test_icons_final.csv'
]

# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
df = pd.concat(data_frames, ignore_index=True)

# Choose two columns to plot
x = df["avg_straight_lenght"]          # X-axis
y = df["paths2bit_direction_bits"]/df["rle_direction_bits"]                   # Y-axis

# Create scatter plot
plt.scatter(x, y, color='black', alpha=0.5, marker='x', s=10)
plt.axhline(y=1, color='red', linestyle='--', linewidth=1)

# Labels and title
plt.xlabel("Normalized average straight length")
plt.ylabel("DEC directional bit reduction ratio")
plt.title("Reduction of the DEC directional bits by applying RLE\n compared to the average straight length (All Images)", fontsize=15)
plt.grid(True)
plt.tight_layout()
plt.xscale('log')
plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
plt.show()
