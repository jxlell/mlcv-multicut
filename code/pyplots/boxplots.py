import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_020525.csv")

# Define columns and labels
# columns = ["3bit_paths_bits", "tree_path_bits"]
# labels = ["fixed 3-bit code word", "code word without duplicates"]


columns = ["paths2bit_direction_bits", "rle_direction_bits"]
labels = ["non-RLE DEC", "DEC applying RLE"]

# Create boxplot
data_to_plot = [df[col].dropna() for col in columns]  # remove NaNs just in case
plt.boxplot(data_to_plot, labels=labels, patch_artist=True,
            boxprops=dict(facecolor="lightblue"),
            medianprops=dict(color="black"),
            whiskerprops=dict(color="black"),
            capprops=dict(color="black"),
            flierprops=dict(markerfacecolor="lightblue", marker="o", alpha=0.5))

# Labels and formatting
plt.ylabel("Bit Count")
plt.title("Comparison of Bit Counts for DEC Approaches")
plt.tight_layout()
plt.yscale('log')  # Log scale for better visibility of differences
plt.show()

for col in columns:
    data = df[col].dropna()
    q1 = data.quantile(0.25)
    q3 = data.quantile(0.75)
    iqr = q3 - q1
    lower_bound = q1 - 1.5 * iqr
    upper_bound = q3 + 1.5 * iqr
    outliers = data[(data < lower_bound) | (data > upper_bound)]
    print(f"{col}: {len(outliers)} outliers")
