#include "header.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

string CLI::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void CLI::ProcessLine(const string& raw_line) {
    string line = trim(raw_line);
    if (line.empty()) return;
    bool is_declaration = false;
    if (line.length() >= 3 && line.substr(0, 3) == "var") {
        if (line.length() == 3 || line[3] == ' ' || line[3] == '\t') {
            is_declaration = true;
        }
    }

    if (is_declaration) {
        size_t equal_index = line.find('=');
        if (equal_index == string::npos) {
            throw runtime_error("no equal sign");
        }

        string var_name = trim(line.substr(3, equal_index - 3));
        string var_value = trim(line.substr(equal_index + 1));

        if (var_name.empty()) {
            throw runtime_error("no variable name");
        }

        if (var_name.find_first_of(" \t") != string::npos) {
            throw runtime_error("variable name cannot contain spaces");
        }

        if (variables.contains(var_name)) {
            throw runtime_error("variable '" + var_name + "' is already defined");
        }

        variables[var_name] = var_value;
    }
    else {
        double result = calc.Evaluate(line, variables);
        cout << result << endl;
    }
}

void CLI::Run() {
    cout << "to exit type \"exit\" \n";
    string input;

    while (true) {
        cout << "> ";
        if (!getline(cin, input)) break;

        if (input == "exit" || input == "quit") {
            break;
        }

        try {
            ProcessLine(input);
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
}
