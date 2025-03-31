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
    categories = df_sorted['category']
    old_rates = df_sorted['old_rate']
    path_rates = df_sorted['tree_rate']
    rle_rates = df_sorted['rle_rate']
    straights_rates = df_sorted['straights_rate']
    straights_huffman_rates = df_sorted['straights_huffman_rate']
    new_edgebits_rates = df_sorted['reduced_edgebits_rate']
    path_2bit_rates = df_sorted['paths_2bit_rate']

    # Find filenames, categories, and rates of maximum rates for each rate type
    max_old_rate_file = filenames[old_rates.idxmax()]
    max_old_rate_category = categories[old_rates.idxmax()]
    max_old_rate_value = old_rates.max()

    max_path_rate_file = filenames[path_rates.idxmax()]
    max_path_rate_category = categories[path_rates.idxmax()]
    max_path_rate_value = path_rates.max()

    max_rle_rate_file = filenames[rle_rates.idxmax()]
    max_rle_rate_category = categories[rle_rates.idxmax()]
    max_rle_rate_value = rle_rates.max()

    max_straights_rate_file = filenames[straights_rates.idxmax()]
    max_straights_rate_category = categories[straights_rates.idxmax()]
    max_straights_rate_value = straights_rates.max()

    max_straights_huffman_rate_file = filenames[straights_huffman_rates.idxmax()]
    max_straights_huffman_rate_category = categories[straights_huffman_rates.idxmax()]
    max_straights_huffman_rate_value = straights_huffman_rates.max()

    max_new_edgebits_rate_file = filenames[new_edgebits_rates.idxmax()]
    max_new_edgebits_rate_category = categories[new_edgebits_rates.idxmax()]
    max_new_edgebits_rate_value = new_edgebits_rates.max()

    max_path_2bit_rate_file = filenames[path_2bit_rates.idxmax()]
    max_path_2bit_rate_category = categories[path_2bit_rates.idxmax()]
    max_path_2bit_rate_value = path_2bit_rates.max()

    # Print filenames, categories, and rates of maximum rates
    print(f"Filename with max edge bits rate: {max_old_rate_file}, Category: {max_old_rate_category}, Rate: {max_old_rate_value}")
    print(f"Filename with max reduced edgebits rate: {max_new_edgebits_rate_file}, Category: {max_new_edgebits_rate_category}, Rate: {max_new_edgebits_rate_value}")
    print(f"Filename with max tree rate: {max_path_rate_file}, Category: {max_path_rate_category}, Rate: {max_path_rate_value}")
    print(f"Filename with max 2-bit paths rate: {max_path_2bit_rate_file}, Category: {max_path_2bit_rate_category}, Rate: {max_path_2bit_rate_value}")
    print(f"Filename with max RLE rate: {max_rle_rate_file}, Category: {max_rle_rate_category}, Rate: {max_rle_rate_value}")
    print(f"Filename with max straights rate: {max_straights_rate_file}, Category: {max_straights_rate_category}, Rate: {max_straights_rate_value}")
    print(f"Filename with max straights+huffman rate: {max_straights_huffman_rate_file}, Category: {max_straights_huffman_rate_category}, Rate: {max_straights_huffman_rate_value}")

    size = 18
    opacity = 0.8

    plt.figure(figsize=(12, 6))
    plt.scatter(filenames, old_rates, label='edge bits', marker='x', alpha=opacity, s=size)
    plt.scatter(filenames, new_edgebits_rates, label='reduced edge bits', marker='x', alpha=opacity, s=size)
    plt.scatter(filenames, path_rates, label='tree', marker='x', alpha=opacity, s=size)
    plt.scatter(filenames, path_2bit_rates, label='2-bit paths', marker='x', alpha=opacity, s=size)
    plt.scatter(filenames, rle_rates, label='2-bit paths + RLE', marker='x', alpha=opacity, s=size)
    plt.scatter(filenames, straights_rates, label='Straights', marker='x', alpha=opacity, s=size)
    plt.scatter(filenames, straights_huffman_rates, label='Straights + Huffman', marker='x', alpha=opacity, s=size)
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