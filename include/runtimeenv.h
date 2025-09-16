#ifndef RUNTIMEENV_H
#define RUNTIMEENV_H

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "value.h" // must define ti::valptr_t and value types
#include "ast.h"   // forward-declared runtime_env in ast.h, safe to include

namespace ti {

// function object stored in environment
struct function {
    // parameter names, empty for builtins that accept variadic args
    std::vector<std::string> params;

    // Body: AST node to execute for user functions (may be null for builtins)
    std::shared_ptr<ast::node> body; // typically an exprnode or block node

    // builtin flag and implementation
    bool isBuiltin = false;
    // builtin receives args vector and runtime_env reference
    std::function<valptr_t(const std::vector<valptr_t>&, runtime_env&)> builtinImpl;

    function() = default;

    // user function
    function(const std::vector<std::string>& p, std::shared_ptr<ast::node> b)
        : params(p), body(std::move(b)), isBuiltin(false), builtinImpl(nullptr) {}

    // builtin function
    function(std::function<valptr_t(const std::vector<valptr_t>&, runtime_env&)> impl)
        : params(), body(nullptr), isBuiltin(true), builtinImpl(std::move(impl)) {}
};

class runtime_env {
private:
    std::unordered_map<std::string, valptr_t> variables;
    std::unordered_map<std::string, function> functions;

public:
    runtime_env() = default;

    // variables
    valptr_t getVariable(const std::string &name) const;
    void setVariable(const std::string &name, const valptr_t &value);

    // functions
    void defineFunction(const std::string &name, const function &fn);
    bool hasFunction(const std::string &name) const;
    function* getFunction(const std::string &name);
    valptr_t callFunction(const std::string &name, const std::vector<valptr_t> &args);

    // helper for registering builtin
    void registerBuiltin(const std::string &name, std::function<valptr_t(const std::vector<valptr_t>&, runtime_env&)> impl);
};

} // namespace ti

#endif // RUNTIMEENV_H
