#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

class MiniShell {
public:
    void run() {
        std::string input;
        while (true) {
            std::cout << "mysh> ";
            std::getline(std::cin, input);
            
            if (input == "exit" || input == "quit") {
                break;
            }
            
            if (input.empty()) {
                continue;
            }
            
            execute(input);
        }
    }

private:
    void execute(const std::string& input) {
        std::vector<std::string> commands;
        std::stringstream ss(input);
        std::string cmd;
        
        while (std::getline(ss, cmd, '|')) {
            commands.push_back(trim(cmd));
        }
        
        if (commands.size() == 1) {
            execute_simple(commands[0]);
        } else {
            execute_pipeline(commands);
        }
    }
    
    void execute_simple(const std::string& cmd) {
        std::vector<std::string> args;
        std::string input_redirect, output_redirect;
        bool append_mode = false;
        
        parse_command(cmd, args, input_redirect, output_redirect, append_mode);
        
        if (args.empty()) return;
        
        if (args[0] == "cd") {
            if (args.size() > 1) {
                chdir(args[1].c_str());
            } else {
                chdir(getenv("HOME"));
            }
            return;
        }
        
        pid_t pid = fork();
        if (pid == 0) {

            if (!input_redirect.empty()) {
                int fd = open(input_redirect.c_str(), O_RDONLY);
                if (fd < 0) {
                    std::cerr << "Error: Cannot open " << input_redirect << std::endl;
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            if (!output_redirect.empty()) {
                int flags = O_WRONLY | O_CREAT;
                if (append_mode) {
                    flags |= O_APPEND;
                } else {
                    flags |= O_TRUNC;
                }
                int fd = open(output_redirect.c_str(), flags, 0644);
                if (fd < 0) {
                    std::cerr << "Error: Cannot open " << output_redirect << std::endl;
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            std::vector<char*> cargs;
            for (auto& arg : args) {
                cargs.push_back(const_cast<char*>(arg.c_str()));
            }
            cargs.push_back(nullptr);
            
            execvp(cargs[0], cargs.data());
            std::cerr << "Error: Command not found: " << args[0] << std::endl;
            exit(1);
        } else if (pid < 0) {
            std::cerr << "Error: Fork failed" << std::endl;
        } else {
            wait(nullptr);
        }
    }
    
    void execute_pipeline(const std::vector<std::string>& commands) {
        int num_commands = commands.size();
        int pipes[num_commands - 1][2];

        for (int i = 0; i < num_commands - 1; i++) {
            if (pipe(pipes[i]) < 0) {
                std::cerr << "Error: Pipe creation failed" << std::endl;
                return;
            }
        }
        
        for (int i = 0; i < num_commands; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                if (i > 0) {
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }
                
                if (i < num_commands - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                
                for (int j = 0; j < num_commands - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                
                std::vector<std::string> args;
                std::string input_redirect;
                std::string output_redirect;
                bool append_mode = false;
                parse_command(commands[i], args, input_redirect, output_redirect, append_mode);
                
                if (args.empty()) exit(1);
                
                std::vector<char*> cargs;
                for (auto& arg : args) {
                    cargs.push_back(const_cast<char*>(arg.c_str()));
                }
                cargs.push_back(nullptr);
                
                execvp(cargs[0], cargs.data());
                std::cerr << "Error: Command not found: " << args[0] << std::endl;
                exit(1);
            } else if (pid < 0) {
                std::cerr << "Error: Fork failed" << std::endl;
            }
        }
        
        for (int i = 0; i < num_commands - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        
        for (int i = 0; i < num_commands; i++) {
            wait(nullptr);
        }
    }
    
    void parse_command(const std::string& cmd, std::vector<std::string>& args,
                       std::string& input_redirect, std::string& output_redirect,
                       bool& append_mode) {
        std::stringstream ss(cmd);
        std::string token;
        bool parsing_args = true;
        
        while (ss >> token) {
            if (token == "<") {
                ss >> input_redirect;
                parsing_args = false;
            } else if (token == ">") {
                ss >> output_redirect;
                append_mode = false;
                parsing_args = false;
            } else if (token == ">>") {
                ss >> output_redirect;
                append_mode = true;
                parsing_args = false;
            } else if (parsing_args) {
                args.push_back(token);
            }
        }
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        if (start == std::string::npos) return "";
        return str.substr(start, end - start + 1);
    }
};

int main() {
    MiniShell shell;
    shell.run();
    return 0;
}