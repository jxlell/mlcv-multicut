import pandas as pd
import matplotlib.pyplot as plt



csv_files = [
    'code/output_files/mc_results_test_screenshots_new.csv',
    'code/output_files/mc_results_test_photos_new.csv',
    'code/output_files/mc_results_test_icons_new.csv',
    'code/output_files/mc_results_test_textures_new.csv'
]

df = []
for file in csv_files:
    try:
        df.append(pd.read_csv(file))
    except FileNotFoundError:
        print(f"Warning: File not found: {file}")

if not df:
    raise ValueError("No valid CSV files found to combine.")

combined_data = pd.concat(df, ignore_index=True)

# Filter out missing values and check for required columns
required_columns = ['region_color_UF_time', 'region_color_dfs_time']
for col in required_columns:
    if col not in combined_data.columns:
        raise KeyError(f"Missing required column in CSV: {col}")

uf_times = combined_data['region_color_UF_time'].dropna()
dfs_times = combined_data['region_color_dfs_time'].dropna()

# Prepare data for boxplot
plot_data = [uf_times, dfs_times]
labels = ['Union-Find', 'DFS']

# Print average times per file
for file, data in zip(csv_files, df):
    avg_uf = data['region_color_UF_time'].dropna().mean()
    avg_dfs = data['region_color_dfs_time'].dropna().mean()
    print(f"{file}:")
    print(f"  Average Union-Find time: {avg_uf:.4f} ms")
    print(f"  Average DFS time:        {avg_dfs:.4f} ms")

# Create boxplot
plt.figure(figsize=(6, 4))
plt.boxplot(plot_data, labels=labels, patch_artist=True,
            boxprops=dict(facecolor='skyblue'),
            medianprops=dict(color='black'))

# Labels and formatting
plt.ylabel('Time (ms)')
plt.title('Region Coloring Time Distribution: Union-Find vs DFS')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.tight_layout()
plt.yscale('log')  # Log scale for better visibility of differences
plt.savefig('/Users/jalell/Documents/GitHub/thesis-multicut/img/boxplot_UFvsDFS.pdf')  # Save the plot as a PDF file
plt.show()
