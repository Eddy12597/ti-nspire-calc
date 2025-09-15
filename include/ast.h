#ifndef AST_H
#define AST_H

#include "value.h"
#include <vector>
#include <string>
#include <functional>

class runtimeenv;

namespace ast {

struct node {
    virtual ~node() = default;
};

struct exprnode : node {
    virtual ti::value eval() = 0;
};

}

#include "runtimeenv.h"

struct function {
    std::vector<std::string> params;
    std::shared_ptr<ast::node> body;
    bool isBuiltin;
    std::function<std::shared_ptr<ti::value>(const std::vector<std::shared_ptr<ti::value>>&)> builtinImpl;
};

#endif // AST_H