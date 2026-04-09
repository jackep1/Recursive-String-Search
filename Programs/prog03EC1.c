#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/// @brief Searches a file for a target string
/// @param filename Character array containing the filename
/// @param target Character array of the string to be searched for
/// @return The number of times that the target string was found in the file
int search_file_for(const char* filename, const char* target)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        // Should exit the program, or just report one non-searchable?
        printf("Error: Unable to open file %s\n", filename);
        return -1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int target_count = 0;

    while ((read = getline(&line, &len, file)) != -1)
    {
        // For each line, search character by character
        char* match = line;
        while ((match = strstr(match, target)) != NULL)
        {
            // For each match, increment target count and move match pointer to the next character
            target_count++;
            match++;
            //printf("Found %s in %s\n", target, filename);
        }
    }
    // Close the file, free line
    fclose(file);
    free(line);
    return target_count;
}

/// @brief Non-recursively searches all of a directory's files for a target string by calling search_file_for() on each
/// @param directory The directory whose contents should be searched
/// @param target Character array of the string to be searched for
/// @param extensions Array of character arrays containing valid extensions
/// @return The number of times that the target string was found in the directory's files
int search_dir_for(const char* directory, const char* target)
{
    DIR* dir = opendir(directory);
    if (dir == NULL)
    {
        printf("Error: Unable to open directory %s\n", directory);
        return -1;
    }

    struct dirent* entry;
    struct stat filestat;
    char filepath[PATH_MAX];
    int target_count = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip hidden files, and the current and parent directories
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        // Make full file path
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        // Check check that entry is file and .txt
        if (stat(filepath, &filestat) == 0 && S_ISREG(filestat.st_mode) && strstr(entry->d_name, ".txt") != NULL)
        {
            int count = search_file_for(filepath, target);
            if (count != -1) { target_count += count; }
        }
    }
    closedir(dir);
    return target_count;
}

int main(int argc, char* argv[])
{
    // Program requires something to search for, and something to search in
    if (argc != 3)
    {
        printf("Usage: %s <target string> <search directory>", argv[0]);
        return 1;
    }

    // Assign target string and searchable directory
    char* target = argv[1];
    char* search_dir = argv[2];
    if (search_dir == NULL)
    {
        printf("Error: Unable to allocate memory\n");
        return 2;
    }
    // Search the directory for .txt files, and search each file for the target string
    int target_count = search_dir_for(search_dir, target);

    // Report the number of occurrences found
    printf("%d\n", target_count);

    return 0;
}