import pandas as pd
import matplotlib.pyplot as plt

# Replace with your CSV file path
# csv_file1 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv'
# csv_file2 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'
# csv_file3 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'
# csv_file4 = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv'

csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_sample.csv'

df = pd.read_csv(csv_file)


df['dt_improvement_rate'] = df['tree_rate'] / df['2bit_rate']

df['crossings_per_pixel'] = df['crossings'] / df['pixels']

plt.figure(figsize=(10, 6))
plt.scatter(df['crossings_per_pixel'], df['dt_improvement_rate'], alpha=0.5, color='blue')
plt.title('Scatter Plot of DT Improvement Rate vs Crossings per Pixel')
plt.xlabel('Crossings per Pixel')
plt.ylabel('DT Improvement Factor (tree_rate / rle_rate)')
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
plt.tight_layout()
plt.show()
