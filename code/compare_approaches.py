import csv
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

category = 'screenshot_web'

compression_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/compression_rates/output_' + category + '.csv'
rle_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/rle_compression_rates/output_' + category + '.csv'
straights_path = '/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/2ndoutput/straights_compress_rates/output_' + category + '.csv'

def read_csv(file_path):
    with open(file_path, mode='r') as file:
        reader = csv.reader(file)
        return [float(entry) for entry in list(reader)[0]]
    
path_rates = read_csv(compression_path)
rle_rates = read_csv(rle_path)
straights_rates = read_csv(straights_path)

print("tree paths: " + str(round((sum(path_rates)/len(path_rates)), 2)))
print("2bit+RLEpaths: " + str(round((sum(rle_rates)/len(rle_rates)), 2)))
print("straights: " + str(round((sum(straights_rates)/len(straights_rates)), 2)))