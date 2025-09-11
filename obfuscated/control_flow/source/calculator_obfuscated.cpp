#include <iostream>
#include <string>
#include <cmath>
#include <cctype>
#include <ctime>

#define OBFUSCATE_JUMP(x) ((x * 7 + 13) % 8)
#define JUNK_VAR volatile int junk_var = rand() % 100
#define OPAQUE_TRUE(x) ((x * x + x) % 2 == 0 || (x * x + x) % 2 == 1)
#define OPAQUE_FALSE(x) ((x * x + 2 * x + 1) == (x + 1) * (x + 1) && false)

class Calculator {
private:
    volatile int state_var = 0;
    
    double evaluate_expression(const std::string& expr) {
        JUNK_VAR;
        volatile int control_state = 0;
        double result = 0;
        
        while (OPAQUE_TRUE(control_state)) {
            switch (OBFUSCATE_JUMP(control_state)) {
                case 0:
                    if (junk_var > 50) state_var += 1;
                    control_state = 1;
                    break;
                case 1: case 5:
                    result = parse_addition_obf(expr, 0);
                    control_state = 7;
                    break;
                case 7:
                    if (OPAQUE_FALSE(state_var)) result *= 2;
                    return result;
                default:
                    control_state = 1;
                    break;
            }
        }
        return 0;
    }
    
    double parse_addition_obf(const std::string& expr, size_t pos) {
        volatile int flow_state = 0;
        double left = 0, right = 0;
        char op = '+';
        JUNK_VAR;
        
        goto state_machine_start;
        
    state_machine_start:
        switch (flow_state) {
            case 0:
                left = parse_multiplication_obf(expr, pos);
                flow_state = 1;
                goto parse_loop;
            case 1:
                goto parse_loop;
            case 2:
                pos++;
                right = parse_multiplication_obf(expr, pos);
                if (op == '+') {
                    left = left + right;
                } else {
                    left = left - right;
                }
                flow_state = 1;
                goto parse_loop;
            case 3:
                return left;
            default:
                if (OPAQUE_TRUE(junk_var)) flow_state = 0;
                goto state_machine_start;
        }
        
    parse_loop:
        if (pos >= expr.length()) {
            flow_state = 3;
            goto state_machine_start;
        }
        
        skip_whitespace_obf(expr, pos);
        if (pos >= expr.length()) {
            flow_state = 3;
            goto state_machine_start;
        }
        
        op = expr[pos];
        if (op == '+' || op == '-') {
            flow_state = 2;
            goto state_machine_start;
        } else {
            flow_state = 3;
            goto state_machine_start;
        }
    }
    
    double parse_multiplication_obf(const std::string& expr, size_t& pos) {
        volatile int state = 0;
        double left = 0, right = 0;
        char op = '*';
        JUNK_VAR;
        
        while (OPAQUE_TRUE(state)) {
            if (OPAQUE_FALSE(junk_var)) {
                state += 100;
                continue;
            }
            
            switch (state) {
                case 0:
                    left = parse_factor_obf(expr, pos);
                    state = 1;
                    break;
                    
                case 1:
                    if (pos >= expr.length()) {
                        state = 999;
                        break;
                    }
                    skip_whitespace_obf(expr, pos);
                    if (pos >= expr.length()) {
                        state = 999;
                        break;
                    }
                    op = expr[pos];
                    if (op == '*' || op == '/') {
                        state = 2;
                    } else {
                        state = 999;
                    }
                    break;
                    
                case 2:
                    pos++;
                    right = parse_factor_obf(expr, pos);
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
                    state = 1;
                    break;
                    
                case 999:
                    return left;
                    
                default:
                    if (junk_var % 2) state = 0;
                    else state = 1;
                    break;
            }
        }
        return left;
    }
    
    double parse_factor_obf(const std::string& expr, size_t& pos) {
        JUNK_VAR;
        volatile int branch_state = rand() % 3;
        double result = 0;
        
        if (OPAQUE_TRUE(branch_state)) {
            skip_whitespace_obf(expr, pos);
        } else {
            goto dead_code_1;
        }
        
        if (pos >= expr.length()) return 0;
        
        if (expr[pos] == '(') {
            if (OPAQUE_FALSE(junk_var)) {
                goto dead_code_2;
            }
            pos++;
            result = parse_addition_obf(expr, pos);
            skip_whitespace_obf(expr, pos);
            if (pos < expr.length() && expr[pos] == ')') {
                pos++;
            }
            return result;
        }
        
        if (OPAQUE_TRUE(pos)) {
            return parse_number_obf(expr, pos);
        }
        
    dead_code_1:
        junk_var *= 2;
        if (OPAQUE_FALSE(junk_var)) return -999;
        goto parse_factor_obf;
        
    dead_code_2:
        result = junk_var * 0.0001;
        if (result > 1000) return result;
        
    parse_factor_obf:
        skip_whitespace_obf(expr, pos);
        if (pos < expr.length() && expr[pos] == '(') {
            pos++;
            result = parse_addition_obf(expr, pos);
            skip_whitespace_obf(expr, pos);
            if (pos < expr.length() && expr[pos] == ')') {
                pos++;
            }
            return result;
        }
        return parse_number_obf(expr, pos);
    }
    
