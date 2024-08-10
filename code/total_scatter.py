import csv
import matplotlib.pyplot as plt

# Function to read CSV file and return the data as a list of dictionaries
def read_csv(file_path):
    data = []
    with open(file_path, 'r') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            data.append(row)
    return data

def filter_by_category(data, category):
    return [row for row in data if row['category'] == category and float(row['rate']) < 400]

# Paths to the two CSV files
csv_file_1 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_sorted.csv'
csv_file_2 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/parsed_qoi_results.csv'

# Read data from both CSV files
data1 = read_csv(csv_file_1)
data2 = read_csv(csv_file_2)

# Combine the data from both files
category_to_filter = 'pngimg'
filtered_data1 = filter_by_category(data1, category_to_filter)
filtered_data2 = filter_by_category(data2, category_to_filter)

# Sort the filtered data by the compression rate
sorted_data1 = sorted(filtered_data1, key=lambda x: float(x['rate']))
sorted_data2 = sorted(filtered_data2, key=lambda x: float(x['rate']))

# Extract the filenames and compression rates for plotting
filenames1 = [row['filename'] for row in sorted_data1]
compression_rates1 = [float(row['rate']) for row in sorted_data1]

filenames2 = [row['filename'] for row in sorted_data2]
compression_rates2 = [float(row['rate']) for row in sorted_data2]

# Create the scatter plot
plt.scatter(range(len(filenames1)), compression_rates1, alpha=0.5, label='Multicut Compression', s=5)
plt.scatter(range(len(filenames2)), compression_rates2, alpha=0.5, label='PNG', color='gray', s=5)
# Remove x-axis ticks and labels
plt.xticks([])
# Label the axes
plt.xlabel('Files sorted by PNG Compression Rate')
plt.ylabel('Compression Rate')
plt.title('Scatter Plot of Compression Rates, Category: ' + category_to_filter)
plt.yscale('symlog')
# Add a legend
plt.legend()

# Optionally, rotate x-axis labels for better readability
#plt.xticks(ticks=range(len(filenames)), labels=filenames, rotation=90)

# Show the plot
plt.tight_layout()
plt.show()
