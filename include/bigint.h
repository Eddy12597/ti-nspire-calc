#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <climits>

class BigInt {
private:
    static const int BASE = 1000000000; // 10^9 base for efficient storage
    static const int BASE_DIGITS = 9;
    
    std::vector<int> digits;
    bool is_negative;
    
    // Helper functions
    void remove_leading_zeros();
    void normalize();
    int compare_magnitude(const BigInt& other) const;
    
public:
    // Constructors
    BigInt();
    BigInt(long long num);
    BigInt(const std::string& str);
    BigInt(const BigInt& other);
    
    // Assignment operators
    BigInt& operator=(const BigInt& other);
    BigInt& operator=(long long num);
    
    // Unary operators
    BigInt operator-() const;
    BigInt operator+() const;
    
    // Arithmetic operators
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;
    
    // Compound assignment operators
    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator%=(const BigInt& other);
    
    // Comparison operators
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;
    
    // Input/output
    friend std::ostream& operator<<(std::ostream& os, const BigInt& num);
    friend std::istream& operator>>(std::istream& is, BigInt& num);
    
    // Utility functions
    std::string to_string() const;
    long long to_long_long() const;
    bool is_zero() const;
    BigInt abs() const;
    
    // Memory efficient functions
    void shrink_to_fit();
    size_t memory_usage() const;
};

#endif // BIGINT_H