import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Replace with your actual CSV path
csv_path = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test.csv"

# Read CSV
df = pd.read_csv(csv_path)

# Filter out any missing or invalid entries (optional, but safe)
uf_times = df['region_color_UF_time'].dropna()
dfs_times = df['region_color_dfs_time'].dropna()

# Compute averages
avg_uf = uf_times.mean()
avg_dfs = dfs_times.mean()

# Bar plot
methods = ['Union-Find', 'DFS']
averages = [avg_uf, avg_dfs]

plt.figure(figsize=(6, 4))
bars = plt.bar(methods, averages, color=['skyblue', 'lightcoral'])

# Add value labels on top of bars
for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2.0, yval, f'{yval:.2f} ms',
             ha='center', va='bottom', fontsize=10)

plt.ylabel('Average Time (ms)')
plt.title('Average Region Coloring Time: Union-Find vs DFS')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.tight_layout()
plt.show()
