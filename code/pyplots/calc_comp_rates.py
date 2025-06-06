import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv',
    'code/output_files/mc_results_test_textures_final.csv',
    'code/output_files/mc_results_test_photos_final.csv',
    'code/output_files/mc_results_test_icons_final.csv'
]

single_csv = 'code/output_files/mc_results_test_screenshots_final.csv'
df_single = pd.read_csv(single_csv)
mean_tree_rate = df_single["tree_rate"].mean()
print(f"Average tree_rate for screenshots: {mean_tree_rate}")

data_frames = [pd.read_csv(file) for file in csv_files]


categories = ["screenshots", "textures", "photos", "icons"]
for df_part, category in zip(data_frames, categories):
    df_part["category"] = category
df = pd.concat(data_frames, ignore_index=True)

for file, category in zip(csv_files, categories):
    df_file = pd.read_csv(file)
    avg_dt_rate = df_file["tree_rate"].mean()
    avg_dec_rate = df_file["rle_rate"].mean()
    avg_sls_rate = df_file["straights_huffman_rate"].mean()
    avg_rcmv_rate = df_file["red_edgebits_rate"].mean()
    med_dt_rate = df_file["tree_rate"].median()
    med_dec_rate = df_file["rle_rate"].median()
    med_sls_rate = df_file["straights_huffman_rate"].median()
    med_rcmv_rate = df_file["red_edgebits_rate"].median()
    print(f"{category} rates:")
    print(f"  tree_rate: mean={avg_dt_rate}, median={med_dt_rate}")
    print(f"  rle_rate: mean={avg_dec_rate}, median={med_dec_rate}")
    print(f"  straights_huffman_rate: mean={avg_sls_rate}, median={med_sls_rate}")
    print(f"  red_edgebits_rate: mean={avg_rcmv_rate}, median={med_rcmv_rate}\n")