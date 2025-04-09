import pandas as pd
import matplotlib.pyplot as plt

# Load your CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test.csv")

# Compute total bits and overhead
df["overhead"] = df["total_tree_bits"] - df["tree_path_bits"] - df["tree_start_bits"] - df["dpcm-huffman_bits"]
df["mc_bits"] = df["tree_path_bits"] + df["tree_start_bits"]
# Print overhead for each file name
for filename, overhead in zip(df["filename"], df["overhead"]):
    print(f"{filename}: {overhead}")

# Create stacked bar chart
labels = df["filename"].apply(lambda x: x.split(",")[0][:15])  # shorten filename for x-axis
x = range(len(df))

barwidth = 0.5

# plt.bar(x, df["tree_start_bits"], label="tree_start_bits", width=barwidth)
# plt.bar(x, df["tree_path_bits"], bottom=df["tree_start_bits"], label="tree_path_bits", width=barwidth)
# bottom = df["tree_path_bits"] + df["tree_start_bits"]
# plt.bar(x, df["overhead"], bottom=bottom, label="overhead", width=barwidth)

plt.bar(x, df["dpcm-huffman_bits"], label="dpcm-huffman_bits", width=barwidth)
plt.bar(x, df["mc_bits"], bottom=df["dpcm-huffman_bits"], label="mc_bits", width=barwidth)
plt.bar([x + barwidth for x in x], df["region_color_bits"], label="region_color_bits", width=barwidth)
# Display the numbers of regions on top of the bars
for i, (mc_bits, dpcm_bits, regions) in enumerate(zip(df["mc_bits"], df["dpcm-huffman_bits"], df["regions"])):
    total_height = mc_bits + dpcm_bits
    plt.text(i, total_height + 5, str(regions), ha='center', va='bottom', fontsize=8)

plt.xticks(x, labels, rotation=45, ha='right')
plt.xticks([])
plt.ylabel("Bits")
plt.title("Breakdown of Tree Bits")
plt.legend()
plt.tight_layout()
# plt.yscale('log')
plt.show()
