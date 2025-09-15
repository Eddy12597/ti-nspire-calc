#ifndef REPL_H
#define REPL_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include "colors.h"

namespace ti {

struct cmdres {
    int exitcode = 0;
    std::string output;
    cmdres (int exitcode, const std::string& output) : exitcode(exitcode), output(output) {}
    cmdres (const std::string& output) : exitcode(0), output(output) {}
};

class repl {
private:
    std::vector<std::string> history;
public:
    repl() = default;
    repl(const repl&) = default;
    repl(repl&&) = default;
    repl& operator=(const repl&) = default;
    repl& operator=(repl&&) = default;
    
    ~repl() = default;
    
    void clear();
    std::string input(const std::string& prompt);
    cmdres expr(const std::string& code);

    void run();

};


}
#endif // REPL_H