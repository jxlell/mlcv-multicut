import csv
import re

# Define the input and output file paths
input_file_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/qoi.txt'
output_file_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/parsed_qoi_results.csv'

# Regular expression to match the lines with image paths and compression data
image_path_regex = re.compile(r'^images\/([a-z_]+)\/(.+\.png) (\d+x\d+)$')
compression_data_regex = re.compile(r'^libpng:\s+(\d+\.\d+)\s+(\d+\.\d+)\s+\d+\.\d+\s+\d+\.\d+\s+(\d+)\s+(\d+\.\d+)%$')

# Open the input file and read all lines
with open(input_file_path, 'r') as input_file:
    lines = input_file.readlines()

# Prepare a list to store the extracted data
extracted_data = []

# Iterate through the lines and extract the required information
for i in range(len(lines)):
    image_match = image_path_regex.match(lines[i].strip())
    if image_match:
        category = image_match.group(1)
        filename = image_match.group(2)
        dimensions = image_match.group(3)  # Extract the dimensions

        # Look for the 'libpng' compression data line within the next few lines
        for j in range(i + 1, i + 10):  # Assuming the libpng line is within the next 10 lines
            if j >= len(lines):
                break
            compression_match = compression_data_regex.match(lines[j].strip())
            if compression_match:
                decode_ms = compression_match.group(1)
                encode_ms = compression_match.group(2)
                size_kb = compression_match.group(3)
                rate = compression_match.group(4)
                extracted_data.append([filename, category, dimensions, encode_ms, decode_ms, size_kb, rate])
                break

# Write the extracted data to the CSV file
with open(output_file_path, 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)
    # Write the header row
    csv_writer.writerow(['filename', 'category', 'dimensions', 'encode_ms', 'decode_ms', 'size_kb', 'rate'])
    # Write the data rows
    csv_writer.writerows(extracted_data)

print(f"Data has been successfully extracted to {output_file_path}")