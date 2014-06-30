// ------------------
// Purenum 0.4e alpha
// ------------------
// integer_gmp.h
// ------------------

// see file: "integer.txt" for more info about class Integer

// This version of class Integer only works on systems supplying the GMP
// library (the "GNU Multi-Precision" library).  If you need a more portable
// bignum class, try the regular class Integer from 'integer.h'.

// It is hoped that this version of class Integer will be useful for comparing
// differences in speed and correctness between Integer and GMP.


#ifndef INTEGER_GMP_H
#define INTEGER_GMP_H


////////////////////////////////////////////////////////////////////////////////
// CONFIGURATION (must be reconfigured for each system this is compiled on!)
// CONFIGURATION (must be reconfigured for each system this is compiled on!)
// CONFIGURATION (must be reconfigured for each system this is compiled on!)
//
// INTEGER_H_DIGITTYPE --  GMP handles all atoms in integer_gmp, these
// typedefs are left here for class member function parameter compatibility.
// 
//#define INTEGER_H_DIGITTYPE unsigned char     // type of Integer_GMP::atom
//#define INTEGER_H_DIGITTYPE unsigned short    // type of Integer_GMP::atom
#define INTEGER_H_DIGITTYPE unsigned int        // type of Integer_GMP::atom
//#define INTEGER_H_DIGITTYPE unsigned long     // type of Integer_GMP::atom
//
//#define INTEGER_H_SDIGITTYPE signed char      // type of Integer_GMP::satom
//#define INTEGER_H_SDIGITTYPE signed short     // type of Integer_GMP::satom
#define INTEGER_H_SDIGITTYPE signed int         // type of Integer_GMP::satom
//#define INTEGER_H_SDIGITTYPE signed long      // type of Integer_GMP::satom
// 
////////////////////////////////////////////////////////////////////////////////


#ifndef PURENUM_STRING_CONSTRUCTOR_EXPLICIT
#define EX
#else    // PURENUM_STRING_CONSTRUCTOR_EXPLICIT
#define EX explicit
#endif    // PURENUM_STRING_CONSTRUCTOR_EXPLICIT


#include "gmp.h"

// FIXME: delete the following <iostream> include?
#include <iostream>

class Integer_GMP
{
public:
    typedef INTEGER_H_DIGITTYPE atom;
    typedef INTEGER_H_SDIGITTYPE satom;
    enum sign { positive, negative };

    inline Integer_GMP();                      // see INTEGER_H_INITZERO above
    inline Integer_GMP(sign, atom);            // init from a hardware number
    inline Integer_GMP(atom);                  // init from a hardware value
    inline Integer_GMP(satom);                 // init from a hardware value
    inline Integer_GMP(const Integer_GMP &);   // init from another Integer_GMP
EX  inline Integer_GMP(const char *ascii);     // init from a string number
    inline ~Integer_GMP();                     // destructor

    // conversion operators
    // FIXME: need signed int conversion operator
    inline operator bool() const;
    inline operator unsigned int() const;

    // unary math operators (members)
    inline Integer_GMP &operator++();      // prefix
    inline Integer_GMP operator++(int);    // postfix
    inline Integer_GMP &operator--();      // prefix
    inline Integer_GMP operator--(int);    // postfix

    // binary math operators (members)
    inline Integer_GMP &operator=(const Integer_GMP &);
    inline Integer_GMP &operator=(const atom &);
    inline Integer_GMP &operator=(const satom &);
    inline Integer_GMP &operator+=(const Integer_GMP &);
    inline Integer_GMP &operator+=(const atom &);
    inline Integer_GMP &operator+=(const satom &);
    inline Integer_GMP &operator-=(const Integer_GMP &);
    inline Integer_GMP &operator-=(const atom &);
    inline Integer_GMP &operator-=(const satom &);
    inline Integer_GMP &operator*=(const Integer_GMP &);
    inline Integer_GMP &operator*=(const atom &);
    inline Integer_GMP &operator*=(const satom &);
    inline Integer_GMP &operator/=(const Integer_GMP &);
    inline Integer_GMP &operator/=(const atom &);
    inline Integer_GMP &operator/=(const satom &);
    inline Integer_GMP &operator%=(const Integer_GMP &);
    inline Integer_GMP &operator%=(const atom &);
    inline Integer_GMP &operator%=(const satom &);

