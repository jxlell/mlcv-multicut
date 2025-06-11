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
csv_file1 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv'
csv_file2 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'
csv_file3 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'
csv_file4 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv'

df = pd.concat([pd.read_csv(csv_file1), pd.read_csv(csv_file2)], ignore_index=True)

# Load CSV

df['tree_stack'] = df['tree_path_bits'] + df['tree_start_bits']

column = 'treeMCBits'

df['tree_stack_bpp'] = df['tree_stack'] / df['edgebits']
df['treeMCBits_bpp'] = df[column] / df['edgebits']

plt.boxplot([df['tree_stack_bpp'], df['treeMCBits_bpp']], labels=['tree_stack', column])
plt.ylabel('Bits per pixel (bpp)')
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
plt.xticks([1, 2], ["DT without Deflate", "DT with Deflate"])
plt.title('Comparing bits per pixel (bpp) for the DT \n without and with application of the Deflate algorithm', fontsize=16)
plt.yscale('log')  # Set y-axis to logarithmic scale for better visualization
plt.subplots_adjust(left=0.15, right=0.97, top=0.89, bottom=0.1)
plt.show()