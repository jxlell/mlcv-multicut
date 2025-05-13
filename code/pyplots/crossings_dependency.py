import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats

# Load the CSV data
df = pd.read_csv("code/output_files/mc_results_test_screenshots.csv")

# Calculate the difference between crossings and disc_comp
df['crossings_minus_disc_comp'] = df['crossings'] - df['disc_comp']

# Calculate the difference between tree_rate and 2bit_rate
df['rate_difference'] = df['tree_rate'] - df['2bit_rate']

# Set up the plot with Seaborn style
sns.set(style="whitegrid")
fig, ax = plt.subplots(figsize=(10, 6))

# Scatter plot with a regression line
sns.regplot(x='crossings', y='rate_difference', data=df,
            scatter_kws={'s': 100, 'alpha': 0.6, 'color': 'blue'},
            line_kws={'color': 'red', 'linewidth': 2}, ax=ax)

# Set labels and title
ax.set_xlabel("Crossings", fontsize=12)
ax.set_ylabel("Difference Between Tree Rate and 2bit Rate", fontsize=12)
ax.set_title("Connection Between Crossings - Disc Comp and Rate Difference", fontsize=14)

# Show the plot
plt.tight_layout()
plt.show()
