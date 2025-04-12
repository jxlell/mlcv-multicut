import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_extended.csv")

# Calculate crossings as a percentage of pixels
df["crossings_pct"] = df["crossings"] / df["pixels"] * 100

# Calculate crossings per disconnected component
df["crossings_per_disc_comp"] = df["crossings"] / df["disc_comp"]

# Sort by crossings_per_disc_comp
df_sorted = df.sort_values(by="crossings_pct")


# Plot
fig, ax1 = plt.subplots(figsize=(12, 6))

# Left Y-axis: compression rates
# ax1.plot(df_sorted["filename"], df_sorted["tree_rate"], label="tree_rate", color="tab:blue", marker='o')
# ax1.plot(df_sorted["filename"], df_sorted["2bit_rate"], label="2bit_rate", color="tab:green", marker='x')
# ax1.set_ylabel("Compression Rate")

# Calculate compression rate difference
df_sorted["rate_diff"] = df_sorted["tree_rate"] - df_sorted["2bit_rate"]

# Left Y-axis: difference of compression rates
ax1.plot(df_sorted["filename"], df_sorted["rate_diff"], label="tree_rate - 2bit_rate", color="tab:blue", marker='o')
ax1.set_ylabel("Compression Rate Difference")

# Plot horizontal line at height 0 for the difference
ax1.axhline(0, color="red", linestyle="--", linewidth=1, label="Zero Line")



# ax1.set_xticks(range(len(df_sorted)))
# ax1.set_xticklabels(df_sorted["filename"], rotation=45, ha='right')
ax1.legend(loc="upper left")

# Right Y-axis: crossings as % of pixels
ax2 = ax1.twinx()
ax2.plot(df_sorted["filename"], df_sorted["crossings_pct"], label="crossings (% of pixels)", color="tab:red", linestyle="--", marker='^')
ax2.set_ylabel("Crossings (% of Pixels)", color="tab:red")

# ax2 = ax1.twinx()
# ax2.plot(df_sorted["filename"], df_sorted["crossings_per_disc_comp"], label="crossings / disc_comp", color="tab:red", linestyle="--", marker='^')
# ax2.set_ylabel("Crossings per Disconnected Component", color="tab:red")
# ax2.tick_params(axis='y', labelcolor="tab:red")


# Title and layout
plt.title("Compression Rates and Relative Crossings")
plt.xticks([])
plt.tight_layout()
plt.grid(True)
plt.yscale('log')
plt.show()
