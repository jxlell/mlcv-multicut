import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({
    'text.usetex': True,
    "font.family": "serif",
    "font.size": 15,
    "pgf.texsystem": "pdflatex",
    "pgf.rcfonts": False,
})
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv',
    'code/output_files/mc_results_test_textures_final.csv',
    'code/output_files/mc_results_test_photos_final.csv',
    'code/output_files/mc_results_test_icons_final.csv'
]
categories = ["screenshots", "textures", "photos", "icons"]
data_frames = [pd.read_csv(file) for file in csv_files]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)

# Define compression and decompression parts
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
methods = ["rcmv", "tree", "dec", "sls"]

# Compute averages and normalize to percentages
comp_times = {
    method: [df[p].mean() for p in comp_parts + comp_method_parts[method]]
    for method in methods
}
comp_perc = {
    method: (np.array(values) / np.sum(values)) * 100
    for method, values in comp_times.items()
}
decomp_times = {
    method: [df[p].mean() if isinstance(p, str) else 0.0 for p in decomp_method_parts[method]]
    for method in methods
}
decomp_perc = {
    method: (np.array(values) / np.sum(values)) * 100
    for method, values in decomp_times.items()
}

# Plotting
x = np.arange(len(methods))
bar_width = 0.35
colors_comp = ['#FF9999', '#FF6666', '#FF3333', '#66B2FF', '#3366FF', '#9999FF']
colors_decomp = ['#CCFFCC', '#99FF99', '#66CC66', '#339933', '#006600']

fig, ax = plt.subplots(figsize=(12, 6))

# Compression bars (left side)
bottom = np.zeros(len(methods))
for i in range(len(next(iter(comp_perc.values())))):
    values = [comp_perc[m][i] for m in methods]
    ax.bar(x - bar_width / 2, values, bar_width, bottom=bottom, color=colors_comp[i % len(colors_comp)])
    bottom += values

# Decompression bars (right side)
bottom = np.zeros(len(methods))
for i in range(len(next(iter(decomp_perc.values())))):
    values = [decomp_perc[m][i] for m in methods]
    ax.bar(x + bar_width / 2, values, bar_width, bottom=bottom, color=colors_decomp[i % len(colors_decomp)])
    bottom += values

# Labels and formatting
ax.set_xticks(x)
ax.set_xticklabels(["rCMV", "DT", "DEC", "SLS"])
ax.set_ylabel("Percentage of Total Time (\%)")
ax.set_title("Relative Contribution of Compression/Decompression Steps")
ax.grid(axis='y', linestyle='--', alpha=0.7)

ax.legend([
    "Color Region Discovery", "Differential + Deflate", "Color Bitstring",
    "Multicut Construction", "Bitstring Construction", "Write to File",
    "Read from File", "Color Decoding", "Multicut Structure Reconstruction",
    "Edge Bit Reconstruction", "Image Reconstruction"
], loc='lower center', bbox_to_anchor=(0.5, -0.3), ncol=4, fontsize="small")

# plt.tight_layout(rect=[0, 0.2, 0.5, 0])
plt.subplots_adjust(bottom=0.22)
plt.show()
