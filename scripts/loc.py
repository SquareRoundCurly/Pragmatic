import os
import sys
import argparse

# Function to count lines of code in a file
def count_lines_of_code(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
        lines = file.readlines()
    return len(lines)

# Function to traverse directories and count LOC
def count_loc_in_directory(directory_path, extensions_blacklist):
    total_loc = 0

    for root, _, files in os.walk(directory_path):
        for file_name in files:
            file_path = os.path.join(root, file_name)
            
            # Check if the file has a blacklisted extension
            _, extension = os.path.splitext(file_name)
            if extension.lower() in extensions_blacklist:
                continue

            loc = count_lines_of_code(file_path)
            total_loc += loc

    return total_loc

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Count lines of code in specified directories.")
    parser.add_argument("directories", nargs="+", help="Directories to count LOC in")
    parser.add_argument("--blacklist", nargs="+", default=[], help="Blacklisted file extensions")
    parser.add_argument("-v", "--verbose", action="store_true", help="Print detailed information")
    
    args = parser.parse_args()

    total_loc = 0

    for directory in args.directories:
        if not os.path.exists(directory):
            print(f"Directory not found: {directory}")
            continue

        loc = count_loc_in_directory(directory, args.blacklist)
        total_loc += loc
        if args.verbose:
            print(f"{directory}: {loc}")

    if args.verbose:
        print(f"Total LOC: {total_loc}")
    else:
        print(total_loc)
