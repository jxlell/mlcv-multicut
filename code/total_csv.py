import pandas as pd
import matplotlib.pyplot as plt

def plot_compression_rates(csv_file, category_filter=None):
    """
    Plots compression rates from a CSV file, optionally filtered by category,
    sorted by old compression rate, showing only dots (scatter plot).

    Args:
        csv_file (str): Path to the CSV file.
        category_filter (str, optional): Category to filter by. Defaults to None (no filter).
    """

    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: CSV file '{csv_file}' not found.")
        return
    except Exception as e:
        print(f"An error occurred while reading the CSV file: {e}")
        return

    # Filter by category if specified
    if category_filter:
        df = df[df['category'] == category_filter]

    if df.empty:
      print(f"No data found for category: {category_filter}")
      return

    df_sorted = df.sort_values(by='old_rate')  # Sort by 'old_rate'

    filenames = df_sorted['filename']
    old_rates = df_sorted['old_rate']
    path_rates = df_sorted['path_rate']
    rle_rates = df_sorted['rle_rate']
    straights_rates = df_sorted['straights_rate']
    straights_huffman_rates = df_sorted['straights_huffman_rate']

    plt.figure(figsize=(12, 6))
    plt.scatter(filenames, old_rates, label='Old Rate', marker='x', alpha=1, s=3) #scatter
    #plt.plot(filenames, old_rates, label='Old Rate', marker='o') #line plot
    plt.scatter(filenames, path_rates, label='Path Rate', marker='o', s=3)
    plt.scatter(filenames, rle_rates, label='RLE Rate', marker='o', s=3)
    plt.scatter(filenames, straights_rates, label='Straights Rate', marker='x', alpha=1, s=3) #scatter
    plt.scatter(filenames, straights_huffman_rates, label='Straights Huffman Rate', marker='x', alpha=1, s=3)
    plt.xticks([])
    plt.yscale('log')

    plt.xlabel('Filename (Sorted by Old Rate)')
    plt.ylabel('Compression Rate')
    plt.title(f'Compression Rates by Old Rate ({category_filter if category_filter else "All Categories"})')
    plt.tight_layout()
    plt.legend()
    plt.grid(True)
    plt.show()

# Example usage:
csv_file_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results.csv'
category_to_show = 'screenshot_game'
plot_compression_rates(csv_file_path, category_to_show)