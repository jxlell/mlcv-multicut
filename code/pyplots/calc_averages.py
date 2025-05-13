import pandas as pd
import matplotlib.pyplot as plt

# df_pk = pd.read_csv("code/output_files/mc_results_test_textures_pk.csv")
# df_no_pk = pd.read_csv("code/output_files/mc_results_test_textures_no_pk.csv")
# df = pd.concat([df_pk, df_no_pk], ignore_index=True)

df = pd.read_csv("code/output_files/mc_results_test_screenshots.csv")


columns_to_average = ["red_edgebits_rate", "tree_rate", "rle_rate", "straights_huffman_rate"]  # Specify the columns
for column_name in columns_to_average:
    if column_name in df.columns:
        average = df[column_name].mean()
        print(f"The average of the column '{column_name}' is: {average}")
    else:
        print(f"Column '{column_name}' not found in the CSV file.")

# Count the number of times each column has the highest compression rate in a row
highest_count = {column: 0 for column in columns_to_average}

for _, row in df.iterrows():
    max_value = float('-inf')
    max_column = None
    for column_name in columns_to_average:
        if column_name in df.columns and row[column_name] > max_value:
            max_value = row[column_name]
            max_column = column_name
    if max_column:
        highest_count[max_column] += 1

for column_name, count in highest_count.items():
    print(f"Column '{column_name}' has the highest compression rate in {count} rows.")

# Print the median values for each column in columns_to_average
for column_name in columns_to_average:
    if column_name in df.columns:
        median = df[column_name].median()
        print(f"The median of the column '{column_name}' is: {median}")
    else:
        print(f"Column '{column_name}' not found in the CSV file.")


# Print the highest compression rate and corresponding filename for each column
for column_name in columns_to_average:
    if column_name in df.columns:
        max_value = df[column_name].max()
        max_row = df[df[column_name] == max_value]
        if not max_row.empty:
            filename = max_row["filename"].iloc[0] if "filename" in max_row.columns else "Unknown"
            print(f"The highest compression rate for '{column_name}' is {max_value}, found in file: {filename}")
    else:
        print(f"Column '{column_name}' not found in the CSV file.")