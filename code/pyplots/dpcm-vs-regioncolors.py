import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test.csv")

# Calculate the averages
avg_region_color_bits = df["region_color_bits"].mean()
avg_dpcm_huffman_bits = df["dpcm-huffman_bits"].mean()

# Plotting the comparison
labels = ["Region Color Bits", "DPCM Huffman Bits"]
values = [avg_region_color_bits, avg_dpcm_huffman_bits]

plt.figure(figsize=(8, 6))
plt.bar(labels, values, color=['#1f77b4', '#ff7f0e'])
plt.xlabel("Bit Type")
plt.ylabel("Average Bits")
plt.title("Comparison of Averaged Region Color Bits and DPCM Huffman Bits")
plt.show()
