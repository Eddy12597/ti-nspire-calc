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

std::string value::toString() const {
    return "<value>";
}

// === void_t implementation ===
std::string void_t::toString() const {
    return "Done";
}

// === integer implementation ===
integer::integer(long long value) : value(value) {}

BigInt integer::getValue() const {
    return value;
}

std::string integer::toString() const {
    return value.to_string();
}

// === decimal implementation ===
decimal::decimal(double value) : value(value) {}

decimal::decimal(const fraction& frac) : value(frac.getValue()) {}

double decimal::getValue() const {
    return value;
}

std::string decimal::toString() const {
    return std::to_string(value);
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
    return static_cast<double>(numerator.getValue().to_long_long()) / denominator.getValue().to_long_long();
}

decimal fraction::getDecimalValue() const {
    return decimal(getValue());
}

std::tuple<BigInt, BigInt> fraction::toTuple() const {
    return std::make_tuple(numerator.getValue(), denominator.getValue());
}

std::string fraction::toString() const {
    return "(" + numerator.getValue().to_string() + ") / (" + denominator.getValue().to_string() + ")";
}

// === string implementation ===
string::string(const std::string& s) : value(s) {}

std::string string::getValue() const {
    return value;
}

std::string string::toString() const {
    return "\"" + value + "\"";
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