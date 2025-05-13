import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
csv_path = 'code/output_files/mc_results_test_screenshots.csv'
df = pd.read_csv(csv_path)

# Compute average values
tree_path_avg = df['tree_path_bits'].mean()
tree_start_avg = df['tree_start_bits'].mean()
paths_start_avg = df['paths2bit_start_bits'].mean()
paths_dir_avg = df['paths2bit_direction_bits'].mean()

# Bar positions and width
labels = ['Tree Encoding', '2-Bit Encoding']
x = [0, 1]
width = 0.5

# Plot
fig, ax = plt.subplots(figsize=(6, 6))

# Stacked bar 1 (Tree)
bar1 = ax.bar(x[0], tree_path_avg, width, label='Path Bits (Tree)', color='#1f77b4')
bar2 = ax.bar(x[0], tree_start_avg, width, bottom=tree_path_avg, label='Start Bits (Tree)', color='#aec7e8')

# Stacked bar 2 (2-Bit)
bar3 = ax.bar(x[1], paths_start_avg, width, bottom=paths_dir_avg, label='Start Bits (2-Bit)', color='#ff7f0e')
bar4 = ax.bar(x[1], paths_dir_avg, width, label='Direction Bits (2-Bit)', color='#ffbb78')

# Labels and formatting
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.set_ylabel('Average Bits')
ax.set_title('Average Bit Usage: Tree vs 2-Bit Encoding')
ax.legend()

plt.tight_layout()
plt.show()
