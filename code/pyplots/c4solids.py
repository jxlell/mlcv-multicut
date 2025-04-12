import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV data
df = pd.read_csv("code/output_files/mc_results_test.csv")

# Specify the filenames to compare
filename_1 = "C4_Solids_Demo.png"  # Replace with the first filename
filename_2 = "A_House_in_California.png"  # Replace with the second filename

# Filter the data for these specific filenames
df_filtered = df[df["filename"].isin([filename_1, filename_2])]

# Extract the relevant columns for comparison
df_comparison = df_filtered[["filename", "disc_comp", "crossings", "tree_rate", "2bit_rate"]]

# Set up the plot
fig, ax = plt.subplots(figsize=(10, 6))

# Create a grouped bar chart
df_comparison.set_index("filename").plot(kind="bar", ax=ax, color=["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728"])

# Add labels and title
ax.set_xlabel("Filename")
ax.set_ylabel("Value")
ax.set_title("Comparison of Disc Comp, Crossings, Tree Rate, and 2bit Rate for Specific Filenames")
ax.set_xticklabels([filename_1, filename_2])

# Show the plot
plt.yscale('log')  # Set y-axis to logarithmic scale
plt.tight_layout()
plt.show()
