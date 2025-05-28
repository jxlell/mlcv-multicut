import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load CSV (update this path to your actual file location)
csv_file = 'code/output_files/mc_results_test_icons_new.csv'
df = pd.read_csv(csv_file)

# Compute full compression and decompression times
df["cmv_comp_time"] = df["region_color_dfs_time"] 
df["colors_comp_time"] = df["dpcm_huffman_time"] + df["dpcm_huffman_bitstring_time"]
df["rcmv_comp_time"] = df["cmv_comp_time"] + df["rcmv_construction_time"] + df["rcmv_bitstring_time"]
df["tree_comp_time"] = df["cmv_comp_time"] + df["tree_construction_time"] + df["tree_bitstring_time"]
df["dec_comp_time"] = df["cmv_comp_time"] + df["dec_construction_time"] + df["dec_bitstring_time"]
df["sls_comp_time"] = df["cmv_comp_time"] + df["sls_construction_time"] + df["sls_bitstring_time"]

df["cmv_decomp_time"] = df["decode_colors_time"] + df["cmv_reconstruction_time"] + df["dfs_reconstruction_time"]
df["rcmv_decomp_time"] = df["decode_colors_time"] + df["reconstruct_rcmv_time"] + df["reconstruct_rcmv_cmv_time"] + df["dfs_reconstruction_time"]
df["dt_decomp_time"] = df["decode_colors_time"] + df["assemble_tree_paths_time"] + df["reconstruct_tree_edgebits_time"] + df["dfs_reconstruction_time"]
df["dec_decomp_time"] = df["decode_colors_time"] + df["dec_reconstruction_time"] + df["dec_cmv_reconstruction_time"] + df["dfs_reconstruction_time"]
df["sls_decomp_time"] = df["decode_colors_time"] + df["sls_reconstruction_time"] + df["sls_cmv_reconstruction_time"] + df["dfs_reconstruction_time"]

methods = ["rcmv", "tree", "dec", "sls"]
comp_parts = ["read_img_time", "region_color_dfs_time", "dpcm_huffman_time", "dpcm_huffman_bitstring_time"]
comp_method_parts = {
    "rcmv": ["rcmv_construction_time", "rcmv_bitstring_time"],
    "tree": ["tree_construction_time", "tree_bitstring_time"],
    "dec": ["dec_construction_time", "dec_bitstring_time"],
    "sls": ["sls_construction_time", "sls_bitstring_time"]
}
decomp_method_parts = {
    "rcmv": ["decode_colors_time", "reconstruct_rcmv_time", "reconstruct_rcmv_cmv_time", "dfs_reconstruction_time"],
    "tree": ["decode_colors_time", "assemble_tree_paths_time", "reconstruct_tree_edgebits_time", "dfs_reconstruction_time"],
    "dec": ["decode_colors_time", "dec_reconstruction_time", "dec_cmv_reconstruction_time", "dfs_reconstruction_time"],
    "sls": ["decode_colors_time", "sls_reconstruction_time", "sls_cmv_reconstruction_time", "dfs_reconstruction_time"]
}

# Calculate averages
comp_times = {method: [df[p].mean() for p in comp_parts + comp_method_parts[method]] for method in methods}
decomp_times = {method: [df[p].mean() for p in decomp_method_parts[method]] for method in methods}

# Plotting
x = np.arange(len(methods))
bar_width = 0.35
colors_comp = ['#FFCCCC', '#FF9999', '#FF6666', '#FF3333', '#66B2FF', '#3366FF']
colors_decomp = ['#CCFFCC', '#99FF99', '#66CC66', '#339933']

fig, ax = plt.subplots(figsize=(12, 6))

# Compression bars
bottom = np.zeros(len(methods))
for i in range(len(comp_parts + ["construct", "bitstring"])):
    values = [comp_times[m][i] for m in methods]
    ax.bar(x - bar_width / 2, values, bar_width, bottom=bottom, color=colors_comp[i % len(colors_comp)])
    bottom += values

# Decompression bars
bottom = np.zeros(len(methods))
for i in range(len(decomp_method_parts["rcmv"])):
    values = [decomp_times[m][i] for m in methods]
    ax.bar(x + bar_width / 2, values, bar_width, bottom=bottom, color=colors_decomp[i % len(colors_decomp)])
    bottom += values

ax.set_xticks(x)
ax.set_xticklabels(methods)
ax.set_ylabel("Average Time (ms)")
ax.set_title("Compression and Decompression Time per Method")
ax.legend(["Multicut Edge Bits Construction", "Creating Encoding Structure", "3", "4", "Construct", "Bitstring", "Decomp Step 1", "2", "3", "4"], loc="upper right", fontsize="small")
plt.tight_layout()
plt.show()
