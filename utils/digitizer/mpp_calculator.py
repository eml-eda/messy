import os
import pandas as pd
import matplotlib.pyplot as plt
import sys

# Create the 'out' folder if it doesn't exist
output_folder = 'out'
os.makedirs(output_folder, exist_ok=True)

# Get the CSV filename from command-line argument or use default 'digitized_data.csv'
if len(sys.argv) > 1:
    csv_filename = sys.argv[1]
else:
    csv_filename = 'digitized_data.csv'

# Read the CSV file containing X and Y values
data = pd.read_csv(csv_filename, header=None, names=['X', 'Y'])

# Compute (X * Y) for each row
data['X_times_Y'] = data['X'] * data['Y']

# Find the maximum power point (MPP)
max_power_point = data.loc[data['X_times_Y'].idxmax()]

# Save MPP data to a separate CSV file
mpp_data = pd.DataFrame({'Voltage': [max_power_point['X']], 'Current': [max_power_point['Y']], 'Power': [max_power_point['X_times_Y']]})
mpp_data.to_csv(os.path.join(output_folder, 'mpp.csv'), index=False)

# Generate a plot with only the maximum power point
plt.plot(data['X'], data['Y'])
plt.scatter(max_power_point['X'], max_power_point['Y'], color='red', label='MPP')
plt.xlabel('Voltage (V)')
plt.ylabel('Current (mA)')
plt.legend()
plt.grid(True)
plt.savefig(os.path.join(output_folder, 'original_plot.png'))
plt.close()

# Generate a plot with V on the x-axis and P on the y-axis
plt.plot(data['X'], data['X_times_Y'])
plt.scatter(max_power_point['X'], max_power_point['X_times_Y'], color='red', label='MPP')
plt.xlabel('Voltage (V)')
plt.ylabel('Power (mW)')
plt.legend()
plt.grid(True)
plt.savefig(os.path.join(output_folder, 'mpp_plot.png'))
plt.close()

print("Maximum power point data saved to 'out/mpp.csv'")
print("Original plot with maximum power point saved as 'out/original_plot.png'")
print("Plot of Voltage vs Power saved as 'out/mpp_plot.png'")
