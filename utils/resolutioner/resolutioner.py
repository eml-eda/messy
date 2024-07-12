import sys

def repeat_lines(input_file, output_file, num_repeats):
    try:
        with open(input_file, 'r') as file:
            lines = file.readlines()

        total_lines = len(lines)
        with open(output_file, 'w') as file:
            for i, line in enumerate(lines):
                for _ in range(num_repeats):
                    file.write(line)
                print(f"Progress: {((i+1)/total_lines)*100:.2f}%")
    except FileNotFoundError:
        print(f"The file {input_file} does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python resolutioner.py <input_file> <output_file> <num_repeats>")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        num_repeats = int(sys.argv[3])
        repeat_lines(input_file, output_file, num_repeats)
