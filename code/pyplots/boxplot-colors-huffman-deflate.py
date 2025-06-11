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



df = pd.read_csv("code/output_files/mc_results_test_sample.csv")

columns_to_compare = ['region_color_bits', 'dpcm-huffman_bits', 'dpcm_deflate_bits']  # Replace with your actual column names

# Calculate bpp (bits per pixel) for each method
for col in columns_to_compare:
    df[f'{col}_bpp'] = df[col] / df['edgebits']

# Prepare data for boxplot
bpp_columns = [f'{col}_bpp' for col in columns_to_compare]
bpp_data = [df[col] for col in bpp_columns]

# Create boxplot
plt.boxplot(bpp_data, labels=columns_to_compare)
plt.xticks([1, 2, 3], ["no encoding", "Differential \n+ Huffman", "Differential \n+ Deflate"])
plt.ylabel('Bits per edge (bpe)')
plt.grid(which='both', linestyle='--', alpha=0.7)
plt.title('Comparing bits per edge (bpe) for different color \nencoding methods (Sample Image Data)')
plt.show()