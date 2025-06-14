import pandas as pd
import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 12,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Replace with your CSV file path
csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_sample.csv'

# Load CSV
df = pd.read_csv(csv_file)

# Replace with your actual column names
col1 = 'dpcm_deflate_bits'
col2 = 'dpcm_deflate_bits_no_inter'
pixels_col = 'pixels'

# Compute bits per pixel
bpp1 = df[col1] / df[pixels_col]
bpp2 = df[col2] / df[pixels_col]

# Prepare data for boxplot
data = [bpp1.dropna(), bpp2.dropna()]
# data = [df[col1].dropna(), df[col2].dropna()]

print(f"Mean BPP of {col1}: {bpp1.mean()}")
print(f"Mean BPP of {col2}: {bpp2.mean()}")

print(f"Median BPP of {col1}: {bpp1.median()}")
print(f"Median BPP of {col2}: {bpp2.median()}")

plt.boxplot(data, labels=[col1, col2])
plt.xticks([1, 2], ["Inter-channel DPCM", "Channel-wise DPCM"])
plt.ylabel('Bits per pixel')
plt.title('Comparing DPCM Encoding Approaches (Sample Image Data)')
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
# plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
plt.show()