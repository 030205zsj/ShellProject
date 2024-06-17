//creator:zsj
//time:2021/6/17

#include "shell.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <cstring>

void Shell::run() {
    std::string command;
    while (true) {
        std::cout << "myshell> ";
        std::getline(std::cin, command);
        if (command == "exit") {
            break;
        }
        executeCommand(command);
    }
}

void Shell::parseCommand(const std::string& command, std::vector<std::string>& args, std::string& inputFile, std::string& outputFile, std::string& appendFile) {
    std::istringstream iss(command);
    std::string arg;
    while (iss >> arg) {
        if (arg == "<") {
            iss >> inputFile;
        } else if (arg == ">") {
            iss >> outputFile;
        } else if (arg == ">>") {
            iss >> appendFile;
        } else {
            args.push_back(arg);
        }
    }
}

void Shell::handlePipesAndRedirections(std::vector<std::string>& args, const std::string& inputFile, const std::string& outputFile, const std::string& appendFile) {
    int in = 0, fd[2];
    size_t i = 0;
    
    // Handle input redirection
    if (!inputFile.empty()) {
        in = open(inputFile.c_str(), O_RDONLY);
        if (in < 0) {
            perror("open input file failed");
            return;
        }
    }
    
    while (i < args.size()) {
        std::vector<std::string> cmdArgs;
        while (i < args.size() && args[i] != "|") {
            cmdArgs.push_back(args[i]);
            i++;
        }
        if (i < args.size() && args[i] == "|") {
            i++;
        }

        if (pipe(fd) == -1) {
            perror("pipe failed");
            return;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程
            dup2(in, 0);
            if (i < args.size()) {
                dup2(fd[1], 1);
            } else {
                // Handle output redirection
                if (!outputFile.empty()) {
                    int out = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (out < 0) {
                        perror("open output file failed");
                        exit(EXIT_FAILURE);
                    }
                    dup2(out, 1);
                    close(out);
                } else if (!appendFile.empty()) {
                    int out = open(appendFile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (out < 0) {
                        perror("open append file failed");
                        exit(EXIT_FAILURE);
                    }
                    dup2(out, 1);
                    close(out);
                }
            }
            close(fd[0]);
            close(fd[1]);

            std::vector<char*> c_args;
            for (const auto& s : cmdArgs) {
                c_args.push_back(const_cast<char*>(s.c_str()));
            }
            c_args.push_back(nullptr);

            execvp(c_args[0], c_args.data());
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // 父进程
            wait(nullptr);
            close(fd[1]);
            in = fd[0];
        } else {
            perror("fork failed");
            return;
        }
    }
}

void Shell::executeCommand(const std::string& command) {
    std::vector<std::string> args;
    std::string inputFile, outputFile, appendFile;
    parseCommand(command, args, inputFile, outputFile, appendFile);
    
    if (args.empty()) return;

    handlePipesAndRedirections(args, inputFile, outputFile, appendFile);
}
