// repl.cpp

#include <algorithm>
#include <cctype>
#include <sstream>
#include "../include/repl.h"
#include "../include/token.h"

std::string ti::repl::input(const std::string& prompt) {
    

    std::string fullInput;
    std::string str;

    std::cout << col::cyan << prompt << col::RESET;
    if (!std::getline(std::cin, str)) return std::string();
    fullInput = str;

    std::string trimmed = tk::trim(str);
    std::vector<std::string> stack;
    int consecutiveEmpty = trimmed.empty() ? 1 : 0;
    stack = tk::recompute_stack(fullInput);
    bool multiline = !stack.empty() || tk::is_continue_only(trimmed);

    while (multiline) {
        std::string cont = "... ";
        std::cout << col::cyan << cont << [&](const int n, const std::string& s) -> std::string {
            std::string str; 
            for (int i = 0; i < std::max(n - static_cast<int>(cont.size()), 0); i++) {
                str += s;
            }
            return str;
        }(prompt.size(), " ") << col::RESET;
        if (!std::getline(std::cin, str)) break;
        fullInput += "\n";
        fullInput += str;
        trimmed = tk::trim(str);

        if (trimmed.empty()) {
            consecutiveEmpty++;
            if (consecutiveEmpty >= 2) {
                break;
            }
        } else {
            consecutiveEmpty = 0;
        }

        // recompute stack from full input and decide
        stack = tk::recompute_stack(fullInput);
        std::string lastNonEmpty;
        size_t p = fullInput.find_last_not_of('\n');
        if (p == std::string::npos) lastNonEmpty = std::string();
        else {
            size_t lineStart = fullInput.find_last_of('\n', p);
            if (lineStart == std::string::npos) lastNonEmpty = tk::trim(fullInput);
            else lastNonEmpty = tk::trim(fullInput.substr(lineStart + 1));
        }
        // if stack empty and last non-empty line does not request continuation, break immediately
        if (stack.empty() && !tk::is_continue_only(lastNonEmpty)) {
            break;
        }
        multiline = true;

        // if no stack and last line not continue-only, we'll exit loop naturally
    }

    tk::trim_trailing_empty(fullInput);

    return fullInput;
}

// TI Nspire uses 'expr' for eval/exec
ti::cmdres ti::repl::expr(const std::string& code) {
    std::vector<std::string> tokens = tk::tokenize(code);
    // TODO: implement
    
    int exitcode;
    std::stringstream out;

    // test tokenization => good
    // std::stringstream out;
    // for (const auto& str : tokens) {
    //     out << str << ", ";
    // }
    return ti::cmdres(0, out.str());
}

void ti::repl::run() {
    while (true) {
        std::string input = this->input("ti> ");
        if (input == "exit") {
            std::cout << col::RESET << std::endl;
            break;
        } else if (input == "clear") {
            std::cout << col::CLEAR << col::RESET << std::endl;
            continue;
        }
        ti::cmdres res = this->expr(input);
        if (res.exitcode != 0) {
            std::cout << col::red;
        }
        std::cout << res.output << col::RESET << std::endl;
    }
}