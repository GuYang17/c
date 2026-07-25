#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cout << argv[i];
        if (i < argc - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;

    return 0;
}