import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

plt.rcParams.update(
    {
        'text.usetex': True,
        "font.family": "serif",
        "font.size": 12,
        "pgf.texsystem": "pdflatex",
        "pgf.rcfonts": False,
    }
)
plt.rc('text', usetex=True)
plt.rc('text.latex', preamble=r'\usepackage{amssymb}\usepackage{wasysym}')


# Replace 'your_file.csv' with your CSV file path
# csv_file = '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv'

csv_files = [
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_screenshots_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_textures_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_photos_final.csv',
    '/Users/jalell/Documents/GitHub/mlcv-multicut/code/output_files/mc_results_test_icons_final.csv'
]

categories = ['screenshots', 'textures', 'photos', 'icons']
df = pd.concat([pd.read_csv(file) for file in csv_files], ignore_index=True)

df['mapped_category'] = df['category'].map({
    'screenshot_game': 'screenshots',
    'screenshot_game_reduced2': 'screenshots',
    'screenshot_web': 'screenshots',
    'textures_photo': 'textures',
    'textures_pk': 'textures',
    'textures_pk01': 'textures',
    'textures_pk02': 'textures',
    'textures_plants': 'textures',
    'photo_kodak': 'photos',
    'photo_tecnick': 'photos',
    'photo_wikipedia': 'photos',
    'icon_64': 'icons',
    'icon_512': 'icons'
})



plt.figure(figsize=(8, 6))

sns.violinplot(data=df, x='mapped_category', y='tree_rate', inner='box', scale='width')

plt.title(r'Distribution of $\mathit{sls\_mc\_bits\_bpp}$ by Category')
plt.xlabel('Image Category')
plt.ylabel(r'$\mathit{sls\_mc\_bits\_bpp}$')
plt.yscale('log')

plt.tight_layout()
plt.show()