#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
        }
    }
    // Close the file, free line
    fclose(file);
    free(line);
    return target_count;
}

int main(int argc, char* argv[])
{
    // Program requires something to search for, and something to search in
    if (argc < 3)
    {
        printf("Usage: %s <target string> <searchable file 1> <searchable file 2> ... <searchable file N>\n", argv[0]);
        return 1;
    }

    // Assign target string and searchable files
    char* target = argv[1];
    char** searchables = malloc((argc - 2) * sizeof(char*));
    if (searchables == NULL)
    {
        printf("Error: Unable to allocate memory\n");
        return 2;
    }
    for (int i = 2; i < argc; i++)
    {
        searchables[i-2] = argv[i];
    }

    // Search for target string in each searchable file
    int target_count = 0;
    for (int i = 0; i < argc - 2; i++)
    {
        int count = search_file_for(searchables[i], target);
        if (count != -1) { target_count += count; }
    }

    // Report the number of occurrences found
    printf("%d\n", target_count);

    // Free allocated memory
    free(searchables);

    return 0;
}