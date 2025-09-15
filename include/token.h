#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace tk {

    std::set<std::string> continueOnlyKwds = {
        "\\",
        "then",
        "else",
        "elseif",
    };
    
    std::unordered_map<std::string, std::string> blockKwPairs = {
        {"prgm", "endprgm"},
        {"func", "endfunc"},
        {"for", "endfor"},
        {"if", "endif"},
        {"while", "endwhile"},
        {"loop", "endloop"},
        {"try", "endtry"},
    };

    std::set<std::string> endReadlnKwds = {
        "endprgm",
        "endfunc",
        "endfor",
        "endif",
        "endwhile",
        "endloop",
        "endtry"
    };
    

    auto trim = [](const std::string& s) -> std::string {
        const std::string ws = " \t\r\n";
        size_t start = s.find_first_not_of(ws);
        if (start == std::string::npos) return std::string();
        size_t end = s.find_last_not_of(ws);
        return s.substr(start, end - start + 1);
    };

    // tokenizer: extract identifiers, numbers, and symbol/operator tokens; lowercase identifiers
    auto tokenize = [](const std::string &line) -> std::vector<std::string> {
        std::vector<std::string> toks;
        size_t i = 0;
        const size_t n = line.size();
        while (i < n) {
            char ch = line[i];
            // skip whitespace
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') { ++i; continue; }

            // identifier: letter or underscore followed by letters/digits/underscores
            if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
                std::string cur;
                while (i < n && (std::isalnum(static_cast<unsigned char>(line[i])) || line[i] == '_')) {
                    cur.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(line[i]))));
                    ++i;
                }
                toks.push_back(cur);
                continue;
            }

            // number: digit sequence, allow single decimal point
            if (std::isdigit(static_cast<unsigned char>(ch))) {
                std::string cur;
                bool seen_dot = false;
                while (i < n && (std::isdigit(static_cast<unsigned char>(line[i])) || (!seen_dot && line[i] == '.'))) {
                    if (line[i] == '.') seen_dot = true;
                    cur.push_back(line[i]);
                    ++i;
                }
                toks.push_back(cur);
                continue;
            }

            // symbols / operators: check multi-char operators first
            auto peek2 = [&](size_t p) -> std::string {
                if (p + 1 < n) return std::string() + line[p] + line[p+1];
                return std::string();
            };
            std::string two = peek2(i);
            if (!two.empty()) {
                if (two == "->" || two == ">=" || two == "<=" || two == "==" || two == "!=" || two == "&&" || two == "||" || two == "<<" || two == ">>") {
                    toks.push_back(two);
                    i += 2;
                    continue;
                }
            }

            // single-char symbol
            toks.push_back(std::string(1, ch));
            ++i;
        }
        return toks;
    };

    auto get_first_token = [](const std::string &line) -> std::string {
        auto toks = tokenize(line);
        return toks.empty() ? std::string() : toks.front();
    };

    auto get_last_token = [](const std::string &line) -> std::string {
        auto toks = tokenize(line);
        return toks.empty() ? std::string() : toks.back();
    };

    auto is_continue_only = [](const std::string& line) -> bool {
        std::string l = get_last_token(line);
        // continue if line ends with ':' explicitly
        if (!line.empty() && line.back() == ':') return true;
        for (const auto& kw : tk::continueOnlyKwds) if (!kw.empty() && l == kw) return true;
        return false;
    };

    auto find_block_start = [](const std::string& line) -> std::string {
        std::string f = get_first_token(line);
        if (f.empty()) return std::string();
        for (const auto& p : tk::blockKwPairs) {
            if (f == p.first) return p.second;
        }
        return std::string();
    };

    auto find_end_kw = [](const std::string& line) -> std::string {
        std::string f = get_first_token(line);
        if (f.empty()) return std::string();
        for (const auto& kw : tk::endReadlnKwds) {
            if (f == kw) return kw;
        }
        return std::string();
    };

    auto recompute_stack = [](const std::string &all) -> std::vector<std::string> {
        std::vector<std::string> st;
        size_t pos = 0;
        while (pos <= all.size()) {
            size_t next = all.find('\n', pos);
            std::string line = (next == std::string::npos) ? all.substr(pos) : all.substr(pos, next - pos);
            std::string tline = tk::trim(line);
            if (!tline.empty()) {
                std::string nb = tk::find_block_start(tline);
                if (!nb.empty()) {
                    st.push_back(nb);
                    pos = (next == std::string::npos) ? all.size() + 1 : next + 1;
                    continue;
                }
                std::string fe = tk::find_end_kw(tline);
                if (!fe.empty()) {
                    // pop until match
                    if (!st.empty() && st.back() == fe) {
                        st.pop_back();
                    } else {
                        auto it = std::find(st.rbegin(), st.rend(), fe);
                        if (it != st.rend()) {
                            while (!st.empty() && st.back() != fe) st.pop_back();
                            if (!st.empty() && st.back() == fe) st.pop_back();
                        }
                    }
                }
            }
            if (next == std::string::npos) break;
            pos = next + 1;
        }
        return st;
    };

    auto is_line_empty = [](const std::string &line) {
        const std::string ws = " \t\r\n";
        size_t start = line.find_first_not_of(ws);
        return start == std::string::npos;
    };

    // Trim trailing empty lines the user may have entered to force break
    auto trim_trailing_empty = [](std::string &s) {
        
        while (true) {
            // find last line
            size_t pos = s.find_last_of('\n');
            std::string last = (pos == std::string::npos) ? s : s.substr(pos + 1);
            if (is_line_empty(last)) {
                // remove last line and its preceding newline if any
                if (pos == std::string::npos) {
                    s.clear();
                    break;
                } else {
                    s.erase(pos); // remove from last '\n' onwards
                    continue;
                }
            }
            break;
        }
    };

};

#endif // TOKEN_H