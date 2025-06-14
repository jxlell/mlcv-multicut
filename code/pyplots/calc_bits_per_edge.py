import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load and combine CSVs
csv_files = [
    'code/output_files/mc_results_test_screenshots_final.csv',
    # 'code/output_files/mc_results_test_textures_final.csv',
    # 'code/output_files/mc_results_test_photos_final.csv',
    # 'code/output_files/mc_results_test_icons_final.csv'
]


df = pd.concat([pd.read_csv(file) for file in csv_files], ignore_index=True)

df['dt_bpe'] = df['treeMCBits'] / df['edgebits']
df['dec_bpe'] = df['rle'] / df['edgebits']
