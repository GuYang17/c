#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        char ch;
        while (std::cin.get(ch)) {
            std::cout << ch;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            std::ifstream file(argv[i]);
            if (!file.is_open()) {
                std::perror(("Error opening file: " + std::string(argv[i])).c_str());
                continue;
            }

            char ch;
            while (file.get(ch)) {
                std::cout << ch;
            }
            file.close();
        }
    }

    return 0;
}