#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <iostream>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <sstream>
#include <memory>

namespace ti {

// Forward declarations
class fraction;
class decimal;

class value {
public:
    virtual ~value() = default;
    virtual std::string toString() const;
    
    friend std::ostream& operator<<(std::ostream& os, const value& obj);
};

// TODO: refactor to bigint (more than long long)
class integer : public value {
private:
    long long value;

public:
    explicit integer(long long value);
    ~integer() override = default;

    long long getValue() const;
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
    std::tuple<long long, long long> toTuple() const;
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

template<typename T = value> 
class list : public value {
private:
    std::vector<T> values;
public:
    list(const std::vector<T>& values);
    list(const list<T>& values);
    
    T operator[](size_t i) const;
    std::vector<T> getValues() const;
    size_t size() const;
    
    std::string toString() const override;
};

using matrix = list<list<std::shared_ptr<value>>>;
using vptr = std::shared_ptr<ti::value>;

#if 0
class function {
private:
    std::vector<std::string> params; // hmm, why std::string inside?
    std::shared_ptr<
public:

};

#endif
} // namespace ti

#endif // VARIABLE_H