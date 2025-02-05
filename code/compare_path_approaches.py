import csv
import matplotlib as plt
import pandas as pd
import numpy as np
import os

def read_csv(file_path):
    with open(file_path, mode='r') as file:
        reader = csv.reader(file)
        return [list(map(float, row)) for row in reader]

def calculate_percentage_difference(values1, values2):
    percentage_differences = []
    for v1, v2 in zip(values1, values2):
        percentage_differences.append((v2 - v1) / v1 * 100)
    return percentage_differences

def write_csv(file_path, data):
    with open(file_path, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(data)

def main(file1, file2, output_file):
    values1 = read_csv(file1)
    values2 = read_csv(file2)
    
    if len(values1) != len(values2):
        raise ValueError("Input files must have the same number of rows")
    
    percentage_differences = [calculate_percentage_difference(row1, row2) for row1, row2 in zip(values1, values2)]
    
    write_csv(output_file, percentage_differences)

    

if __name__ == "__main__":
    category = 'photo_wikipedia'
    file1 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_' + category + '.csv'
    file2 = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/rle_compression_rates/output_' + category + '.csv'
    output_file = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/path_vs_rle/path_vs_rle_' + category +'.csv'

    # Create the files if they do not exist
    for file in [file1, file2, output_file]:
        if not os.path.exists(file):
            open(file, 'w').close()
    # Create the file if it does not exist
    #open(output_file, 'a').close()
    main(file1, file2, output_file)