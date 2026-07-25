#include <stdio.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    char ch;

    if (argc < 2) {
        while ((ch = getchar()) != EOF) {
            putchar(ch);
        }
    } else {
        for (int i = 1; i < argc; i++) {
            fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror("Error opening file");
                continue;
            }
            while ((ch = fgetc(fp)) != EOF) {
                putchar(ch);
            }
            fclose(fp);
        }
    }

    return 0;
}