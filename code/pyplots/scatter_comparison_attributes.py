import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test.csv")


# Sort by tree_rate
df_sorted = df.sort_values(by="tree_rate").reset_index(drop=True)

# Prepare data
x = list(range(len(df_sorted)))
tree_rates = df_sorted["tree_rate"]
two_bit_rates = df_sorted["2bit_rate"]

# Determine color based on which rate is higher
colors_tree = ['red' if tr > tbr else 'lightgray' for tr, tbr in zip(tree_rates, two_bit_rates)]
colors_2bit = ['purple' if tbr > tr else 'lightgray' for tr, tbr in zip(tree_rates, two_bit_rates)]

# Plot
plt.figure(figsize=(12, 6))
plt.scatter(x, tree_rates, label="tree_rate", color=colors_tree, marker='o')
plt.scatter(x, two_bit_rates, label="2bit_rate", color=colors_2bit, marker='x')

# Labels and legend
plt.xlabel("Images sorted by tree_rate")
plt.ylabel("Compression rate (lower is better)")
plt.title("Compression Rate Comparison: Tree vs 2bit")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
