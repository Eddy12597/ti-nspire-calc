#ifndef OBJECT_H
#define OBJECT_H

/**
 * object
 * integer
 * decimal
 * fraction
 * str
 * list
 * vector
 * matrix
 */

#include <string>
#include <any>
#include <iostream>
#include <valarray>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <sstream>

namespace ti {

class object {
public:
    friend std::ostream& operator<< (std::ostream& os, ti::object obj);
};

class integer : object {

private:
    long long value;

public:

    integer(long long value) : value(value){}
    ~integer() = default;

    long long getValue() {
        return this->value;
    }

    friend std::ostream& operator<< (std::ostream& os, ti::integer& intg) {
        return os << intg.getValue();
    }

};

class fraction;

class decimal : object {
private:
    double value;
public:
    decimal(double value) : value(value) {}
    ~decimal() = default;

    double getValue() {
        return this->value;
    }


    ti::fraction toFraction(const double number, const double precision = 5e-16, const int max_cycles = 100) {
        if (std::isnan(number)) throw std::invalid_argument("Cannot convert NaN to fraction");
        if (std::isinf(number)) throw std::invalid_argument("Cannot convert infinity to fraction");
        if (std::abs(number) < std::numeric_limits<double>::min())  return ti::fraction(0LL, 1LL); 
        
        
        const int sign = (number < 0) ? -1 : 1;
        double abs_number = std::abs(number);
        
        double integral_part;
        double fractional_part = std::modf(abs_number, &integral_part);
        
        long long numerator_prev = 1;
        long long denominator_prev = 0;
        long long numerator_curr = static_cast<long long>(integral_part);
        long long denominator_curr = 1;
        
        double error = std::abs(static_cast<double>(numerator_curr) / denominator_curr - abs_number);
        
        if (error <= precision || fractional_part == 0.0) {
            return ti::fraction(sign * numerator_curr, denominator_curr);
        }
        
        double x = fractional_part;
        
        for (int i = 0; i < max_cycles; ++i) {
            if (std::abs(x) < std::numeric_limits<double>::min()) {
                break;
            }
            
            x = 1.0 / x;
            double a_double;
            x = std::modf(x, &a_double);
            long long a = static_cast<long long>(a_double);
            
            long long numerator_next = a * numerator_curr + numerator_prev;
            long long denominator_next = a * denominator_curr + denominator_prev;
            
            if (numerator_next < numerator_curr || denominator_next < denominator_curr) {
                break; 
            }
            
            numerator_prev = numerator_curr;
            denominator_prev = denominator_curr;
            numerator_curr = numerator_next;
            denominator_curr = denominator_next;
            
            double current_value = static_cast<double>(numerator_curr) / denominator_curr;
            error = std::abs(current_value - abs_number);
            
            if (error <= precision) {
                break;
            }
        }
        
        return ti::fraction(sign * numerator_curr, denominator_curr);
    }

    friend std::ostream& operator<< (std::ostream& os, ti::decimal dec) {
        return os << dec.getValue();
    }
};

class fraction : object {
private:
    ti::integer numerator;
    ti::integer denominator;
public:
    // accept int and ti::integer
    fraction (const ti::integer& numerator, const ti::integer& denominator)
        : numerator(numerator), denominator(denominator) {}
    fraction (const long long& numerator, const long long& denominator)
        : numerator(ti::integer(numerator)), denominator(ti::integer(denominator)) {}
    
    ~fraction() = default;

    

    // gets C++ double value
    double getDoubleValue() {
        return static_cast<double>(numerator.getValue()) / denominator.getValue();
    }

    // gets ti::decimal value
    ti::decimal getDecimalValue() {
        return ti::decimal(static_cast<double>(numerator.getValue()) / denominator.getValue());
    }

    std::tuple<long long, long long> toTuple() {
        return std::make_tuple(this->numerator.getValue(), this->denominator.getValue());
    }

    friend std::ostream& operator<< (std::ostream& os, ti::fraction fr) {
        return os << fr.numerator << " / " << fr.denominator;
    }
    
};

class str : object {
private:
    std::string value;
public:
    str(const std::string& s) : value(value) {}

    std::string getValue() {
        return this->value;
    }

    friend std::ostream& operator<< (std::ostream& os, ti::str s) {
        return os << s.getValue();
    }
};

template<typename T> struct _is_2d_vector : std::false_type {};
template<typename InnerT> struct _is_2d_vector<std::vector<std::vector<InnerT>>> : std::true_type {};

template<typename T> inline constexpr bool is_2d_vector_v = _is_2d_vector<T>::value;

template<typename T = ti::object> class list : object {
private:
    std::vector<T> values;
public:

    T operator[] (const size_t i) {
        return values[i];
    }

    list(const std::vector<T>& values) {
        if constexpr(is_2d_vector_v(T)) {
            std::cerr << "[Warning] list<list<list>> not allowed on TI Nspire (throws 'Invalid list or matrix' error)" << std::endl;
        }
        const size_t s = values.size();
        this->values.reserve(s);
        for (size_t i = 0; i < s; i++) {
            this->values.emplace_back(values[i]);
        }
    }
    list(const ti::list<T>& values) {
        if constexpr(is_2d_vector_v(T)) {
            std::cerr << "[Warning] list<list<list>> not allowed on TI Nspire (throws 'Invalid list or matrix' error)" << std::endl;
        }
        const size_t s = values.size();
        this->values.reserve(s);
        for (size_t i = 0; i < s; i++) {
            this->values.emplace_back(values[i]);
        }
    }

    std::vector<T> getValues() {
        return values;
    }

    std::string toVectorString() {
        ss::stringstream str();
        str << "["
        for (size_t i = 0; i < this->values.size(); i++) {
            ss << this->values[i] << (i == this->values.size() - 1 ? " " : "");
        }
        ss << "]"
        return ss.str();
    }

};

typedef ti::list<ti::list<ti::object>> matrix;

};
#endif // OBJECT_H