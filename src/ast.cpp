#include "../include/ast.h"
#include "../include/runtimeenv.h"
#include "../include/value.h"
#include <stdexcept>

using namespace ast;

// var_node
ti::valptr_t var_node::eval(ti::runtime_env &env) {
    return env.getVariable(name);
}

// assign_node
ti::valptr_t assign_node::eval(ti::runtime_env &env) {
    ti::valptr_t v = rhs->eval(env);
    env.setVariable(name, v);
    return v;
}

// call_node
ti::valptr_t call_node::eval(ti::runtime_env &env) {
    // evaluate callee - support simple function name via var_node
    // if callee eval returns something special, could be a first-class function; for now we expect callee to be a var_node
    // if callee is var_node, we can extract name without evaluating it:
    // Try dynamic_cast to var_node
    if (var_node* vn = dynamic_cast<var_node*>(callee.get())) {
        std::vector<ti::valptr_t> argvals;
        argvals.reserve(args.size());
        for (auto &a : args) argvals.push_back(a->eval(env));
        return env.callFunction(vn->name, argvals);
    } else {
        // evaluate callee: it could return a function object in a value wrapper (not implemented here)
        throw std::runtime_error("call_node: only simple function-name calls supported right now");
    }
}

// binary_op_node
ti::valptr_t binary_op_node::eval(ti::runtime_env &env) {
    auto l = left->eval(env);
    auto r = right->eval(env);
    if (!l || !r) throw std::runtime_error("binary op on null");

    // naive runtime type handling: try numeric addition/subtraction on decimals/integers
    // You should add proper visitor/variant logic in your value classes. Below is a minimal example.
    // Assume you have a decimal/integer types that expose toString() and numeric getters.

    // For demo: produce a string value of concatenated toStrings for Add
    if (op == binary_op::Add) {
        // create a string value containing concatenation (replace with numeric add)
        return std::make_shared<ti::string>(l->toString() + r->toString());
    } else {
        // not implemented
        throw std::runtime_error("binary op not implemented");
    }
}
