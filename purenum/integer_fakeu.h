// ------------------
// Purenum 0.4e alpha
// ------------------
// integer_fakeu.h
// ------------------

// class Integer_fakeu is NOT a bignum, hence the name "fake"
// it provides the Integer interface around a simple unsigned int
// this is useful for speed testing on small integer values


#ifndef INTEGER_FAKEU_H
#define INTEGER_FAKEU_H


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


// FIXME: delete the following <iostream> include?
#include <iostream>

class Integer_fakeu
{
public:
    typedef INTEGER_H_DIGITTYPE atom;
    typedef INTEGER_H_SDIGITTYPE satom;
    enum sign { positive, negative };

    inline Integer_fakeu();                     // see INTEGER_H_INITZERO above
    inline Integer_fakeu(sign, atom);           // init from a hardware number
    inline Integer_fakeu(atom);                 // init from a hardware value
    inline Integer_fakeu(satom);                // init from a hardware value
    inline Integer_fakeu(const Integer_fakeu &);// init from Integer_fakeu
EX  inline Integer_fakeu(const char *ascii);    // init from a string number
    inline ~Integer_fakeu();                    // destructor

    // conversion operators
    // FIXME: need signed int conversion operator
    inline operator bool() const;
    inline operator unsigned int() const;

    // unary math operators (members)
    inline Integer_fakeu &operator++();      // prefix
    inline Integer_fakeu operator++(int);    // postfix
    inline Integer_fakeu &operator--();      // prefix
    inline Integer_fakeu operator--(int);    // postfix

    // binary math operators (members)
    inline Integer_fakeu &operator=(const Integer_fakeu &);
    inline Integer_fakeu &operator=(const atom &);
    inline Integer_fakeu &operator=(const satom &);
    inline Integer_fakeu &operator+=(const Integer_fakeu &);
    inline Integer_fakeu &operator+=(const atom &);
    inline Integer_fakeu &operator+=(const satom &);
    inline Integer_fakeu &operator-=(const Integer_fakeu &);
    inline Integer_fakeu &operator-=(const atom &);
    inline Integer_fakeu &operator-=(const satom &);
    inline Integer_fakeu &operator*=(const Integer_fakeu &);
    inline Integer_fakeu &operator*=(const atom &);
    inline Integer_fakeu &operator*=(const satom &);
    inline Integer_fakeu &operator/=(const Integer_fakeu &);
    inline Integer_fakeu &operator/=(const atom &);
    inline Integer_fakeu &operator/=(const satom &);
    inline Integer_fakeu &operator%=(const Integer_fakeu &);
    inline Integer_fakeu &operator%=(const atom &);
    inline Integer_fakeu &operator%=(const satom &);

    // friends: unary math operators (global functions)
    friend Integer_fakeu operator~(const Integer_fakeu &);
    friend bool operator!(const Integer_fakeu &);
    friend Integer_fakeu operator-(const Integer_fakeu &);
    friend Integer_fakeu operator+(const Integer_fakeu &);

