import matplotlib.pyplot as plt

# Categories
categories = ['Natural Photos', 'Icons', 'Screenshots']

# New Compression Rates
values = [0.85,16.7,18.4]
min_values = [0.8,2.52,0.8]
max_values = [1.07,94.3,621]

# Old Compression Rates
old_values = [0.98, 7.6, 4.57]
old_min_values = [0.925, 2.55, 0.927]
old_max_values = [1.23, 11.6, 12]
# Plotting the bar graph
plt.bar(categories, values)
# Plotting the bar graph with error bars
plt.bar(categories, values, yerr=[min_values, max_values], capsize=5)
# Adding labels and title
#plt.xlabel('Categories')
plt.ylabel('Compression Rates')
plt.title('Bar Graph')
#plt.xticks(rotation=90)

# Displaying the graph
plt.axhline(y=1, color='r', linestyle='--')
plt.show()
# Displaying the graph
plt.show()