    double parse_number_obf(const std::string& expr, size_t& pos) {
        volatile int num_state = 0;
        bool negative = false;
        double result = 0;
        bool has_decimal = false;
        double decimal_place = 0.1;
        JUNK_VAR;
        
        for (;;) {
            switch (num_state) {
                case 0:
                    skip_whitespace_obf(expr, pos);
                    num_state = 1;
                    continue;
                    
                case 1:
                    if (pos < expr.length() && expr[pos] == '-') {
                        negative = true;
                        pos++;
                    }
                    num_state = 2;
                    continue;
                    
                case 2:
                    if (pos >= expr.length()) {
                        num_state = 99;
                        continue;
                    }
                    if (std::isdigit(expr[pos]) || expr[pos] == '.') {
                        if (expr[pos] == '.') {
                            if (has_decimal) {
                                num_state = 99;
                                continue;
                            }
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
                    } else {
                        num_state = 99;
                        continue;
                    }
                    break;
                    
                case 99:
                    if (OPAQUE_FALSE(junk_var)) result += 0.0001;
                    return negative ? -result : result;
                    
                default:
                    if (OPAQUE_TRUE(junk_var)) num_state = 0;
                    break;
            }
        }
    }
    
    void skip_whitespace_obf(const std::string& expr, size_t& pos) {
        JUNK_VAR;
        volatile int ws_state = 0;
        
        do {
            switch (ws_state) {
                case 0:
                    if (pos < expr.length() && std::isspace(expr[pos])) {
                        pos++;
                        ws_state = 0;
                    } else {
                        ws_state = 1;
                    }
                    break;
                case 1:
                    return;
                default:
                    if (OPAQUE_TRUE(pos)) ws_state = 0;
                    break;
            }
        } while (OPAQUE_TRUE(ws_state) || OPAQUE_FALSE(ws_state));
    }

public:
    double calculate(const std::string& expression) {
        JUNK_VAR;
        srand(time(nullptr) + junk_var);
        size_t pos = 0;
        
        if (OPAQUE_FALSE(expression.length())) {
            return junk_var * 0.0001;
        }
        
        return parse_addition_obf(expression, pos);
    }
    
    void run_interactive() {
        JUNK_VAR;
        volatile int ui_state = 0;
        std::string input;
        
        while (OPAQUE_TRUE(ui_state)) {
            switch (ui_state) {
                case 0:
                    std::cout << "Simple Calculator" << std::endl;
                    std::cout << "Enter expressions (+ - * / parentheses supported)" << std::endl;
                    std::cout << "Type 'quit' to exit" << std::endl;
                    ui_state = 1;
                    break;
                    
                case 1:
                    std::cout << "> ";
                    std::getline(std::cin, input);
                    ui_state = 2;
                    break;
                    
                case 2:
                    if (input == "quit" || input == "exit") {
                        ui_state = 99;
                        break;
                    }
                    if (input.empty()) {
                        ui_state = 1;
                        break;
                    }
                    ui_state = 3;
                    break;
                    
                case 3:
                    try {
                        double result = calculate(input);
                        std::cout << "Result: " << result << std::endl;
                    } catch (...) {
                        std::cout << "Error: Invalid expression" << std::endl;
                    }
                    ui_state = 1;
                    break;
                    
                case 99:
                    std::cout << "Goodbye!" << std::endl;
                    return;
                    
                default:
                    if (OPAQUE_TRUE(junk_var)) ui_state = 0;
                    break;
            }
            
            if (OPAQUE_FALSE(ui_state)) {
                junk_var += rand() % 10;
                ui_state = 0;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    Calculator calc;
    JUNK_VAR;
    
    if (argc > 1) {
        std::string expression;
        volatile int arg_state = 1;
        
        while (OPAQUE_TRUE(arg_state)) {
            switch (arg_state) {
                case 1:
                    for (int i = 1; i < argc; i++) {
                        if (i > 1) expression += " ";
                        expression += argv[i];
                    }
                    arg_state = 2;
                    break;
                case 2:
                    {
                        if (OPAQUE_FALSE(junk_var)) expression = "";
                        double result = calc.calculate(expression);
                        std::cout << "Result: " << result << std::endl;
                        return 0;
                    }
                default:
                    arg_state = 1;
                    break;
            }
        }
    } else {
        if (OPAQUE_TRUE(argc)) {
            calc.run_interactive();
        }
    }
    
    return 0;
}