    // friends: unary math operators (global functions)
    friend Integer_GMP operator~(const Integer_GMP &);
    friend bool operator!(const Integer_GMP &);
    friend Integer_GMP operator-(const Integer_GMP &);
    friend Integer_GMP operator+(const Integer_GMP &);

    // friends: binary math operators (global functions)
    friend Integer_GMP operator+(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP operator+(const Integer_GMP &, const atom &);
    friend Integer_GMP operator+(const atom &, const Integer_GMP &);
    friend Integer_GMP operator+(const Integer_GMP &, const satom &);
    friend Integer_GMP operator+(const satom &, const Integer_GMP &);
    friend Integer_GMP operator-(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP operator-(const Integer_GMP &, const atom &);
    friend Integer_GMP operator-(const atom &, const Integer_GMP &);
    friend Integer_GMP operator-(const Integer_GMP &, const satom &);
    friend Integer_GMP operator-(const satom &, const Integer_GMP &);
    friend Integer_GMP operator*(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP operator*(const Integer_GMP &, const atom &);
    friend Integer_GMP operator*(const atom &, const Integer_GMP &);
    friend Integer_GMP operator*(const Integer_GMP &, const satom &);
    friend Integer_GMP operator*(const satom &, const Integer_GMP &);
    friend Integer_GMP operator/(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP operator/(const Integer_GMP &, const atom &);
    friend Integer_GMP operator/(const atom &, const Integer_GMP &);
    friend Integer_GMP operator/(const Integer_GMP &, const satom &);
    friend Integer_GMP operator/(const satom &, const Integer_GMP &);
    friend Integer_GMP operator%(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP operator%(const Integer_GMP &, const atom &);
    friend Integer_GMP operator%(const atom &, const Integer_GMP &);
    friend Integer_GMP operator%(const Integer_GMP &, const satom &);
    friend Integer_GMP operator%(const satom &, const Integer_GMP &);
    friend bool operator==(const Integer_GMP &, const Integer_GMP &);
    friend bool operator==(const Integer_GMP &, const atom &);
    friend bool operator==(const atom &, const Integer_GMP &);
    friend bool operator==(const Integer_GMP &, const satom &);
    friend bool operator==(const satom &, const Integer_GMP &);
    friend bool operator!=(const Integer_GMP &, const Integer_GMP &);
    friend bool operator!=(const Integer_GMP &, const atom &);
    friend bool operator!=(const atom &, const Integer_GMP &);
    friend bool operator!=(const Integer_GMP &, const satom &);
    friend bool operator!=(const satom &, const Integer_GMP &);
    friend bool operator>(const Integer_GMP &, const Integer_GMP &);
    friend bool operator>(const Integer_GMP &, const atom &);
    friend bool operator>(const atom &, const Integer_GMP &);
    friend bool operator>(const Integer_GMP &, const satom &);
    friend bool operator>(const satom &, const Integer_GMP &);
    friend bool operator>=(const Integer_GMP &, const Integer_GMP &);
    friend bool operator>=(const Integer_GMP &, const atom &);
    friend bool operator>=(const atom &, const Integer_GMP &);
    friend bool operator>=(const Integer_GMP &, const satom &);
    friend bool operator>=(const satom &, const Integer_GMP &);
    friend bool operator<(const Integer_GMP &, const Integer_GMP &);
    friend bool operator<(const Integer_GMP &, const atom &);
    friend bool operator<(const atom &, const Integer_GMP &);
    friend bool operator<(const Integer_GMP &, const satom &);
    friend bool operator<(const satom &, const Integer_GMP &);
    friend bool operator<=(const Integer_GMP &, const Integer_GMP &);
    friend bool operator<=(const Integer_GMP &, const atom &);
    friend bool operator<=(const atom &, const Integer_GMP &);
    friend bool operator<=(const Integer_GMP &, const satom &);
    friend bool operator<=(const satom &, const Integer_GMP &);
    friend bool operator&&(const Integer_GMP &, const Integer_GMP &);
    friend bool operator&&(const Integer_GMP &, const bool);
    friend bool operator&&(const bool, const Integer_GMP &);
    friend bool operator||(const Integer_GMP &, const Integer_GMP &);
    friend bool operator||(const Integer_GMP &, const bool);
    friend bool operator||(const bool, const Integer_GMP &);

    // FIXME: move these to a seperate optional file?
    // other/math functions (global functions)
    friend std::ostream &operator<<(std::ostream &, const Integer_GMP &);  // FIXME delete?
    friend void swap(Integer_GMP &, Integer_GMP &);
    friend Integer_GMP abs(const Integer_GMP &);
    friend Integer_GMP min(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP max(const Integer_GMP &, const Integer_GMP &);
    friend Integer_GMP fac(const Integer_GMP &);
    friend Integer_GMP pow(const Integer_GMP &, const Integer_GMP &);
    friend void divmod(const Integer_GMP &dividend, const Integer_GMP &divisor,
                       Integer_GMP &quotient, Integer_GMP &remainder);

    // exceptions
    // FIXME: throw outofmemory when 'new' fails?  or not?
    class exception { };
    class divisionbyzero : public exception { };   // division by zero
    class outofmemory : public exception { };      // software integer overflow
    class outofrange : public exception { };       // hardware integer overflow
    class badstring : public exception { };        // non-numeric initializer
    class negativefactorial : public exception {}; // can't do x! for x < 0

    // ascii string input and output           FIXME: not internationalized
    char *ascii(const Integer_GMP base = 10) const;                // out new[]
    void ascii(atom size, char *, const Integer_GMP base = 10) const; // output
    void ascii(const char *, const Integer_GMP base = 10);            // input

#if 0
    // string input and output, digits are zero-based (no character set)
    char *string(atom base = 10) const;                      // output to new[]
    void string(atom size, char *, atom base = 10) const;    // buffer output
    void string(const char *, atom base = 10);               // buffer input

    // bitstream input and output, a string() with a header and a footer
    atom *bitstream(atom base = maxatom_) const;
    void bitstream(atom size, atom *a, atom base = maxatom_) const;
    void bitstream(const atom *a, atom base = maxatom_);
#endif    // 0

private:
    // internal representation of the integer value (this is the entire state)
    mpz_t value_;
};

//#if 0
// functions to help measure code speed (tricks compiler into not optimizing)
void touch(const Integer_GMP &);          // empty function, for testing only
void touch(const Integer_GMP::atom &);    // empty function, for testing only
//#endif    // 0

// unary math operators (global functions)
inline Integer_GMP operator~(const Integer_GMP &left);
inline bool operator!(const Integer_GMP &left);
inline Integer_GMP operator-(const Integer_GMP &left);
inline Integer_GMP operator+(const Integer_GMP &left);

// binary math operators (global functions)
inline Integer_GMP operator+(const Integer_GMP &left, const Integer_GMP &right);
inline Integer_GMP operator+(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline Integer_GMP operator+(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator+(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline Integer_GMP operator+(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator-(const Integer_GMP &left, const Integer_GMP &right);
inline Integer_GMP operator-(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline Integer_GMP operator-(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator-(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline Integer_GMP operator-(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator*(const Integer_GMP &left, const Integer_GMP &right);
inline Integer_GMP operator*(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline Integer_GMP operator*(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator*(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline Integer_GMP operator*(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator/(const Integer_GMP &left, const Integer_GMP &right);
inline Integer_GMP operator/(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline Integer_GMP operator/(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator/(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline Integer_GMP operator/(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator%(const Integer_GMP &left, const Integer_GMP &right);
inline Integer_GMP operator%(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline Integer_GMP operator%(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline Integer_GMP operator%(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline Integer_GMP operator%(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator==(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator==(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline bool operator==(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline bool operator==(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline bool operator==(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator!=(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator!=(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline bool operator!=(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline bool operator!=(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline bool operator!=(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator>(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator>(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline bool operator>(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline bool operator>(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline bool operator>(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator>=(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator>=(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline bool operator>=(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline bool operator>=(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline bool operator>=(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator<(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator<(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline bool operator<(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline bool operator<(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline bool operator<(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator<=(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator<=(const Integer_GMP &left, const Integer_GMP::atom &rightatom);
inline bool operator<=(const Integer_GMP::atom &leftatom, const Integer_GMP &right);
inline bool operator<=(const Integer_GMP &left, const Integer_GMP::satom &rightatom);
inline bool operator<=(const Integer_GMP::satom &leftatom, const Integer_GMP &right);
inline bool operator&&(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator&&(const Integer_GMP &left, const bool right);
inline bool operator&&(const bool left, const Integer_GMP &right);
inline bool operator||(const Integer_GMP &left, const Integer_GMP &right);
inline bool operator||(const Integer_GMP &left, const bool right);
inline bool operator||(const bool left, const Integer_GMP &right);
inline std::ostream &operator<<(std::ostream &, const Integer_GMP &);
inline void swap(Integer_GMP &, Integer_GMP &);
inline Integer_GMP abs(const Integer_GMP &);
inline Integer_GMP min(const Integer_GMP &, const Integer_GMP &);
inline Integer_GMP max(const Integer_GMP &, const Integer_GMP &);
inline Integer_GMP fac(const Integer_GMP &);
inline Integer_GMP pow(const Integer_GMP &, const Integer_GMP &);
inline void divmod(const Integer_GMP &dividend, const Integer_GMP &divisor,
                   Integer_GMP &quotient, Integer_GMP &remainder);

inline
Integer_GMP::Integer_GMP()
{
    mpz_init(value_);
}

inline
Integer_GMP::Integer_GMP(Integer_GMP::sign s, Integer_GMP::atom a)
{
    mpz_init_set_ui(value_,(unsigned long int)a);
    if (s == negative) mpz_neg(value_,value_);
}

inline
Integer_GMP::Integer_GMP(Integer_GMP::atom a)
{
    mpz_init_set_ui(value_,(unsigned long int)a);
}

inline
Integer_GMP::Integer_GMP(Integer_GMP::satom i)
{
    mpz_init_set_si(value_,(signed long int)i);
}

inline
Integer_GMP::Integer_GMP(const Integer_GMP &other)
{
    mpz_init_set(value_,other.value_);
}

inline
Integer_GMP::~Integer_GMP()
{
    mpz_clear(value_);
}

inline
Integer_GMP::operator bool() const
{
    if (mpz_sgn(value_) != 0)
        return true;
    else
        return false;
}

inline
Integer_GMP::operator unsigned int() const
{
    unsigned int i = (unsigned int)mpz_get_ui(value_);
    Integer_GMP I = i;
    if (mpz_cmp(I.value_,this->value_) != 0) throw outofrange();
    return i;
}

inline Integer_GMP &
Integer_GMP::operator++()    // prefix
{
    mpz_add_ui(value_,value_,1);
    return *this;
}

inline Integer_GMP
Integer_GMP::operator++(int)    // postfix
{
    Integer_GMP result = *this;
    this->operator++();
    return result;
}

inline Integer_GMP &
Integer_GMP::operator--()    // prefix
{
    mpz_sub_ui(value_,value_,1);
    return *this;
}

inline Integer_GMP
Integer_GMP::operator--(int)    // postfix
{
    Integer_GMP result = *this;
    this->operator--();
    return result;
}

inline Integer_GMP &
Integer_GMP::operator=(const Integer_GMP &right)
{
    mpz_set(value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator=(const atom &rightatom)
{
    mpz_set_ui(value_,(unsigned long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator=(const satom &rightatom)
{
    mpz_set_si(value_,(signed long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator+=(const Integer_GMP &right)
{
    mpz_add(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator+=(const atom &rightatom)
{
    mpz_add_ui(value_,value_,(unsigned long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator+=(const satom &rightatom)
{
    Integer_GMP right = rightatom;
    mpz_add(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator-=(const Integer_GMP &right)
{
    mpz_sub(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator-=(const atom &rightatom)
{
    mpz_sub_ui(value_,value_,(unsigned long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator-=(const satom &rightatom)
{
    Integer_GMP right = rightatom;
    mpz_sub(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator*=(const Integer_GMP &right)
{
    mpz_mul(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator*=(const atom &rightatom)
{
    mpz_mul_ui(value_,value_,(unsigned long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator*=(const satom &rightatom)
{
    Integer_GMP right = rightatom;
    mpz_mul(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator/=(const Integer_GMP &right)
{
    mpz_tdiv_q(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator/=(const atom &rightatom)
{
    mpz_tdiv_q_ui(value_,value_,(unsigned long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator/=(const satom &rightatom)
{
    Integer_GMP right = rightatom;
    mpz_tdiv_q(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator%=(const Integer_GMP &right)
{
    mpz_mod(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator%=(const atom &rightatom)
{
    mpz_mod_ui(value_,value_,(unsigned long int)rightatom);
    return *this;
}

inline Integer_GMP &
Integer_GMP::operator%=(const satom &rightatom)
{
    Integer_GMP right = rightatom;
    mpz_mod(value_,value_,right.value_);
    return *this;
}

inline Integer_GMP
operator~(const Integer_GMP &left)    // FIXME see integer.txt complement info
{
    Integer_GMP result = left;
    mpz_com(result.value_,result.value_);
    return result;
}

inline bool
operator!(const Integer_GMP &left)
{
    if (mpz_sgn(left.value_) == 0)
        return true;
    else
        return false;
}

inline Integer_GMP
operator-(const Integer_GMP &left)
{
    Integer_GMP result = left;
    mpz_neg(result.value_,result.value_);
    return result;
}

inline Integer_GMP
operator+(const Integer_GMP &left)
{
    Integer_GMP result = left;
    return result;
}

inline Integer_GMP
operator+(const Integer_GMP &left, const Integer_GMP &right)
{
    Integer_GMP result = left;
    result += right;
    return result;
}

inline Integer_GMP
operator+(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    Integer_GMP result = left;
    result += rightatom;
    return result;
}

inline Integer_GMP
operator+(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result += right;
    return result;
}

inline Integer_GMP
operator+(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    Integer_GMP result = left;
    result += rightatom;
    return result;
}

inline Integer_GMP
operator+(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result += right;
    return result;
}

inline Integer_GMP
operator-(const Integer_GMP &left, const Integer_GMP &right)
{
    Integer_GMP result = left;
    result -= right;
    return result;
}

inline Integer_GMP
operator-(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    Integer_GMP result = left;
    result -= rightatom;
    return result;
}

inline Integer_GMP
operator-(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result -= right;
    return result;
}

inline Integer_GMP
operator-(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    Integer_GMP result = left;
    result -= rightatom;
    return result;
}

inline Integer_GMP
operator-(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result -= right;
    return result;
}

inline Integer_GMP
operator*(const Integer_GMP &left, const Integer_GMP &right)
{
    Integer_GMP result = left;
    result *= right;
    return result;
}

inline Integer_GMP
operator*(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    Integer_GMP result = left;
    result *= rightatom;
    return result;
}

inline Integer_GMP
operator*(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result *= right;
    return result;
}

inline Integer_GMP
operator*(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    Integer_GMP result = left;
    result *= rightatom;
    return result;
}

inline Integer_GMP
operator*(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result *= right;
    return result;
}

inline Integer_GMP
operator/(const Integer_GMP &left, const Integer_GMP &right)
{
    Integer_GMP result = left;
    result /= right;
    return result;
}

inline Integer_GMP
operator/(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    Integer_GMP result = left;
    result /= rightatom;
    return result;
}

inline Integer_GMP
operator/(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result /= right;
    return result;
}

inline Integer_GMP
operator/(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    Integer_GMP result = left;
    result /= rightatom;
    return result;
}

inline Integer_GMP
operator/(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result /= right;
    return result;
}

inline Integer_GMP
operator%(const Integer_GMP &left, const Integer_GMP &right)
{
    Integer_GMP result = left;
    result %= right;
    return result;
}

inline Integer_GMP
operator%(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    Integer_GMP result = left;
    result %= rightatom;
    return result;
}

inline Integer_GMP
operator%(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result %= right;
    return result;
}

inline Integer_GMP
operator%(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    Integer_GMP result = left;
    result %= rightatom;
    return result;
}

inline Integer_GMP
operator%(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    Integer_GMP result = leftatom;
    result %= right;
    return result;
}

inline bool
operator==(const Integer_GMP &left, const Integer_GMP &right)
{
    return !mpz_cmp(left.value_,right.value_);
}

inline bool
operator==(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    return !(mpz_cmp_ui(left.value_,(unsigned long int)rightatom));
}

inline bool
operator==(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    return !(mpz_cmp_ui(right.value_,(unsigned long int)leftatom));
}

inline bool
operator==(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    return !(mpz_cmp_si((left.value_),((signed long int)rightatom)));
}

inline bool
operator==(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    return !(mpz_cmp_si(right.value_,(signed long int)leftatom));
}

inline bool
operator!=(const Integer_GMP &left, const Integer_GMP &right)
{
    return mpz_cmp(left.value_,right.value_);
}

inline bool
operator!=(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    return mpz_cmp_ui(left.value_,(unsigned long int)rightatom);
}

inline bool
operator!=(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    return mpz_cmp_ui(right.value_,(unsigned long int)leftatom);
}

inline bool
operator!=(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    return mpz_cmp_si(left.value_,(signed long int)rightatom);
}

inline bool
operator!=(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    return mpz_cmp_si(right.value_,(signed long int)leftatom);
}

inline bool
operator>(const Integer_GMP &left, const Integer_GMP &right)
{
    return mpz_cmp(left.value_,right.value_) > 0;
}

inline bool
operator>(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    return (mpz_cmp_ui(left.value_,(unsigned long int)rightatom)) > 0;
}

inline bool
operator>(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_ui(right.value_,(unsigned long int)leftatom)) < 0;
}

inline bool
operator>(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    return (mpz_cmp_si(left.value_,(signed long int)rightatom)) > 0;
}

inline bool
operator>(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_si(right.value_,(signed long int)leftatom)) < 0;
}

inline bool
operator>=(const Integer_GMP &left, const Integer_GMP &right)
{
    return mpz_cmp(left.value_,right.value_) >= 0;
}

inline bool
operator>=(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    return (mpz_cmp_ui(left.value_,(unsigned long int)rightatom)) >= 0;
}

inline bool
operator>=(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_ui(right.value_,(unsigned long int)leftatom)) <= 0;
}

inline bool
operator>=(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    return (mpz_cmp_si(left.value_,(signed long int)rightatom)) >= 0;
}

inline bool
operator>=(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_si(right.value_,(signed long int)leftatom)) <= 0;
}

inline bool
operator<(const Integer_GMP &left, const Integer_GMP &right)
{
    return mpz_cmp(left.value_,right.value_) < 0;
}

inline bool
operator<(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    return (mpz_cmp_ui(left.value_,(unsigned long int)rightatom)) < 0;
}

inline bool
operator<(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_ui(right.value_,(unsigned long int)leftatom)) > 0;
}

inline bool
operator<(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    return (mpz_cmp_si(left.value_,(signed long int)rightatom)) < 0;
}

inline bool
operator<(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_si(right.value_,(signed long int)leftatom)) > 0;
}

inline bool
operator<=(const Integer_GMP &left, const Integer_GMP &right)
{
    return mpz_cmp(left.value_,right.value_) <= 0;
}

inline bool
operator<=(const Integer_GMP &left, const Integer_GMP::atom &rightatom)
{
    return (mpz_cmp_ui(left.value_,(unsigned long int)rightatom)) <= 0;
}

inline bool
operator<=(const Integer_GMP::atom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_ui(right.value_,(unsigned long int)leftatom)) >= 0;
}

inline bool
operator<=(const Integer_GMP &left, const Integer_GMP::satom &rightatom)
{
    return (mpz_cmp_si(left.value_,(signed long int)rightatom)) <= 0;
}

inline bool
operator<=(const Integer_GMP::satom &leftatom, const Integer_GMP &right)
{
    return (mpz_cmp_si(right.value_,(signed long int)leftatom)) >= 0;
}

inline bool
operator&&(const Integer_GMP &left, const Integer_GMP &right)
{
    return (bool)left && (bool)right;
}

inline bool
operator&&(const Integer_GMP &left, const bool right)
{
    return (bool)left && right;
}

inline bool
operator&&(const bool left, const Integer_GMP &right)
{
    return left && (bool)right;
}

inline bool
operator||(const Integer_GMP &left, const Integer_GMP &right)
{
    return (bool)left || (bool)right;
}

inline bool
operator||(const Integer_GMP &left, const bool right)
{
    return (bool)left || right;
}

inline bool
operator||(const bool left, const Integer_GMP &right)
{
    return left || (bool)right;
}

inline std::ostream &
operator<<(std::ostream &s, const Integer_GMP &i)    // FIXME:  needs io manips?
{
    unsigned long strsize = mpz_sizeinbase(i.value_,10) + 2;
    char *str = new char[strsize];
    mpz_get_str(str,10,i.value_);
    s << str;
    delete [] str;

    return s;
}

inline void
swap(Integer_GMP &left, Integer_GMP &right)
{
    mpz_t value;
    mpz_init(value);
    mpz_set(value,left.value_);
    mpz_set(left.value_,right.value_);
    mpz_set(right.value_,value);
    mpz_clear(value);
}

inline Integer_GMP
abs(const Integer_GMP &other)
{
    Integer_GMP result = other;
    mpz_abs(result.value_,result.value_);
    return result;
}

inline Integer_GMP
min(const Integer_GMP &left, const Integer_GMP &right)
{
    if (left < right)
        return left;
    else
        return right;
}

inline Integer_GMP
max(const Integer_GMP &left, const Integer_GMP &right)
{
    if (left > right)
        return left;
    else
        return right;
}

inline Integer_GMP
fac(const Integer_GMP &value)
{
    if (value < 0) throw Integer_GMP::negativefactorial();
    Integer_GMP result = 1;
    if (value > 1)
    {
        Integer_GMP next = value;
        do
        {
            result *= next;
            --next;
        }
        while (next > 1);
    }
    return result;
}

inline Integer_GMP
pow(const Integer_GMP &value, const Integer_GMP &exponent)
{
    if (exponent == 0) return 1;
    Integer_GMP result = value;
    Integer_GMP count = abs(exponent);
    for (--count; count > 0; --count)
        result *= value;
    if (exponent < 0) return 1 / result;    // always rounds to zero!
    return result;
}

inline void
divmod(const Integer_GMP &dividend, const Integer_GMP &divisor,
       Integer_GMP &quotient, Integer_GMP &remainder)
{
    mpz_tdiv_qr(quotient.value_,remainder.value_,
                dividend.value_,divisor.value_);
}

#include <cctype>
#include <cstring>

inline    // FIXME: probably should not be inline?
Integer_GMP::Integer_GMP(const char *asciiz)
{
    // FIXME: decimal string assumed, need to allow base 2, 8, 16, etc.

    const size_t len = strlen(asciiz);
    size_t i;
    for (i = 0; i < len; ++i)
        if (!isdigit(asciiz[i])) throw badstring();

    if (mpz_init_set_str(value_,asciiz,10) == -1)
    {
        mpz_clear(value_);
        throw badstring();
    }
}

#ifdef PURENUM_SETTING_INT_DEFAULT
typedef Integer_GMP Int;
#endif    // PURENUM_SETTING_INT_DEFAULT

#undef EX

#endif    // INTEGER_GMP_H

