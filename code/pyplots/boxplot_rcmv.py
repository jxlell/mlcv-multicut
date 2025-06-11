import pandas as pd

import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 18,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Load the CSV files and combine them into one DataFrame
csv_files = [
    # 'code/output_files/mc_results_test_screenshots_new.csv'
    # ,'code/output_files/mc_results_test_textures_new.csv'
    # ,'code/output_files/mc_results_test_photos_new.csv'
    'code/output_files/mc_results_test_sample.csv'
]



# Read and concatenate all CSV files
data_frames = [pd.read_csv(file) for file in csv_files]
data = pd.concat(data_frames, ignore_index=True)

# Create two side-by-side boxplots comparing values of two columns
columns_to_compare = ['edgebits', 'edgeBitsMCBits']  # Replace with your actual column names

data['edgebits_amount'] = data['edgebits'] * (data['mc_percentage'] / 100)

data['edgebits_bpe'] = data['edgebits'] / data['edgebits_amount']
data['edgeBitsMCBits_bpe'] = data['edgeBitsMCBits'] / data['edgebits_amount']

data['edgebits_bpp'] = data['edgebits'] / data['edgebits']
data['edgeBitsMCBits_bpp'] = data['edgeBitsMCBits'] / data['edgebits']

# Calculate medians
median_edgebits = data['edgebits'].median()
median_rcmv_bits = data['edgeBitsMCBits'].median()

# Calculate reduction rate
reduction_rate = (median_edgebits - median_rcmv_bits) / median_edgebits * 100
# Calculate reduction factor
reduction_factor = median_edgebits / median_rcmv_bits
print(f"Reduction factor: {reduction_factor:.2f}")

print(f"Median edgebits: {median_edgebits:.2f}")
print(f"Median rCMV bits: {median_rcmv_bits:.2f}")
print(f"Reduction rate: {reduction_rate:.2f}%")

plt.figure(figsize=(8, 6))

# Prepare data for boxplot (drop NaNs)
box_data = [
    data['edgebits_bpp'].dropna(),
    data['edgeBitsMCBits_bpp'].dropna()
]

# Create the boxplot using matplotlib directly
plt.boxplot(box_data)

# Set custom tick labels
plt.xticks([1, 2], ['CMV', 'rCMV'])

# Add titles and labels
plt.title('Comparing CMV and rCMV Bits per edge \n(Sample Image Data)')
plt.ylabel('Bits per edge (bpe)')
plt.grid(True, linestyle='--', linewidth=0.5, alpha=0.7)
plt.yscale('log')  
plt.show()
