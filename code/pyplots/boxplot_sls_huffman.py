import pandas as pd
import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 15,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Replace with your CSV file path
csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_sample.csv'

df = pd.read_csv(csv_file)

# Load CSV

col1 = "straights_mc_bits"
col2 = "sls_mc_bits"

pixels_col = 'edgebits'
avg_straight_length_col = 'avg_straight_lenght'

# Compute bits per pixel
df[col1 + '_bpp'] = df[col1] / df[pixels_col]
df[col2 + '_bpp'] = df[col2] / df[pixels_col]

boxplot_columns = [col1 + '_bpp', col2 + '_bpp']
df['col2_improvement'] = df[col1 + '_bpp'] / df[col2 + '_bpp']

plt.figure(figsize=(8, 6))
# plt.scatter(df[avg_straight_length_col], df[col1 + '_bpp'], label='SLS without Huffman', alpha=0.7)
# plt.scatter(df[avg_straight_length_col], df[col2 + '_bpp'], label='SLS with Huffman', alpha=0.7)
# plt.xlabel('Average Straight Length')
# plt.ylabel('Bits per pixel (bpp)')
# plt.title('Bits per pixel vs. Average Straight Length')
# plt.legend()
# # plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
# plt.grid(True, linestyle='--', linewidth=0.5, alpha=0.7)
# plt.show()

# plt.boxplot([df['tree_stack_bpp'], df['treeMCBits_bpp']], labels=['tree_stack', column])
plt.boxplot([df[boxplot_columns[0]], df[boxplot_columns[1]]], labels=boxplot_columns)
plt.ylabel('Bits per edge (bpe)')
plt.grid(True, which="both", linestyle='--', linewidth=0.5, alpha=0.7)
plt.xticks([1, 2], ["SLS without Huffman", "SLS with Huffman"])
plt.title('Comparing bits per edge (bpe) for the SLS method before \n ' \
'and after applying Huffman coding (Sample Image Data)')
# plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
plt.subplots_adjust(left=0.1, right=0.95, top=0.9, bottom=0.07)
plt.show()