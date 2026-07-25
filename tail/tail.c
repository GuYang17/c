#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int main(int argc, char *argv[]) {
    int lines = 10;
    char *filename = NULL;

    if (argc >= 2 && strcmp(argv[1], "-n") == 0) {
        if (argc >= 3) {
            lines = atoi(argv[2]);
            if (lines <= 0) {
                fprintf(stderr, "Invalid number of lines: %d\n", lines);
                return 1;
            }
            filename = (argc >= 4) ? argv[3] : NULL;
        }
    } else if (argc >= 2) {
        filename = argv[1];
    }

    FILE *fp = (filename == NULL) ? stdin : fopen(filename, "r");
    if (fp == NULL) {
        perror("filename");
        return 1;
    }

    char **buffer = malloc(lines * sizeof(char*));
    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }
    for (int i = 0; i < lines; i++) {
        buffer[i] = NULL;
    }

    char line[MAX_LINE];
    int index = 0;
    int total_lines = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (buffer[index] != NULL) {
            free(buffer[index]);
        }
        buffer[index] = strdup(line);
        if (buffer[index] == NULL) {
            perror("strdup");
            return 1;
        }

        index = (index + 1) % lines;
        total_lines++;
    }

    int start = (total_lines < lines) ? 0 : index;
    int count = (total_lines < lines) ? total_lines : lines;

    for (int i = 0; i < count; i++) {
        int idx = (start + i) % lines;
        if (buffer[idx] != NULL) {
            printf("%s", buffer[idx]);
        }
    }

    for (int i = 0; i < lines; i++) {
        free(buffer[i]);
    }
    free(buffer);

    if (filename != NULL) fclose(fp);
    return 0;
}