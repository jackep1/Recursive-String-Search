#!/bin/bash

# Check if two arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <target_string> <path_to_root_folder>"
    exit 1
fi

# Assign arguments to variables
target=$1
root_folder=$2

# Check if the given path is valid
if [ ! -d "$root_folder" ]; then
    echo "Error: $root_folder is not a valid directory."
    exit 1
fi

# Initialize counters
total_folders=0
total_num_text_files=0
total_depth=0
total_text_files=()

# Function to recursively search folders and text files
search_folder() {
    local current_folder="$1"
    local current_depth="$2"
    current_depth=$((current_depth + 1))

    # Update max depth
    if [ "$current_depth" -gt "$total_depth" ]; then
        total_depth="$current_depth"
    fi

    # Increase folder counter
    total_folders=$((total_folders + 1))

    # List all .txt files in the current folder
    local local_files=($(find "$current_folder" -maxdepth 1 -type f -name "*.txt"))
    local num_text_files=${#local_files[@]}

    # Increase text file counter
    total_num_text_files=$((total_num_text_files + num_text_files))
    
    # Add this folder's text files to global list
    if [ "$num_text_files" -gt 0 ]; then
        total_text_files+=("${local_files[@]}")
    fi

    # Recursively search subdirectories
    for subdir in "$current_folder"/*; do
        if [ -d "$subdir" ]; then
            search_folder "$subdir" "$current_depth"
        fi
    done
}

# Start the search
echo "Recursive search for target $target starting at $root_folder"
search_folder "$root_folder" "$total_depth"
result=$(./Programs/Executables/prog03 "$target" "${total_text_files[@]}")

# Print final stats
echo "Search depth: $total_depth"
echo "Total number of folders: $total_folders"
echo "Total number of text files: $total_num_text_files"
echo "$result"