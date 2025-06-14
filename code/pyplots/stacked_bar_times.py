import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

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

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv',
    'code/output_files/mc_results_test_textures_final.csv',
    'code/output_files/mc_results_test_photos_final.csv',
    'code/output_files/mc_results_test_icons_final.csv'
    # 'code/output_files/mc_results_test_sample.csv'
]

data_frames = [pd.read_csv(file) for file in csv_files]
categories = ["screenshots", "textures", "photos", "icons"]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)

# Compute full compression and decompression times (unchanged)
df["cmv_comp_time"] = df["region_color_dfs_time"]
df["colors_comp_time"] = df["dpcm_huffman_time"] + df["dpcm_huffman_bitstring_time"]
# To use a DataFrame that's all zeros, you can create it like this:
zero_df = pd.DataFrame(0, index=np.arange(len(df)), columns=df.columns)

# Example: replace df with zero_df for testing
# df = zero_df

df["rcmv_comp_time"] = df["cmv_comp_time"] + df["deflate_edgebits_time"] + 0 + df["write_time"]
df["tree_comp_time"] = df["cmv_comp_time"] + df["tree_construction_time"] + df["tree_bitstring_time"] + df["write_time"]
df["dec_comp_time"] = df["cmv_comp_time"] + df["dec_construction_time"] + df["dec_bitstring_time"] + df["write_time"]
df["sls_comp_time"] = df["cmv_comp_time"] + df["sls_construction_time"] + df["sls_bitstring_time"] + df["write_time"]

df["cmv_decomp_time"] = df["decode_colors_time"] + df["cmv_reconstruction_time"] + df["dfs_reconstruction_time"]
df["rcmv_decomp_time"] = df["read_time"] + df["decode_colors_time"] + df["inflate_edgebits_time"] + 0 + df["dfs_reconstruction_time"]
df["dt_decomp_time"] = df["read_time"] + df["decode_colors_time"] + df["assemble_tree_paths_time"] + df["reconstruct_tree_edgebits_time"] + df["dfs_reconstruction_time"]
df["dec_decomp_time"] = df["read_time"] + df["decode_colors_time"] + df["dec_reconstruction_time"] + df["dec_cmv_reconstruction_time"] + df["dfs_reconstruction_time"]
df["sls_decomp_time"] = df["read_time"] + df["decode_colors_time"] + df["sls_reconstruction_time"] + df["sls_cmv_reconstruction_time"] + df["dfs_reconstruction_time"]

methods = ["rcmv", "tree", "dec", "sls"]

# Define parts for stacked bars
comp_parts = ["region_color_dfs_time", "dpcm_huffman_time", "dpcm_huffman_bitstring_time"]
comp_method_parts = {
    "rcmv": ["inflate_edgebits_time", "rcmv_bitstring_time", "write_time"],
    "tree": ["tree_construction_time", "tree_bitstring_time", "write_time"],
    "dec": ["dec_construction_time", "dec_bitstring_time", "write_time"],
    "sls": ["sls_construction_time", "sls_bitstring_time", "write_time"]
}
decomp_method_parts = {
    "rcmv": ["read_time", "decode_colors_time", "inflate_edgebits_time", 0, "dfs_reconstruction_time"],
    "tree": ["read_time", "decode_colors_time", "assemble_tree_paths_time", "reconstruct_tree_edgebits_time", "dfs_reconstruction_time"],
    "dec": ["read_time", "decode_colors_time", "dec_reconstruction_time", "dec_cmv_reconstruction_time", "dfs_reconstruction_time"],
    "sls": ["read_time", "decode_colors_time", "sls_reconstruction_time", "sls_cmv_reconstruction_time", "dfs_reconstruction_time"]
}

# Calculate averages per method
comp_times = {
    method: [df[p].mean() for p in comp_parts + comp_method_parts[method]]
    for method in methods
}
decomp_times = {
    method: [
        df[p].mean() if isinstance(p, str) else 0.0
        for p in decomp_method_parts[method]
    ]
    for method in methods
}


# Plotting setup
x = np.arange(len(methods))
bar_width = 0.35

colors_comp = ['#FF9999', '#FF6666', '#FF3333', '#66B2FF', '#3366FF', '#9999FF']  # 6 comp parts
colors_decomp = ['#CCFFCC', '#99FF99', '#66CC66', '#339933', '#006600']           # 5 decomp parts

fig, ax = plt.subplots(figsize=(12, 6))

# Compression bars (left)
bottom = np.zeros(len(methods))
for i in range(len(comp_times["rcmv"])):
    values = [comp_times[m][i] for m in methods]
    ax.bar(x - bar_width / 2, values, bar_width, bottom=bottom, color=colors_comp[i % len(colors_comp)])
    bottom += values

# Decompression bars (right)
bottom = np.zeros(len(methods))
for i in range(len(decomp_times["rcmv"])):
    values = [decomp_times[m][i] for m in methods]
    ax.bar(x + bar_width / 2, values, bar_width, bottom=bottom, color=colors_decomp[i % len(colors_decomp)])
    bottom += values

# current_ylim = ax.get_ylim()
# ax.set_ylim(current_ylim[0], current_ylim[1] * 1.1)

# Labels and formatting
ax.set_xticks(x)
ax.set_xticklabels(["rCMV", "DT", "DEC", "SLS"])
ax.set_ylabel("Average Time (ms)")
ax.set_title("Compression and Decompression Time per Method")
ax.grid(axis='y', linestyle='--', alpha=0.7)
# Legend: below the plot, centered, in 5 columns
ax.legend([
    "Color Region Discovery", "DPCM + Deflate", "Color Bitstring",
    "Multicut Construction", "Multicut Bitstring", "Write to File",
    "Read from File", "Color Decoding", "Multicut Structure Reconstruction",
    "Edge Bit Reconstruction", "Image DFS"
], loc='lower center', bbox_to_anchor=(0.5, -0.3), ncol=4, fontsize="small")

# Reserve vertical space for the legend
plt.tight_layout(rect=[0, 0.2, 1, 0])


plt.tight_layout()
plt.show()
