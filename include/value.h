#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <iostream>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <sstream>
#include <memory>
#include <variant>

#include "bigint.h"

namespace ti {

// Forward declarations
class fraction;
class decimal;

class value {
public:
    value() = default;
    virtual ~value() = default;
    virtual std::string toString() const;
    
    friend std::ostream& operator<<(std::ostream& os, const value& obj);
};

using valptr_t = std::shared_ptr<ti::value>;

class void_t : public value {
public:
    void_t() = default;
    ~void_t() override = default;

    std::string toString() const override;
};

inline const valptr_t none = std::make_shared<ti::void_t>();


class integer : public value {
private:
    BigInt value;

public:
    explicit integer(long long value);
    explicit integer(BigInt value);
    ~integer() override = default;

    BigInt getValue() const;
    std::string toString() const override;
};

class decimal : public value {
private:
    double value;
public:
    explicit decimal(double value);
    explicit decimal(const fraction& value);
    ~decimal() override = default;

    double getValue() const;
    fraction toFraction(double precision = 5e-16, int max_cycles = 100);
    fraction toFraction(const decimal& precision, int max_cycles = 100);
    std::string toString() const override;
};

class fraction : public value {
private:
    integer numerator;
    integer denominator;
public:
    fraction(const integer& numerator, const integer& denominator);
    fraction(long long numerator, long long denominator);
    ~fraction() override = default;

    integer getNumerator() const;
    integer getDenominator() const;
    double getValue() const;
    decimal getDecimalValue() const;
    std::tuple<BigInt, BigInt> toTuple() const;
    std::string toString() const override;
};

class string : public value {
private:
    std::string value;
public:
    string(const std::string& s);
    
    std::string getValue() const;
    std::string toString() const override;
};

template<typename T> 
struct _is_2d_vector : std::false_type {};

template<typename InnerT> 
struct _is_2d_vector<std::vector<std::vector<InnerT>>> : std::true_type {};

template<typename T> 
inline constexpr bool is_2d_vector_v = _is_2d_vector<T>::value;


// === collection base ===
template<typename T>
class collection : public value {
protected:
    std::vector<T> values;

public:
    collection() = default;
    explicit collection(const std::vector<T>& values) : values(values) {}
    collection(const collection<T>& other) = default;
    virtual ~collection() = default;

    T operator[](size_t i) const { return values[i]; }
    std::vector<T> getValues() const { return values; }
    size_t size() const { return values.size(); }

    // subclasses must override formatting
    virtual std::string toString() const override = 0;
};


// Type trait to detect if a type is a std::variant 
template<typename T> struct is_variant : std::false_type {}; 
template<typename... Ts> struct is_variant<std::variant<Ts...>> : std::true_type {}; template<typename T> inline constexpr bool is_variant_v = is_variant<T>::value;

// === list ===
template<typename T>
class list : public collection<T> {
public:
    using collection<T>::collection; // inherit constructors
    list(const list<T>& other) = default;

    std::string toString() const override {
        std::stringstream ss;
        ss << "{";
        for (size_t i = 0; i < this->values.size(); i++) {
            const auto& v = this->values[i];
            if constexpr (std::is_same_v<T, std::shared_ptr<value>>) {
                ss << v->toString();
            } else if constexpr (is_variant_v<T>) {
                ss << std::visit([](const auto& arg) -> std::string {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::shared_ptr<value>>) {
                        return arg->toString();
                    } else {
                        return arg.toString();
                    }
                }, v);
            } else {
                ss << v.toString();
            }
            if (i != this->values.size() - 1) ss << ", ";
        }
        ss << "}";
        return ss.str();
    }
};

// === vector ===
template<typename T>
class vector : public collection<T> {
public:
    using collection<T>::collection;
    vector(const vector<T>& other) = default;
    vector(const list<T>& l) : collection<T>(l.getValues()) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < this->values.size(); i++) {
            const auto& v = this->values[i];
            if constexpr (std::is_same_v<T, std::shared_ptr<value>>) {
                ss << v->toString();
            } else if constexpr (is_variant_v<T>) {
                ss << std::visit([](const auto& arg) -> std::string {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::shared_ptr<value>>) {
                        return arg->toString();
                    } else {
                        return arg.toString();
                    }
                }, v);
            } else {
                ss << v.toString();
            }
            if (i != this->values.size() - 1) ss << " ";
        }
        ss << "]";
        return ss.str();
    }
};


using matrix = list<list<std::shared_ptr<value>>>;

#if 0
class function {
private:
    std::vector<std::string> params; // hmm, why std::string inside?
    std::shared_ptr<
public:

};

#endif
} // namespace ti

#endif // VALUE_H