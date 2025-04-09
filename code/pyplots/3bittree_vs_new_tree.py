import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("code/output_files/mc_results_test_reduced.csv")

# Plot
plt.figure(figsize=(12, 6))
bar_width = 0.25
x = range(len(df["filename"]))

# Plot bars for 3bit_paths_bits and tree_path_bits
# plt.bar(x, df["3bit_paths_bits"], width=bar_width, label="3bit_paths_bits", color='skyblue')
# plt.bar([i + bar_width for i in x], df["tree_path_bits"], width=bar_width, label="tree_path_bits", color='orange')

plt.bar(x, df["3bit_paths_bits"], width=bar_width, label="3bit paths bits", color='lightgreen')
plt.bar([i + bar_width for i in x], df["paths2bit_direction_bits"], width=bar_width, label="2bit direction bits", color='skyblue')
plt.bar([i + 2*bar_width for i in x], df["tree_path_bits"], width=bar_width, label="tree_path_bits", color='orange')


# Add title and remove x-axis labels
plt.xticks([])
plt.ylabel("Bits")
plt.title("Comparison of 3bit_paths_bits and tree_path_bits")
plt.legend()
plt.tight_layout()
plt.show()
