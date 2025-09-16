#include "../include/runtimeenv.h"
#include <stdexcept>
#include <iostream>
#include <string>

namespace ti {

valptr_t runtime_env::getVariable(const std::string &name) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        throw std::runtime_error("undefined variable: " + name);
    }
    return it->second;
}

void runtime_env::setVariable(const std::string &name, const valptr_t &value) {
    variables[name] = value;
}

void runtime_env::defineFunction(const std::string &name, const function &fn) {
    functions[name] = fn;
}

bool runtime_env::hasFunction(const std::string &name) const {
    return functions.find(name) != functions.end();
}

function* runtime_env::getFunction(const std::string &name) {
    auto it = functions.find(name);
    if (it == functions.end()) return nullptr;
    return &it->second;
}

valptr_t runtime_env::callFunction(const std::string &name, const std::vector<valptr_t> &args) {
    auto it = functions.find(name);
    if (it == functions.end()) {
        throw std::runtime_error("undefined function: " + name);
    }
    function &fn = it->second;
    if (fn.isBuiltin) {
        return fn.builtinImpl(args, *this);
    } else {
        // very simple user function handling:
        // - bind positional params to local variables (no local stack implemented here)
        // - evaluate body (the body must be an ast::exprnode)
        // NOTE: for a robust interpreter you should create scoped callframes, but keep it simple here.
        if (fn.body == nullptr) return valptr_t(); // null
        // create a temporary env copy for function call (simple lexical isolation)
        runtime_env local = *this; // shallow copy of variables and functions
        for (size_t i = 0; i < fn.params.size() && i < args.size(); ++i) {
            local.setVariable(fn.params[i], args[i]);
        }
        // evaluate
        ast::exprnode* expr = dynamic_cast<ast::exprnode*>(fn.body.get());
        if (!expr) return valptr_t();
        return expr->eval(local);
    }
}

void runtime_env::registerBuiltin(const std::string &name, std::function<valptr_t(const std::vector<valptr_t>&, runtime_env&)> impl) {
    functions[name] = function(std::move(impl));
}

// register a simple 'disp' builtin example
void register_default_builtins(runtime_env &env) {
    env.registerBuiltin("disp", [](const std::vector<valptr_t> &args, runtime_env & /*env*/) -> valptr_t {
        for (auto &a : args) {
            if (a) std::cout << a->toString() << " ";
            else std::cout << "<null> ";
        }
        std::cout << std::endl;
        return valptr_t(); // or return none if you have one
    });
}

} // namespace ti
