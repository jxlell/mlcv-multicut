import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
csv_path = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_test_020525.csv"
try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    raise FileNotFoundError(f"CSV file not found at path: {csv_path}")

csv_files = [
    'code/output_files/mc_results_test_screenshots.csv',
    'code/output_files/mc_results_test_photos.csv',
    'code/output_files/mc_results_test_icons.csv',
    'code/output_files/mc_results_test_textures_pk.csv',
    'code/output_files/mc_results_test_textures_no_pk.csv'
]

data_frames = []
for file in csv_files:
    try:
        data_frames.append(pd.read_csv(file))
    except FileNotFoundError:
        print(f"Warning: File not found: {file}")

if not data_frames:
    raise ValueError("No valid CSV files found to combine.")

combined_data = pd.concat(data_frames, ignore_index=True)

# Filter out missing values and check for required columns
required_columns = ['region_color_UF_time', 'region_color_dfs_time']
for col in required_columns:
    if col not in df.columns:
        raise KeyError(f"Missing required column in CSV: {col}")

uf_times = df['region_color_UF_time'].dropna()
dfs_times = df['region_color_dfs_time'].dropna()

# Prepare data for boxplot
plot_data = [uf_times, dfs_times]
labels = ['Union-Find', 'DFS']

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
