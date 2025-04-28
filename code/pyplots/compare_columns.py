import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_reduced2.csv")

# Define columns
columns = ["dpcm-huffman_bits"
           ,"deflate_bits" 
           ,"deflate_unseparated"
        #    ,"region_color_bits"
           ]

# columns = ["transfer_codes", "transfer_map"]

# Calculate averages
averages = [df[col].mean() for col in columns]

# Plot average bars
plt.bar(columns, averages, color=["skyblue", "lightgreen", "salmon"])

# Labels and formatting
plt.ylabel("Average Bit Count")
plt.title("Average Bit Count Comparison")
plt.tight_layout()
plt.show()
