import pandas as pd
import matplotlib.pyplot as plt

def plot_average_compression_rates(csv_file):
    """
    Plots the average compression rate for each category in a bar chart.

    Args:
        csv_file (str): Path to the CSV file.
    """

    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: CSV file '{csv_file}' not found.")
        return
    except Exception as e:
        print(f"An error occurred while reading the CSV file: {e}")
        return

    # Calculate average compression rates for each category
    average_rates = df.groupby('category')[['old_rate', 'rle_rate', 'path_rate', 'straights_rate', 'straights_huffman_rate']].mean()

    # Create the bar chart
    average_rates.plot(kind='bar', figsize=(10, 6))

    plt.xlabel('Category')
    plt.ylabel('Average Compression Rate')
    plt.title('Average Compression Rates by Category')
    plt.yscale('log')
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.grid(axis='y') # only gridlines on y axis
    plt.show()

# Example usage:
csv_file_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results.csv'
plot_average_compression_rates(csv_file_path)