    // friends: binary math operators (global functions)
  friend Integer_fakeu operator+(const Integer_fakeu &, const Integer_fakeu &);
  friend Integer_fakeu operator+(const Integer_fakeu &, const atom &);
  friend Integer_fakeu operator+(const atom &, const Integer_fakeu &);
  friend Integer_fakeu operator+(const Integer_fakeu &, const satom &);
  friend Integer_fakeu operator+(const satom &, const Integer_fakeu &);
  friend Integer_fakeu operator-(const Integer_fakeu &, const Integer_fakeu &);
  friend Integer_fakeu operator-(const Integer_fakeu &, const atom &);
  friend Integer_fakeu operator-(const atom &, const Integer_fakeu &);
  friend Integer_fakeu operator-(const Integer_fakeu &, const satom &);
  friend Integer_fakeu operator-(const satom &, const Integer_fakeu &);
  friend Integer_fakeu operator*(const Integer_fakeu &, const Integer_fakeu &);
  friend Integer_fakeu operator*(const Integer_fakeu &, const atom &);
  friend Integer_fakeu operator*(const atom &, const Integer_fakeu &);
  friend Integer_fakeu operator*(const Integer_fakeu &, const satom &);
  friend Integer_fakeu operator*(const satom &, const Integer_fakeu &);
  friend Integer_fakeu operator/(const Integer_fakeu &, const Integer_fakeu &);
  friend Integer_fakeu operator/(const Integer_fakeu &, const atom &);
  friend Integer_fakeu operator/(const atom &, const Integer_fakeu &);
  friend Integer_fakeu operator/(const Integer_fakeu &, const satom &);
  friend Integer_fakeu operator/(const satom &, const Integer_fakeu &);
  friend Integer_fakeu operator%(const Integer_fakeu &, const Integer_fakeu &);
  friend Integer_fakeu operator%(const Integer_fakeu &, const atom &);
  friend Integer_fakeu operator%(const atom &, const Integer_fakeu &);
  friend Integer_fakeu operator%(const Integer_fakeu &, const satom &);
  friend Integer_fakeu operator%(const satom &, const Integer_fakeu &);
  friend bool operator==(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator==(const Integer_fakeu &, const atom &);
  friend bool operator==(const atom &, const Integer_fakeu &);
  friend bool operator==(const Integer_fakeu &, const satom &);
  friend bool operator==(const satom &, const Integer_fakeu &);
  friend bool operator!=(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator!=(const Integer_fakeu &, const atom &);
  friend bool operator!=(const atom &, const Integer_fakeu &);
  friend bool operator!=(const Integer_fakeu &, const satom &);
  friend bool operator!=(const satom &, const Integer_fakeu &);
  friend bool operator>(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator>(const Integer_fakeu &, const atom &);
  friend bool operator>(const atom &, const Integer_fakeu &);
  friend bool operator>(const Integer_fakeu &, const satom &);
  friend bool operator>(const satom &, const Integer_fakeu &);
  friend bool operator>=(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator>=(const Integer_fakeu &, const atom &);
  friend bool operator>=(const atom &, const Integer_fakeu &);
  friend bool operator>=(const Integer_fakeu &, const satom &);
  friend bool operator>=(const satom &, const Integer_fakeu &);
  friend bool operator<(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator<(const Integer_fakeu &, const atom &);
  friend bool operator<(const atom &, const Integer_fakeu &);
  friend bool operator<(const Integer_fakeu &, const satom &);
  friend bool operator<(const satom &, const Integer_fakeu &);
  friend bool operator<=(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator<=(const Integer_fakeu &, const atom &);
  friend bool operator<=(const atom &, const Integer_fakeu &);
  friend bool operator<=(const Integer_fakeu &, const satom &);
  friend bool operator<=(const satom &, const Integer_fakeu &);
  friend bool operator&&(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator&&(const Integer_fakeu &, const bool);
  friend bool operator&&(const bool, const Integer_fakeu &);
  friend bool operator||(const Integer_fakeu &, const Integer_fakeu &);
  friend bool operator||(const Integer_fakeu &, const bool);
  friend bool operator||(const bool, const Integer_fakeu &);

    // FIXME: move these to a seperate optional file?
    // other/math functions (global functions)
    friend std::ostream &operator<<(std::ostream &, const Integer_fakeu &);  // FIXME delete?
    friend void swap(Integer_fakeu &, Integer_fakeu &);
    friend Integer_fakeu abs(const Integer_fakeu &);
    friend Integer_fakeu min(const Integer_fakeu &, const Integer_fakeu &);
    friend Integer_fakeu max(const Integer_fakeu &, const Integer_fakeu &);
    friend Integer_fakeu fac(const Integer_fakeu &);
    friend Integer_fakeu pow(const Integer_fakeu &, const Integer_fakeu &);
    friend void divmod(const Integer_fakeu &dividend, const Integer_fakeu
                       &divisor, Integer_fakeu &quotient, Integer_fakeu
                       &remainder);

    // exceptions
    // FIXME: throw outofmemory when 'new' fails?  or not?
    class exception { };
    class divisionbyzero : public exception { };   // division by zero
    class outofmemory : public exception { };      // software integer overflow
    class outofrange : public exception { };       // hardware integer overflow
    class badstring : public exception { };        // non-numeric initializer
    class negativefactorial : public exception {}; // can't do x! for x < 0

    // ascii string input and output           FIXME: not internationalized
    char *ascii(const Integer_fakeu base = 10) const;              // out new[]
    void ascii(atom size, char *, const Integer_fakeu base = 10) const;//output
    void ascii(const char *, const Integer_fakeu base = 10);       // input

private:
    // internal representation of the integer value (this is the entire state)
    Integer_fakeu::atom value_;
};

//#if 0
// functions to help measure code speed (tricks compiler into not optimizing)
void touch(const Integer_fakeu &);          // empty function, for testing only
void touch(const Integer_fakeu::atom &);    // empty function, for testing only
//#endif    // 0

// unary math operators (global functions)
inline Integer_fakeu operator~(const Integer_fakeu &left);
inline bool operator!(const Integer_fakeu &left);
inline Integer_fakeu operator-(const Integer_fakeu &left);
inline Integer_fakeu operator+(const Integer_fakeu &left);

// binary math operators (global functions)
inline Integer_fakeu operator+(const Integer_fakeu &left, const Integer_fakeu &right);
inline Integer_fakeu operator+(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline Integer_fakeu operator+(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator+(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline Integer_fakeu operator+(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator-(const Integer_fakeu &left, const Integer_fakeu &right);
inline Integer_fakeu operator-(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline Integer_fakeu operator-(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator-(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline Integer_fakeu operator-(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator*(const Integer_fakeu &left, const Integer_fakeu &right);
inline Integer_fakeu operator*(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline Integer_fakeu operator*(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator*(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline Integer_fakeu operator*(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator/(const Integer_fakeu &left, const Integer_fakeu &right);
inline Integer_fakeu operator/(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline Integer_fakeu operator/(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator/(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline Integer_fakeu operator/(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator%(const Integer_fakeu &left, const Integer_fakeu &right);
inline Integer_fakeu operator%(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline Integer_fakeu operator%(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline Integer_fakeu operator%(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline Integer_fakeu operator%(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator==(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator==(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline bool operator==(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline bool operator==(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline bool operator==(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator!=(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator!=(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline bool operator!=(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline bool operator!=(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline bool operator!=(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator>(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator>(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline bool operator>(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline bool operator>(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline bool operator>(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator>=(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator>=(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline bool operator>=(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline bool operator>=(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline bool operator>=(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator<(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator<(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline bool operator<(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline bool operator<(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline bool operator<(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator<=(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator<=(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom);
inline bool operator<=(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right);
inline bool operator<=(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom);
inline bool operator<=(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right);
inline bool operator&&(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator&&(const Integer_fakeu &left, const bool right);
inline bool operator&&(const bool left, const Integer_fakeu &right);
inline bool operator||(const Integer_fakeu &left, const Integer_fakeu &right);
inline bool operator||(const Integer_fakeu &left, const bool right);
inline bool operator||(const bool left, const Integer_fakeu &right);
inline std::ostream &operator<<(std::ostream &, const Integer_fakeu &);
inline void swap(Integer_fakeu &, Integer_fakeu &);
inline Integer_fakeu abs(const Integer_fakeu &);
inline Integer_fakeu min(const Integer_fakeu &, const Integer_fakeu &);
inline Integer_fakeu max(const Integer_fakeu &, const Integer_fakeu &);
inline Integer_fakeu fac(const Integer_fakeu &);
inline Integer_fakeu pow(const Integer_fakeu &, const Integer_fakeu &);
inline void divmod(const Integer_fakeu &dividend, const Integer_fakeu &divisor,
                   Integer_fakeu &quotient, Integer_fakeu &remainder);

inline
Integer_fakeu::Integer_fakeu()
{
}

inline
Integer_fakeu::Integer_fakeu(Integer_fakeu::sign s, Integer_fakeu::atom a)
{
    if (s == negative) outofrange();
    value_ = a;
}

inline
Integer_fakeu::Integer_fakeu(Integer_fakeu::atom a)
{
    value_ = a;
}

inline
Integer_fakeu::Integer_fakeu(Integer_fakeu::satom i)
{
    value_ = (Integer_fakeu::atom)i;
}

inline
Integer_fakeu::Integer_fakeu(const Integer_fakeu &other)
{
    value_ = other.value_;
}

inline
Integer_fakeu::~Integer_fakeu()
{
}

inline
Integer_fakeu::operator bool() const
{
    return value_;
}

inline
Integer_fakeu::operator unsigned int() const
{
    return value_;
}

inline Integer_fakeu &
Integer_fakeu::operator++()    // prefix
{
    ++value_;
    return *this;
}

inline Integer_fakeu
Integer_fakeu::operator++(int)    // postfix
{
    Integer_fakeu result = *this;
    value_++;
    return result;
}

inline Integer_fakeu &
Integer_fakeu::operator--()    // prefix
{
    --value_;
    return *this;
}

inline Integer_fakeu
Integer_fakeu::operator--(int)    // postfix
{
    Integer_fakeu result = *this;
    value_--;
    return result;
}

inline Integer_fakeu &
Integer_fakeu::operator=(const Integer_fakeu &right)
{
    value_ = right.value_;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator=(const atom &rightatom)
{
    value_ = rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator=(const satom &rightatom)
{
    value_ = (Integer_fakeu::atom)rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator+=(const Integer_fakeu &right)
{
    value_ += right.value_;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator+=(const atom &rightatom)
{
    value_ += rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator+=(const satom &rightatom)
{
    value_ += (Integer_fakeu::atom)rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator-=(const Integer_fakeu &right)
{
    value_ -= right.value_;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator-=(const atom &rightatom)
{
    value_ -= rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator-=(const satom &rightatom)
{
    value_ -= (Integer_fakeu::atom)rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator*=(const Integer_fakeu &right)
{
    value_ *= right.value_;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator*=(const atom &rightatom)
{
    value_ *= rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator*=(const satom &rightatom)
{
    value_ *= (Integer_fakeu::atom)rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator/=(const Integer_fakeu &right)
{
    value_ /= right.value_;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator/=(const atom &rightatom)
{
    value_ /= rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator/=(const satom &rightatom)
{
    value_ /= (Integer_fakeu::atom)rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator%=(const Integer_fakeu &right)
{
    value_ %= right.value_;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator%=(const atom &rightatom)
{
    value_ %= rightatom;
    return *this;
}

inline Integer_fakeu &
Integer_fakeu::operator%=(const satom &rightatom)
{
    value_ %= (Integer_fakeu::atom)rightatom;
    return *this;
}

inline Integer_fakeu
operator~(const Integer_fakeu &left)    // FIXME see integer.txt complement info
{
    Integer_fakeu result = left;
    result.value_ = ~result.value_;
    return result;
}

inline bool
operator!(const Integer_fakeu &left)
{
    return !left.value_;
}

inline Integer_fakeu
operator-(const Integer_fakeu &left)
{
    Integer_fakeu result = left;
    result.value_ = !result.value_;
    return result;
}

inline Integer_fakeu
operator+(const Integer_fakeu &left)
{
    Integer_fakeu result = left;
    return result;
}

inline Integer_fakeu
operator+(const Integer_fakeu &left, const Integer_fakeu &right)
{
    Integer_fakeu result = left;
    result += right;
    return result;
}

inline Integer_fakeu
operator+(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    Integer_fakeu result = left;
    result += rightatom;
    return result;
}

inline Integer_fakeu
operator+(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result += right;
    return result;
}

inline Integer_fakeu
operator+(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    Integer_fakeu result = left;
    result += rightatom;
    return result;
}

inline Integer_fakeu
operator+(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result += right;
    return result;
}

inline Integer_fakeu
operator-(const Integer_fakeu &left, const Integer_fakeu &right)
{
    Integer_fakeu result = left;
    result -= right;
    return result;
}

inline Integer_fakeu
operator-(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    Integer_fakeu result = left;
    result -= rightatom;
    return result;
}

inline Integer_fakeu
operator-(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result -= right;
    return result;
}

inline Integer_fakeu
operator-(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    Integer_fakeu result = left;
    result -= rightatom;
    return result;
}

inline Integer_fakeu
operator-(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result -= right;
    return result;
}

inline Integer_fakeu
operator*(const Integer_fakeu &left, const Integer_fakeu &right)
{
    Integer_fakeu result = left;
    result *= right;
    return result;
}

inline Integer_fakeu
operator*(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    Integer_fakeu result = left;
    result *= rightatom;
    return result;
}

inline Integer_fakeu
operator*(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result *= right;
    return result;
}

inline Integer_fakeu
operator*(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    Integer_fakeu result = left;
    result *= rightatom;
    return result;
}

inline Integer_fakeu
operator*(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result *= right;
    return result;
}

inline Integer_fakeu
operator/(const Integer_fakeu &left, const Integer_fakeu &right)
{
    Integer_fakeu result = left;
    result /= right;
    return result;
}

inline Integer_fakeu
operator/(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    Integer_fakeu result = left;
    result /= rightatom;
    return result;
}

inline Integer_fakeu
operator/(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result /= right;
    return result;
}

inline Integer_fakeu
operator/(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    Integer_fakeu result = left;
    result /= rightatom;
    return result;
}

inline Integer_fakeu
operator/(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result /= right;
    return result;
}

inline Integer_fakeu
operator%(const Integer_fakeu &left, const Integer_fakeu &right)
{
    Integer_fakeu result = left;
    result %= right;
    return result;
}

inline Integer_fakeu
operator%(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    Integer_fakeu result = left;
    result %= rightatom;
    return result;
}

inline Integer_fakeu
operator%(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result %= right;
    return result;
}

inline Integer_fakeu
operator%(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    Integer_fakeu result = left;
    result %= rightatom;
    return result;
}

inline Integer_fakeu
operator%(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    Integer_fakeu result = leftatom;
    result %= right;
    return result;
}

inline bool
operator==(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return left.value_ == right.value_;
}

inline bool
operator==(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    return left.value_ == rightatom;
}

inline bool
operator==(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    return leftatom == right.value_;
}

inline bool
operator==(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    return left.value_ == (Integer_fakeu::atom)rightatom;
}

inline bool
operator==(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    return (Integer_fakeu::atom)leftatom == right.value_;
}

inline bool
operator!=(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return left.value_ != right.value_;
}

inline bool
operator!=(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    return left.value_ != rightatom;
}

inline bool
operator!=(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    return leftatom != right.value_;
}

inline bool
operator!=(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    return left.value_ != (Integer_fakeu::atom)rightatom;
}

inline bool
operator!=(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    return (Integer_fakeu::atom)leftatom != right.value_;
}

inline bool
operator>(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return left.value_ > right.value_;
}

inline bool
operator>(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    return left.value_ > rightatom;
}

inline bool
operator>(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    return leftatom > right.value_;
}

inline bool
operator>(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    return left.value_ > (Integer_fakeu::atom)rightatom;
}

inline bool
operator>(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    return (Integer_fakeu::atom)leftatom > right.value_;
}

inline bool
operator>=(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return left.value_ >= right.value_;
}

inline bool
operator>=(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    return left.value_ >= rightatom;
}

inline bool
operator>=(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    return leftatom >= right.value_;
}

inline bool
operator>=(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    return left.value_ >= (Integer_fakeu::atom)rightatom;
}

inline bool
operator>=(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    return (Integer_fakeu::atom)leftatom >= right.value_;
}

inline bool
operator<(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return left.value_ < right.value_;
}

inline bool
operator<(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    return left.value_ < rightatom;
}

inline bool
operator<(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    return leftatom < right.value_;
}

inline bool
operator<(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    return left.value_ < (Integer_fakeu::atom)rightatom;
}

inline bool
operator<(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    return (Integer_fakeu::atom)leftatom < right.value_;
}

inline bool
operator<=(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return left.value_ <= right.value_;
}

inline bool
operator<=(const Integer_fakeu &left, const Integer_fakeu::atom &rightatom)
{
    return left.value_ <= rightatom;
}

inline bool
operator<=(const Integer_fakeu::atom &leftatom, const Integer_fakeu &right)
{
    return leftatom <= right.value_;
}

inline bool
operator<=(const Integer_fakeu &left, const Integer_fakeu::satom &rightatom)
{
    return left.value_ <= (Integer_fakeu::atom)rightatom;
}

inline bool
operator<=(const Integer_fakeu::satom &leftatom, const Integer_fakeu &right)
{
    return (Integer_fakeu::atom)leftatom <= right.value_;
}

inline bool
operator&&(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return (bool)left && (bool)right;
}

inline bool
operator&&(const Integer_fakeu &left, const bool right)
{
    return (bool)left && right;
}

inline bool
operator&&(const bool left, const Integer_fakeu &right)
{
    return left && (bool)right;
}

inline bool
operator||(const Integer_fakeu &left, const Integer_fakeu &right)
{
    return (bool)left || (bool)right;
}

inline bool
operator||(const Integer_fakeu &left, const bool right)
{
    return (bool)left || right;
}

inline bool
operator||(const bool left, const Integer_fakeu &right)
{
    return left || (bool)right;
}

#include <unistd.h>    // FIXME:  delete this line?

inline std::ostream &
operator<<(std::ostream &s, const Integer_fakeu &i)    // FIXME:  needs io manips?
{
    return s << i.value_;
}

inline void
swap(Integer_fakeu &left, Integer_fakeu &right)
{
    Integer_fakeu::atom value = right.value_;
    right.value_ = left.value_;
    left.value_ = value;
}

inline Integer_fakeu
abs(const Integer_fakeu &other)
{
    Integer_fakeu result = other;
    if (result.value_ < 0) result.value_ = -result.value_;
    return result;
}

inline Integer_fakeu
min(const Integer_fakeu &left, const Integer_fakeu &right)
{
    if (left < right)
        return left;
    else
        return right;
}

inline Integer_fakeu
max(const Integer_fakeu &left, const Integer_fakeu &right)
{
    if (left > right)
        return left;
    else
        return right;
}

inline Integer_fakeu
fac(const Integer_fakeu &value)
{
    if (value < 0) throw Integer_fakeu::negativefactorial();
    Integer_fakeu result = 1;
    if (value > 1)
    {
        Integer_fakeu next = value;
        do
        {
            result *= next;
            --next;
        }
        while (next > 1);
    }
    return result;
}

inline Integer_fakeu
pow(const Integer_fakeu &value, const Integer_fakeu &exponent)
{
    if (exponent == 0) return 1;
    Integer_fakeu result = value;
    Integer_fakeu count = abs(exponent);
    for (--count; count > 0; --count)
        result *= value;
    if (exponent < 0) return 1 / result;    // always rounds to zero!
    return result;
}

inline void
divmod(const Integer_fakeu &dividend, const Integer_fakeu &divisor,
       Integer_fakeu &quotient, Integer_fakeu &remainder)
{
    quotient = dividend.value_ / divisor.value_;
    remainder = dividend.value_ % divisor.value_;
}

inline    // FIXME: probably should not be inline?
Integer_fakeu::Integer_fakeu(const char *asciiz)
{
    // FIXME: decimal string assumed, need to allow base 2, 8, 16, etc.
    // FIXME: also need to check that the base is not larger than maxatom

    value_ = 0;

    unsigned long len = 0;
    for (; asciiz[len] != 0; ++len)
        if (asciiz[len] == ' ') throw badstring();    // no spaces allowed
    if (len == 0) throw badstring();
    if (asciiz[0] == '-') throw outofrange();

    Integer_fakeu::atom &result = value_, digit = 0, shift, i2;
    unsigned long i = len - 1;
    unsigned int value;
    for (;; i--)
    {
        value = asciiz[i] - '0';
        shift = 1;
        for (i2 = 0; i2 < digit; i2++)
            shift *= 10;    // FIXME: only works for base 10 strings
        result += value * shift;
        ++digit;
        if (i == 0) break;
    }
}

#ifdef PURENUM_SETTING_INT_DEFAULT
typedef Integer_fakeu Int;
#endif    // PURENUM_SETTING_INT_DEFAULT

#undef EX

#endif    // INTEGER_FAKEU_H

