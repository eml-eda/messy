import sys
import pandas as pd
import matplotlib.pyplot as plt

def plot_data(data, scale_factor=10):
    # Extract timestamp and values
    timestamp = data.iloc[:, 0]
    values = data.iloc[:, 1:]
    
    # Scale the first curve
    scaled_values = values.copy()
    scaled_values.iloc[:, 2] *= scale_factor
    
    # Plotting
    plt.figure(figsize=(10, 6))
    plt.plot(timestamp, scaled_values.iloc[:, 2], label=f"{values.columns[2]} (scaled by {scale_factor})")
    plt.plot(timestamp, values.iloc[:, 3], label=values.columns[3])
    plt.xlabel('Timestamp')
    plt.ylabel('Values')
    plt.title('Data Visualization')
    plt.legend()
    plt.grid(True)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # Check if a filename is provided as command line argument
    if len(sys.argv) != 2:
        print("Usage: python plotter.py <data_file>")
        sys.exit(1)

    # Read the data file
    filename = sys.argv[1]
    try:
        with open(filename, 'r') as f:
            # Skip the first character
            f.read(1)
            # Read the remaining content of the file
            data = pd.read_csv(f, sep='\s+', skipinitialspace=True, comment='%')
    except FileNotFoundError:
        print("File not found!")
        sys.exit(1)
    except pd.errors.ParserError:
        print("Error parsing the file!")
        sys.exit(1)

    # Check if the file contains at least two columns
    if len(data.columns) < 2:
        print("File must contain at least two columns (timestamp and one value)!")
        sys.exit(1)

    # Plot the data with the first curve scaled
    plot_data(data)
