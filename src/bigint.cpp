#include "../include/bigint.h"

// Constructors
BigInt::BigInt() : is_negative(false) {
    digits.push_back(0);
}

BigInt::BigInt(long long num) : is_negative(num < 0) {
    if (num == 0) {
        digits.push_back(0);
        return;
    }
    
    unsigned long long abs_num = std::abs(num);
    while (abs_num > 0) {
        digits.push_back(abs_num % BASE);
        abs_num /= BASE;
    }
}

void BigInt::normalize() {
    // Remove leading zeros
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    
    // Handle the case of -0
    if (digits.size() == 1 && digits[0] == 0) {
        is_negative = false;
    }
}

BigInt::BigInt(const std::string& str) : is_negative(false) {
    if (str.empty()) {
        digits.push_back(0);
        return;
    }
    
    size_t start = 0;
    if (str[0] == '-') {
        is_negative = true;
        start = 1;
    } else if (str[0] == '+') {
        start = 1;
    }
    
    // Check for valid input
    for (size_t i = start; i < str.size(); ++i) {
        if (!isdigit(str[i])) {
            throw std::invalid_argument("Invalid character in BigInt string");
        }
    }
    
    // Handle very large numbers efficiently
    size_t total_digits = str.size() - start;
    digits.reserve((total_digits + BASE_DIGITS - 1) / BASE_DIGITS);
    
    for (size_t i = str.size(); i > start; i -= BASE_DIGITS) {
        int chunk_start = std::max(static_cast<int>(start), static_cast<int>(i) - BASE_DIGITS);
        std::string chunk = str.substr(chunk_start, i - chunk_start);
        digits.push_back(std::stoi(chunk));
    }
    
    normalize();
}

BigInt::BigInt(const BigInt& other) 
    : digits(other.digits), is_negative(other.is_negative) {}

// Assignment operators
BigInt& BigInt::operator=(const BigInt& other) {
    if (this != &other) {
        digits = other.digits;
        is_negative = other.is_negative;
    }
    return *this;
}

BigInt& BigInt::operator=(long long num) {
    *this = BigInt(num);
    return *this;
}

// Unary operators
BigInt BigInt::operator-() const {
    BigInt result(*this);
    if (!result.is_zero()) {
        result.is_negative = !result.is_negative;
    }
    return result;
}

BigInt BigInt::operator+() const {
    return *this;
}

// Arithmetic operators
BigInt BigInt::operator+(const BigInt& other) const {
    if (is_negative == other.is_negative) {
        BigInt result;
        result.is_negative = is_negative;
        
        size_t max_size = std::max(digits.size(), other.digits.size());
        result.digits.resize(max_size, 0);
        
        int carry = 0;
        for (size_t i = 0; i < max_size; ++i) {
            int sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < other.digits.size()) sum += other.digits[i];
            
            result.digits[i] = sum % BASE;
            carry = sum / BASE;
        }
        
        if (carry > 0) {
            result.digits.push_back(carry);
        }
        
        result.normalize();
        return result;
    } else {
        if (is_negative) {
            return other - (-*this);
        } else {
            return *this - (-other);
        }
    }
}

int BigInt::compare_magnitude(const BigInt& other) const {
    // First compare by number of digits
    if (digits.size() < other.digits.size()) {
        return -1;
    } else if (digits.size() > other.digits.size()) {
        return 1;
    }
    
    // If same number of digits, compare digit by digit from most significant to least
    for (int i = digits.size() - 1; i >= 0; --i) {
        if (digits[i] < other.digits[i]) {
            return -1;
        } else if (digits[i] > other.digits[i]) {
            return 1;
        }
    }
    
    // Numbers are equal in magnitude
    return 0;
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (is_negative != other.is_negative) {
        return *this + (-other);
    }
    
    if (compare_magnitude(other) < 0) {
        BigInt result = other - *this;
        result.is_negative = !is_negative;
        return result;
    }
    
    BigInt result;
    result.is_negative = is_negative;
    result.digits.resize(digits.size(), 0);
    
    int borrow = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        int diff = digits[i] - borrow;
        if (i < other.digits.size()) {
            diff -= other.digits[i];
        }
        
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        result.digits[i] = diff;
    }
    
    result.normalize();
    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result;
    result.is_negative = is_negative != other.is_negative;
    result.digits.resize(digits.size() + other.digits.size(), 0);
    
    for (size_t i = 0; i < digits.size(); ++i) {
        long long carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry > 0; ++j) {
            long long product = result.digits[i + j] + carry + 
                               digits[i] * (j < other.digits.size() ? other.digits[j] : 0);
            result.digits[i + j] = product % BASE;
            carry = product / BASE;
        }
    }
    
    result.normalize();
    return result;
}

// Comparison operators
bool BigInt::operator==(const BigInt& other) const {
    return is_negative == other.is_negative && digits == other.digits;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
    if (is_negative != other.is_negative) {
        return is_negative;
    }
    
    if (is_negative) {
        return compare_magnitude(other) > 0;
    } else {
        return compare_magnitude(other) < 0;
    }
}

bool BigInt::operator<=(const BigInt& other) const {
    return *this < other || *this == other;
}

bool BigInt::operator>(const BigInt& other) const {
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

// Utility functions
std::string BigInt::to_string() const {
    if (is_zero()) {
        return "0";
    }
    
    std::string result;
    if (is_negative) {
        result += '-';
    }
    
    result += std::to_string(digits.back());
    
    for (int i = digits.size() - 2; i >= 0; --i) {
        std::string chunk = std::to_string(digits[i]);
        result += std::string(BASE_DIGITS - chunk.size(), '0') + chunk;
    }
    
    return result;
}

long long BigInt::to_long_long() const {
    long long result = 0;
    long long power = 1;
    
    for (size_t i = 0; i < digits.size(); ++i) {
        result += digits[i] * power;
        power *= BASE;
        
        // Check for overflow
        if (result < 0 && i < digits.size() - 1) {
            throw std::overflow_error("BigInt too large for long long");
        }
    }
    
    return is_negative ? -result : result;
}

bool BigInt::is_zero() const {
    return digits.size() == 1 && digits[0] == 0;
}

BigInt BigInt::abs() const {
    BigInt result(*this);
    result.is_negative = false;
    return result;
}

// Memory management
void BigInt::shrink_to_fit() {
    digits.shrink_to_fit();
}

size_t BigInt::memory_usage() const {
    return sizeof(*this) + digits.capacity() * sizeof(int);
}

// I/O operators
std::ostream& operator<<(std::ostream& os, const BigInt& num) {
    os << num.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, BigInt& num) {
    std::string str;
    is >> str;
    num = BigInt(str);
    return is;
}