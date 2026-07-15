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

int Calculator::GetPriority(string oper) {
    if (oper == "(") return 0;
    else if (oper == "+" || oper == "-") return 1;
    else if (oper == "*" || oper == "/") return 2;
    else if (oper == "max" || oper == "min" || oper == "pow" || oper == "abs") return 3;
    return -1;
}

bool Calculator::Contains(string vtr, char c) {
    for (char char_in_vtr : vtr) {
        if (char_in_vtr == c) return true;
    }
    return false;
}

bool Calculator::TryParseDouble(const string& token, double& outNumber) {
    if (token.find(',') != string::npos) {
        return false;
    }
    try {
        size_t idx;
        outNumber = stod(token, &idx);
        return idx == token.length();
    }
    catch (...) {
        return false;
    }
}

vector<string> Calculator::Tokenize(string input, const unordered_map<string, string>& env) {
    vector<string> tokens;
    string buffer = "";
    string var_buffer = "";

    for (int i = 0; i < input.size(); i++) {
        char s = input[i];
        if (isdigit(s) || s == '.') {
            buffer += s;
        }
        else if (s == '+' || s == '-' || s == '*' || s == '/' || s == '(' || s == ')' || s == ',') {
            if (buffer != "") {
                tokens.push_back(buffer);
                buffer = "";
            }
            if (var_buffer != "") {
                if (env.contains(var_buffer)) {
                    vector<string> var_tokenized = Tokenize(env.at(var_buffer), env);
                    tokens.insert(tokens.end(), var_tokenized.begin(), var_tokenized.end());
                }
                else {
                    tokens.push_back(var_buffer);
                }
                var_buffer = "";
            }
            tokens.push_back(string(1, s));
        }
        else if ((i + 2) < input.size() &&
            (input.substr(i, 3) == "max" || input.substr(i, 3) == "min" ||
                input.substr(i, 3) == "pow" || input.substr(i, 3) == "abs"))
        {
            if (buffer != "") { tokens.push_back(buffer); buffer = ""; }
            if (var_buffer != "") {
                if (env.contains(var_buffer)) {
                    vector<string> var_tok = Tokenize(env.at(var_buffer), env);
                    tokens.insert(tokens.end(), var_tok.begin(), var_tok.end());
                }
                else { tokens.push_back(var_buffer); }
                var_buffer = "";
            }
            tokens.push_back(input.substr(i, 3));
            i += 2;
        }
        else if (s == ' ') {
            if (buffer != "") {
                tokens.push_back(buffer);
                buffer = "";
            }
            if (var_buffer != "") {
                if (env.contains(var_buffer)) {
                    vector<string> var_tokenized = Tokenize(env.at(var_buffer), env);
                    tokens.insert(tokens.end(), var_tokenized.begin(), var_tokenized.end());
                }
                else {
                    tokens.push_back(var_buffer);
                }
                var_buffer = "";
            }
        }
        else {
            var_buffer += s;
        }
    }

    if (buffer != "") {
        tokens.push_back(buffer);
    }
    if (var_buffer != "") {
        if (env.contains(var_buffer)) {
            vector<string> var_tokenized = Tokenize(env.at(var_buffer), env);
            tokens.insert(tokens.end(), var_tokenized.begin(), var_tokenized.end());
        }
        else {
            tokens.push_back(var_buffer);
        }
    }
    return tokens;
}

vector<string> Calculator::TurnToRPN(vector<string> tokens) {
    stack<string> operatorStack;
    vector<string> result;

    for (int i = 0; i < tokens.size(); i++) {
        string token = tokens[i];
        double token_double;
        if (TryParseDouble(token, token_double)) {
            result.push_back(token);
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/") {
            while (!operatorStack.empty() &&
                operatorStack.top() != "(" &&
                (GetPriority(token) < GetPriority(operatorStack.top()) ||
                    GetPriority(token) == GetPriority(operatorStack.top()))) {
                result.push_back(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(token);
        }
        else if (token == "max" || token == "min" || token == "pow" || token == "abs") {
            operatorStack.push(token);
        }
        else if (token == "(") {
            operatorStack.push(token);
        }
        else if (token == ",") {
            while (!operatorStack.empty() && operatorStack.top() != "(") {
                result.push_back(operatorStack.top());
                operatorStack.pop();
            }
        }
        else if (token == ")") {
            while (!operatorStack.empty() && operatorStack.top() != "(") {
                result.push_back(operatorStack.top());
                operatorStack.pop();
            }

            if (!operatorStack.empty()) {
                operatorStack.pop();
            }

            if (!operatorStack.empty() &&
                (operatorStack.top() == "max" || operatorStack.top() == "min" ||
                    operatorStack.top() == "pow" || operatorStack.top() == "abs")) {
                result.push_back(operatorStack.top());
                operatorStack.pop();
            }
        }
        else {
            throw runtime_error("Error: variable '" + token + "' is not defined");

        }
    }

    while (!operatorStack.empty()) {
        result.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return result;
}

double Calculator::Calculate(vector<string> RPN) {
    if (RPN.empty()) return 0.0;

    stack<double> s;

    for (int i = 0; i < RPN.size(); i++) {
        string token = RPN[i];
        double number;

        if (TryParseDouble(token, number)) {
            s.push(number);
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/" ||
            token == "max" || token == "min" || token == "pow") {
            double num2 = s.top(); s.pop();
            double num1 = s.top(); s.pop();

            if (token == "+") { s.push(num1 + num2); }
            else if (token == "-") { s.push(num1 - num2); }
            else if (token == "*") { s.push(num1 * num2); }
            else if (token == "/") {
                if (num2 == 0) { throw runtime_error("ZeroDivision error"); }
                s.push(num1 / num2);
            }
            else if (token == "max") { s.push(max(num1, num2)); }
            else if (token == "min") { s.push(min(num1, num2)); }
            else if (token == "pow") { s.push(pow(num1, num2)); }
        }
        else if (token == "abs") {
            double num1 = s.top(); s.pop();
            s.push(abs(num1));
        }

        else {
            throw runtime_error("variable '" + token + "' is not defined");
        }
    }

    return s.top();
}

double Calculator::Evaluate(string expression, const unordered_map<string, string>& env) {
    vector<string> tokens = Tokenize(expression, env);
    if (tokens.empty()) return 0.0;
    vector<string> rpn = TurnToRPN(tokens);
    return Calculate(rpn);
}
