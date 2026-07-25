#include <stdio.h>
#include <ctype.h>

void wc_file(FILE *fp, int *lines, int *words, int *chars) {
    int ch;
    int in_word = 0;

    *lines = *words = *chars = 0;

    while ((ch = fgetc(fp)) != EOF) {
        (*chars)++;

        if (ch == '\n') {
            (*lines)++;
        }

        if (isspace(ch)) {
            in_word = 0;
        } else {
            if (in_word == 0) {
                in_word = 1;
                (*words)++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int lines, words, chars;

    if (argc < 2) {
        wc_file(stdin, &lines, &words, &chars);
        printf("%7d %7d %7d\n", lines, words, chars);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror(argv[i]);
                continue;
            }
            wc_file(fp, &lines, &words, &chars);
            fclose(fp);
            printf("%7d %7d %7d %s\n", lines, words, chars, argv[i]);
        }
    }
    return 0;
}