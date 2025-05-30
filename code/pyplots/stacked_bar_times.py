import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load CSV
# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_new.csv',
    # 'code/output_files/mc_results_test_textures_new.csv',
    # 'code/output_files/mc_results_test_photos_new.csv',
    'code/output_files/mc_results_test_icons_new.csv'
]

data_frames = [pd.read_csv(file) for file in csv_files]
categories = ["screenshots", "textures", "photos", "icons"]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)


# Compute full compression and decompression times (unchanged)
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

# ⛔ Exclude 'read_img_time'
comp_parts = ["region_color_dfs_time", "dpcm_huffman_time", "dpcm_huffman_bitstring_time"]

comp_method_parts = {
    "rcmv": ["rcmv_construction_time", "rcmv_bitstring_time"],
    "tree": ["tree_construction_time", "tree_bitstring_time"],
    "dec": ["dec_construction_time", "dec_bitstring_time"],
    "sls": ["sls_construction_time", "sls_bitstring_time"]
}
decomp_method_parts = {
    "rcmv": ["decode_colors_time", "reconstruct_rcmv_time", "reconstruct_rcmv_cmv_time", "dfs_reconstruction_time"],
    "tree": ["decode_colors_time", "reconstruct_tree_edgebits_time", "reconstruct_tree_edgebits_time", "dfs_reconstruction_time"],
    "dec": ["decode_colors_time", "dec_reconstruction_time", "dec_cmv_reconstruction_time", "dfs_reconstruction_time"],
    "sls": ["decode_colors_time", "sls_reconstruction_time", "sls_cmv_reconstruction_time", "dfs_reconstruction_time"]
}

# Calculate averages
comp_times = {method: [df[p].mean() for p in comp_parts + comp_method_parts[method]] for method in methods}
decomp_times = {method: [df[p].mean() for p in decomp_method_parts[method]] for method in methods}

# Plotting
x = np.arange(len(methods))
bar_width = 0.35
colors_comp = ['#FF9999', '#FF6666', '#FF3333', '#66B2FF', '#3366FF']  # 5 parts now
colors_decomp = ['#CCFFCC', '#99FF99', '#66CC66', '#339933']

fig, ax = plt.subplots(figsize=(12, 6))

# Compression bars (left bar)
bottom = np.zeros(len(methods))
for i in range(len(comp_parts + ["construct", "bitstring"])):
    values = [comp_times[m][i] for m in methods]
    ax.bar(x - bar_width / 2, values, bar_width, bottom=bottom, color=colors_comp[i % len(colors_comp)])
    bottom += values

# Decompression bars (right bar)
bottom = np.zeros(len(methods))
for i in range(len(decomp_method_parts["rcmv"])):
    values = [decomp_times[m][i] for m in methods]
    ax.bar(x + bar_width / 2, values, bar_width, bottom=bottom, color=colors_decomp[i % len(colors_decomp)])
    bottom += values

ax.set_xticks(x)
ax.set_xticklabels(["rCMV", "DT", "DEC", "SLS"])
ax.set_ylabel("Average Time (ms)")
ax.set_title("Compression and Decompression Time per Method")

# Updated legend
ax.legend([
    "Color Region Discovery + Multicut Edge Bits Construction", "DPCM + Deflate", "Color Bitstring Serialization", 
    "Multicut Structure Construction", "Multicut Encoding Bitstring Serialization",
    "Color Deoding", "Multicut Structure Reconstruction", 
    "Multicut Edge Bits Reconstruction", "Image DFS Reconstruction",
], loc="upper right", fontsize="small")

plt.tight_layout()
plt.show()
