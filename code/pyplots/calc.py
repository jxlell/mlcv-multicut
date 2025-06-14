import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Replace 'your_file.csv' with your CSV file path
csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'

csv_files = [
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'
]

df = pd.concat([pd.read_csv(file) for file in csv_files], ignore_index=True)

df['3bit_bpe'] = df['3bit_paths_bits'] / df['edgebits']
df['dt_bpe'] = (df['tree_path_bits'] + df['tree_start_bits']) / df['edgebits']

means = df[['3bit_bpe', 'dt_bpe']].mean()
print("Means:\n", means)
reduction_rate = ((means['3bit_bpe'] - means['dt_bpe']) / means['3bit_bpe']) * 100
print("Reduction rate:", reduction_rate)

df['straights_bpe'] = df['straights_mc_bits'] / df['edgebits']
df['sls_bpe'] = df['sls_mc_bits'] / df['edgebits']
means_sls = df[['straights_bpe', 'sls_bpe']].mean()
print("SLS Means:\n", means_sls)
reduction_rate_sls = ((means_sls['straights_bpe'] - means_sls['sls_bpe']) / means_sls['straights_bpe']) * 100
print("SLS Reduction rate:", reduction_rate_sls)