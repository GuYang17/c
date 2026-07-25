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
        perror(filename);
        return 1;
    }

    char line[MAX_LINE];
    int count = 0;
    while (count < lines && fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
        count++;
    }

    if (filename != NULL) fclose(fp);
    return 0;
}