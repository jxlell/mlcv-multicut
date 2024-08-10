import csv

# Input and output file paths
input_file_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results.csv'
output_file_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_sorted.csv'

# Read the CSV file and sort the rows by filename
with open(input_file_path, 'r') as input_file:
    reader = csv.reader(input_file)
    header = next(reader)  # Read the header
    sorted_rows = sorted(reader, key=lambda row: row[0])  # Sort rows by the first column (filename)

# Write the sorted rows to a new CSV file
with open(output_file_path, 'w', newline='') as output_file:
    writer = csv.writer(output_file)
    writer.writerow(header)  # Write the header
    writer.writerows(sorted_rows)  # Write the sorted rows

print(f"Sorted data has been written to {output_file_path}")
