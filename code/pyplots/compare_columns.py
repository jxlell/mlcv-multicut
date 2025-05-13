import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import scipy.stats as stats

# Load CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_020525.csv")

# Define columns and labels
# columns = ["paths2bit_direction_bits", "rle_direction_bits"]
# labels = ["standard DEC approach", "RLE-based DEC approach"]

columns = ["3bit_paths_bits", "tree_path_bits"]
labels = ["fixed 3-bit code word", "code word without duplicates"]

# Sample size
n = len(df)

# Calculate means
averages = [df[col].mean() for col in columns]

# Calculate standard error
standard_errors = [df[col].std(ddof=1) / np.sqrt(n) for col in columns]

# Compute 95% confidence intervals (assuming normal distribution)
ci95 = [stats.t.ppf(0.975, df=n-1) * se for se in standard_errors]

# Plot with 95% CI error bars
plt.bar(labels, averages, yerr=ci95, capsize=5, color=["skyblue", "lightgreen"])

# Labels and formatting
plt.ylabel("Average Bit Count")
plt.title("Direction Bits for Directional Tree Approaches (95% CI)")
plt.tight_layout()
plt.show()
