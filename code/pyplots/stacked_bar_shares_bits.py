import pandas as pd
import matplotlib.pyplot as plt

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 15,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv',
    'code/output_files/mc_results_test_textures_final.csv',
    'code/output_files/mc_results_test_photos_final.csv',
    'code/output_files/mc_results_test_icons_final.csv'
]

data_frames = [pd.read_csv(file) for file in csv_files]
categories = ["screenshots", "textures", "photos", "icons"]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)

# Group by category and sum relevant bits
grouped = df.groupby("category")[["treeMCBits", "deflate_bits"]].sum()

# Calculate total bits per category
grouped["total_bits"] = grouped["treeMCBits"] + grouped["deflate_bits"]

# Calculate percentages
grouped["treeMCBits_percent"] = grouped["treeMCBits"] / grouped["total_bits"] * 100
grouped["deflate_bits_percent"] = grouped["deflate_bits"] / grouped["total_bits"] * 100

# Reorder categories
grouped = grouped.reindex(["screenshots", "icons", "photos", "textures"])

# Plot
bar_width = 0.6
indices = range(len(grouped))

fig, ax = plt.subplots(figsize=(8, 6))
ax.bar(
    indices,
    grouped["treeMCBits_percent"],
    bar_width,
    label="DT Bits (\%)",
    color="#007EE3"
)

#00335D
ax.bar(
    indices,
    grouped["deflate_bits_percent"],
    bar_width,
    bottom=grouped["treeMCBits_percent"],
    label="Region Color Bits (Deflate) (\%)",
    color="#007DE376"
)

ax.set_xticks(indices)
ax.set_xticklabels(grouped.index)
ax.set_xlabel("Category")
ax.set_ylabel("Percent (\%)")
ax.set_title("Stacked Bars: Shares of DT bits and region color bits per Category")
ax.legend(loc='upper center', ncol=2)
plt.tight_layout()
plt.show()
