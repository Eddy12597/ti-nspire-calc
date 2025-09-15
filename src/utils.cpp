#include "../include/value.h"
#include <cmath>

namespace ti {

fraction decimal::toFraction(double precision, int max_cycles) {
    if (std::isnan(value)) throw std::invalid_argument("Cannot convert NaN to fraction");
    if (std::isinf(value)) throw std::invalid_argument("Cannot convert infinity to fraction");
    if (std::abs(value) < std::numeric_limits<double>::min())  
        return fraction(0LL, 1LL); 
    
    const int sign = (value < 0) ? -1 : 1;
    double abs_number = std::abs(value);
    
    double integral_part;
    double fractional_part = std::modf(abs_number, &integral_part);
    
    long long numerator_prev = 1;
    long long denominator_prev = 0;
    long long numerator_curr = static_cast<long long>(integral_part);
    long long denominator_curr = 1;
    
    double error = std::abs(static_cast<double>(numerator_curr) / denominator_curr - abs_number);
    
    if (error <= precision || fractional_part == 0.0) {
        return fraction(sign * numerator_curr, denominator_curr);
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
    
    return fraction(sign * numerator_curr, denominator_curr);
}

fraction decimal::toFraction(const decimal& precision, int max_cycles) {
    return toFraction(precision.getValue(), max_cycles);
}

} // namespace ti