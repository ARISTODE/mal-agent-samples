#include <iostream>
#include <string>
#include <cmath>
#include <cctype>

class Calculator {
private:
    double evaluate_expression(const std::string& expr) {
        size_t pos = 0;
        return parse_addition(expr, pos);
    }
    
    double parse_addition(const std::string& expr, size_t& pos) {
        double left = parse_multiplication(expr, pos);
        
        while (pos < expr.length()) {
            skip_whitespace(expr, pos);
            if (pos >= expr.length()) break;
            
            char op = expr[pos];
            if (op == '+' || op == '-') {
                pos++;
                double right = parse_multiplication(expr, pos);
                if (op == '+') {
                    left = left + right;
                } else {
                    left = left - right;
                }
            } else {
                break;
            }
        }
        return left;
    }
    
    double parse_multiplication(const std::string& expr, size_t& pos) {
        double left = parse_factor(expr, pos);
        
        while (pos < expr.length()) {
            skip_whitespace(expr, pos);
            if (pos >= expr.length()) break;
            
            char op = expr[pos];
            if (op == '*' || op == '/') {
                pos++;
                double right = parse_factor(expr, pos);
                if (op == '*') {
                    left = left * right;
                } else {
                    if (right != 0) {
                        left = left / right;
                    } else {
                        std::cout << "Error: Division by zero" << std::endl;
                        return 0;
                    }
                }
            } else {
                break;
            }
        }
        return left;
    }
    
    double parse_factor(const std::string& expr, size_t& pos) {
        skip_whitespace(expr, pos);
        
        if (pos >= expr.length()) return 0;
        
        if (expr[pos] == '(') {
            pos++;
            double result = parse_addition(expr, pos);
            skip_whitespace(expr, pos);
            if (pos < expr.length() && expr[pos] == ')') {
                pos++;
            }
            return result;
        }
        
        return parse_number(expr, pos);
    }
    
    double parse_number(const std::string& expr, size_t& pos) {
        skip_whitespace(expr, pos);
        
        bool negative = false;
        if (pos < expr.length() && expr[pos] == '-') {
            negative = true;
            pos++;
        }
        
        double result = 0;
        bool has_decimal = false;
        double decimal_place = 0.1;
        
        while (pos < expr.length() && (std::isdigit(expr[pos]) || expr[pos] == '.')) {
            if (expr[pos] == '.') {
                if (has_decimal) break;
                has_decimal = true;
            } else {
                if (!has_decimal) {
                    result = result * 10 + (expr[pos] - '0');
                } else {
                    result += (expr[pos] - '0') * decimal_place;
                    decimal_place *= 0.1;
                }
            }
            pos++;
        }
        
        return negative ? -result : result;
    }
    
    void skip_whitespace(const std::string& expr, size_t& pos) {
        while (pos < expr.length() && std::isspace(expr[pos])) {
            pos++;
        }
    }

public:
    double calculate(const std::string& expression) {
        size_t pos = 0;
        return parse_addition(expression, pos);
    }
    
    void run_interactive() {
        std::cout << "Simple Calculator" << std::endl;
        std::cout << "Enter expressions (+ - * / parentheses supported)" << std::endl;
        std::cout << "Type 'quit' to exit" << std::endl;
        
        std::string input;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);
            
            if (input == "quit" || input == "exit") {
                break;
            }
            
            if (input.empty()) {
                continue;
            }
            
            try {
                double result = calculate(input);
                std::cout << "Result: " << result << std::endl;
            } catch (...) {
                std::cout << "Error: Invalid expression" << std::endl;
            }
        }
        
        std::cout << "Goodbye!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    Calculator calc;
    
    if (argc > 1) {
        std::string expression;
        for (int i = 1; i < argc; i++) {
            if (i > 1) expression += " ";
            expression += argv[i];
        }
        
        double result = calc.calculate(expression);
        std::cout << "Result: " << result << std::endl;
    } else {
        calc.run_interactive();
    }
    
    return 0;
}