#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/// @brief Checks if a file has a valid extension
/// @param filename Character array containing the filename
/// @param extensions Array of character arrays containing valid extensions
/// @return 1 if the file has a valid extension, 0 otherwise
int is_valid_extension(const char* filename, const char** extensions)
{
    char* dot = strrchr(filename, '.');
    if (dot == NULL)
    {
        return 0;
    }
    for (int i = 0; extensions[i] != NULL; i++)
    {
        if (strcmp(dot, extensions[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

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
        //exit(3);
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
int search_dir_for(const char* directory, const char* target, const char** extensions)
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
        if (stat(filepath, &filestat) == 0 && S_ISREG(filestat.st_mode))
        {
            if (is_valid_extension(entry->d_name, extensions) == 0)
            {
                continue;
            }
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
    if (argc < 3)
    {
        printf("Usage: %s <target string> <search directory> <file extension 1> ... <file extension N>", argv[0]);
        return 1;
    }

    // Assign target string, searchable directory, and file extensions
    char* target = argv[1];
    char* search_dir = argv[2];
    char** extensions = malloc((argc - 3) * sizeof(char*));
    if (search_dir == NULL || extensions == NULL)
    {
        printf("Error: Unable to allocate memory\n");
        return 2;
    }
    for (int i = 3; i < argc; i++)
    {
        extensions[i-3] = argv[i];
    }
    // Search the directory for .txt files, and search each file for the target string
    int target_count = search_dir_for(search_dir, target, (const char**)extensions);

    // Report the number of occurrences found
    printf("%d\n", target_count);

    return 0;
}