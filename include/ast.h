#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

#include "value.h" // must define ti::valptr_t

// forward-declare runtime_env to avoid cycles
namespace ti { class runtime_env; }

namespace ast {

struct node {
    virtual ~node() = default;
};

struct exprnode : node {
    // evaluate an expression in the given runtime environment
    virtual ti::valptr_t eval(ti::runtime_env &env) = 0;
};

// Concrete nodes
struct literal_node : exprnode {
    ti::valptr_t val;
    literal_node(const ti::valptr_t &v) : val(v) {}
    ti::valptr_t eval(ti::runtime_env & /*env*/) override { return val; }
};

struct var_node : exprnode {
    std::string name;
    var_node(std::string n) : name(std::move(n)) {}
    ti::valptr_t eval(ti::runtime_env &env) override;
};

struct assign_node : exprnode {
    std::string name;
    std::unique_ptr<exprnode> rhs;
    assign_node(std::string n, std::unique_ptr<exprnode> r)
        : name(std::move(n)), rhs(std::move(r)) {}
    ti::valptr_t eval(ti::runtime_env &env) override;
};

struct call_node : exprnode {
    std::unique_ptr<exprnode> callee; // either var_node (function name) or expression returning function
    std::vector<std::unique_ptr<exprnode>> args;
    call_node(std::unique_ptr<exprnode> c, std::vector<std::unique_ptr<exprnode>> a)
        : callee(std::move(c)), args(std::move(a)) {}
    ti::valptr_t eval(ti::runtime_env &env) override;
};

enum class binary_op {
    Add,
    Sub,
    Mul,
    Div,
    // extend as needed
};

struct binary_op_node : exprnode {
    binary_op op;
    std::unique_ptr<exprnode> left;
    std::unique_ptr<exprnode> right;
    binary_op_node(binary_op op_, std::unique_ptr<exprnode> l, std::unique_ptr<exprnode> r)
        : op(op_), left(std::move(l)), right(std::move(r)) {}
    ti::valptr_t eval(ti::runtime_env &env) override;
};

} // namespace ast

#endif // AST_H
