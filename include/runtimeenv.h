#ifndef RUNTIMEENV_H
#define RUNTIMEENV_H

#include "value.h"
#include <unordered_map>
#include <string>


namespace ti {

struct function;

class runtime_env {
private:
    std::unordered_map<std::string, std::shared_ptr<ti::value>> variables;
    std::unordered_map<std::string, function> functions;
public:


};

}


#endif // RUNTIMEENV_H