#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <unordered_map>

class Calculator {
private:
    int GetPriority(std::string oper);
    bool Contains(std::string vtr, char c);
    bool TryParseDouble(const std::string& token, double& outNumber);

    std::vector<std::string> Tokenize(std::string input, const std::unordered_map<std::string, std::string>& env);
    std::vector<std::string> TurnToRPN(std::vector<std::string> tokens);
    double Calculate(std::vector<std::string> RPN);

public:
    double Evaluate(std::string expression, const std::unordered_map<std::string, std::string>& env);
};

class CLI {
private:
    std::unordered_map<std::string, std::string> variables;
    Calculator calc;

    std::string trim(const std::string& str);
    void ProcessLine(const std::string& line);

public:
    void Run();
};

#endif
