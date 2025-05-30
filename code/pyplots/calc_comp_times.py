import pandas as pd

# Load and combine CSVs
csv_files = [
    # 'code/output_files/mc_results_test_screenshots_new.csv',
    # 'code/output_files/mc_results_test_icons_new.csv'
    'code/output_files/mc_results_test_textures_new.csv'
    # 'code/output_files/mc_results_test_photos_new.csv'
]
data_frames = [pd.read_csv(file) for file in csv_files]
df = pd.concat(data_frames, ignore_index=True)

# Compute full computation times per method
df["cmv_comp_time"] = (
    df["read_img_time"] +
    df["region_color_dfs_time"] +
    df["dpcm_huffman_time"] +
    df["dpcm_huffman_bitstring_time"]
)

df["rcmv_comp_time"] = (
    df["cmv_comp_time"] +
    df["rcmv_construction_time"] +
    df["rcmv_bitstring_time"]
)

df["tree_comp_time"] = (
    df["cmv_comp_time"] +
    df["tree_construction_time"] +
    df["tree_bitstring_time"]
)

df["dec_comp_time"] = (
    df["cmv_comp_time"] +
    df["dec_construction_time"] +
    df["dec_bitstring_time"]
)

df["sls_comp_time"] = (
    df["cmv_comp_time"] +
    df["sls_construction_time"] +
    df["sls_bitstring_time"]
)

df["cmv_decomp_time"] = (
    df["decode_colors_time"] +
    df["cmv_reconstruction_time"] +
    df["dfs_reconstruction_time"] 
)

df["rcmv_decomp_time"] = (
    df["decode_colors_time"] +
    df["reconstruct_rcmv_time"] +
    df["reconstruct_rcmv_cmv_time"] +
    df["dfs_reconstruction_time"]
)

df["dt_decomp_time"] = (
    df["decode_colors_time"] +
    df["assemble_tree_paths_time"] +
    df["reconstruct_tree_edgebits_time"] +
    df["dfs_reconstruction_time"]
)

df["dec_decomp_time"] = (
    df["decode_colors_time"] +
    df["dec_reconstruction_time"] +
    df["dec_cmv_reconstruction_time"] +
    df["dfs_reconstruction_time"]
)


df["sls_decomp_time"] = (
    df["decode_colors_time"] +
    df["sls_reconstruction_time"] +
    df["sls_cmv_reconstruction_time"] +
    df["dfs_reconstruction_time"]
)

# Calculate megapixels per second for each compression time (convert ms to seconds)
df["cmv_mpx_per_s"] = df["pixels"] / 1e6 / (df["cmv_comp_time"] / 1000)
df["rcmv_mpx_per_s"] = df["pixels"] / 1e6 / (df["rcmv_comp_time"] / 1000)
df["tree_mpx_per_s"] = df["pixels"] / 1e6 / (df["tree_comp_time"] / 1000)
df["dec_mpx_per_s"] = df["pixels"] / 1e6 / (df["dec_comp_time"] / 1000)
df["sls_mpx_per_s"] = df["pixels"] / 1e6 / (df["sls_comp_time"] / 1000)
print("\n")

print("\nAverage Megapixels per Second (Compression):")
print("CMV:", df["cmv_mpx_per_s"].mean())
print("RCMV:", df["rcmv_mpx_per_s"].mean())
print("Tree:", df["tree_mpx_per_s"].mean())
print("DEC:", df["dec_mpx_per_s"].mean())
print("SLS:", df["sls_mpx_per_s"].mean())
print("\n")

# Print total average times
print("Average Computation Times:")
print("CMV:", df["cmv_comp_time"].mean())
print("RCMV:", df["rcmv_comp_time"].mean())
print("Tree:", df["tree_comp_time"].mean())
print("DEC:", df["dec_comp_time"].mean())
print("SLS:", df["sls_comp_time"].mean())

print("\nAverage Decompression Times:")
print("CMV:", df["cmv_decomp_time"].mean())
print("RCMV:", df["rcmv_decomp_time"].mean())
print("Tree:", df["dt_decomp_time"].mean())
print("DEC:", df["dec_decomp_time"].mean())
print("SLS:", df["sls_decomp_time"].mean())

# Print composition breakdown (means of each contributing component)
# print("\n--- CMV Composition ---")
# print("Read Image:", df["read_img_time"].mean())
# print("Region Coloring (DFS):", df["region_color_dfs_time"].mean())
# print("DPCM + Huffman:", df["dpcm_huffman_time"].mean())
# print("Bitstring Creation:", df["dpcm_huffman_bitstring_time"].mean())

# print("\n--- Tree Composition ---")
# print("Tree Construction:", df["tree_construction_time"].mean())
# print("Tree Bitstring:", df["tree_bitstring_time"].mean())

# print("\n--- DEC Composition ---")
# print("DEC Construction:", df["dec_construction_time"].mean())
# print("DEC Bitstring:", df["dec_bitstring_time"].mean())

# print("\n--- SLS Composition ---")
# print("SLS Construction:", df["sls_construction_time"].mean())
# print("SLS Bitstring:", df["sls_bitstring_time"].mean())
