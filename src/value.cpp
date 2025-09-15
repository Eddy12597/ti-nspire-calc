#include "../include/value.h"
#include <sstream>
#include <iostream>
#include <variant>
#include "../include/colors.h"

// Type trait to detect if a type is a std::variant
template<typename T>
struct is_variant : std::false_type {};

template<typename... Ts>
struct is_variant<std::variant<Ts...>> : std::true_type {};

template<typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;

namespace ti {

// === value implementation ===
std::ostream& operator<<(std::ostream& os, const value& obj) {
    return os << obj.toString();
}

// === integer implementation ===
// TODO: refactor to bigint (more than long long)
integer::integer(long long value) : value(value) {}

long long integer::getValue() const {
    return value;
}

std::string integer::toString() const {
    return "ti::integer(" + std::to_string(value) + ")";
}

// === decimal implementation ===
decimal::decimal(double value) : value(value) {}

decimal::decimal(const fraction& frac) : value(frac.getValue()) {}

double decimal::getValue() const {
    return value;
}

std::string decimal::toString() const {
    return "ti::decimal(" + std::to_string(value) + ")";
}

// === fraction implementation ===
fraction::fraction(const integer& numerator, const integer& denominator)
    : numerator(numerator), denominator(denominator) {}

fraction::fraction(long long numerator, long long denominator)
    : numerator(numerator), denominator(denominator) {}

integer fraction::getNumerator() const {
    return numerator;
}

integer fraction::getDenominator() const {
    return denominator;
}

double fraction::getValue() const {
    return static_cast<double>(numerator.getValue()) / denominator.getValue();
}

decimal fraction::getDecimalValue() const {
    return decimal(getValue());
}

std::tuple<long long, long long> fraction::toTuple() const {
    return std::make_tuple(numerator.getValue(), denominator.getValue());
}

std::string fraction::toString() const {
    return "ti::fraction(" + std::to_string(numerator.getValue()) + ", " + 
           std::to_string(denominator.getValue()) + ")";
}

// str implementation
string::string(const std::string& s) : value(s) {}

std::string string::getValue() const {
    return value;
}

std::string string::toString() const {
    return "ti::string(\"" + value + "\")";
}

// list implementation (for non-template methods)
template<typename T>
list<T>::list(const std::vector<T>& values) : values(values) {
    if constexpr(is_2d_vector_v<T>) {
        std::cerr << col::red << "[Warning] list<list<list>> not allowed on TI Nspire (throws 'Invalid list or matrix' error)" << col::RESET << std::endl;
    }
}

template<typename T>
list<T>::list(const list<T>& other) : values(other.values) {}

template<typename T>
T list<T>::operator[](size_t i) const {
    return values[i];
}

template<typename T>
std::vector<T> list<T>::getValues() const {
    return values;
}

template<typename T>
size_t list<T>::size() const {
    return values.size();
}

template<typename T>
std::string list<T>::toString() const {
    std::stringstream ss;
    ss << "ti::list({";
    for (size_t i = 0; i < values.size(); i++) {
        if constexpr (std::is_same_v<T, std::shared_ptr<value>>) {
            ss << values[i]->toString();
        } else if constexpr (is_variant_v<T>) {
            ss << std::visit([](const auto& arg) -> std::string {
                if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::shared_ptr<value>>) {
                    return arg->toString();
                } else {
                    return arg.toString();
                }
            }, values[i]);
        } else {
            // Handle regular types
            ss << values[i].toString();
        }
        if (i != values.size() - 1) {
            ss << ", ";
        }
    }
    ss << "})";
    return ss.str();
}

// Explicit template instantiation for common types
template class list<integer>;
template class list<decimal>;
template class list<fraction>;
template class list<string>;
template class list<std::shared_ptr<value>>;

template class list<
    std::variant<
        integer,
        decimal, 
        fraction, 
        string, 
        std::shared_ptr<value>, 
        list<
            std::variant<
                integer, 
                decimal, 
                fraction, 
                string, 
                std::shared_ptr<value>
            >
        >
    >
>;

} // namespace ti