//creator:zsj
//time:2021/6/17

#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>

class Shell {
public:
    void run();
private:
    void executeCommand(const std::string& command);
    void parseCommand(const std::string& command, std::vector<std::string>& args, std::string& inputFile, std::string& outputFile, std::string& appendFile);
    void handlePipesAndRedirections(std::vector<std::string>& args, const std::string& inputFile, const std::string& outputFile, const std::string& appendFile);
};

#endif // SHELL_H

