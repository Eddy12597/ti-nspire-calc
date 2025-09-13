#ifndef REPL_H
#define REPL_H



#include <string>
namespace ti {


class Repl {
private:
public:
    Repl();
    ~Repl();
    void println(const std::string& str);
    void clear();
    std::string input(const std::string& prompt);
    
};


}
#endif // REPL_H