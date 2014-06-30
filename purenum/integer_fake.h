// ------------------
// Purenum 0.4e alpha
// ------------------
// integer_fake.h
// ------------------

// class Integer_fake is NOT a bignum, hence the name "fake"
// it provides the Integer interface around a simple signed int
// this is useful for speed testing on small integer values


#ifndef INTEGER_FAKE_H
#define INTEGER_FAKE_H


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

class Integer_fake
{
public:
    typedef INTEGER_H_DIGITTYPE atom;
    typedef INTEGER_H_SDIGITTYPE satom;
    enum sign { positive, negative };

    inline Integer_fake();                      // see INTEGER_H_INITZERO above
    inline Integer_fake(sign, atom);            // init from a hardware number
    inline Integer_fake(atom);                  // init from a hardware value
    inline Integer_fake(satom);                 // init from a hardware value
    inline Integer_fake(const Integer_fake &);  // init from Integer_fake
EX  inline Integer_fake(const char *ascii);     // init from a string number
    inline ~Integer_fake();                     // destructor

    // conversion operators
    // FIXME: need signed int conversion operator
    inline operator bool() const;
    inline operator unsigned int() const;

    // unary math operators (members)
    inline Integer_fake &operator++();      // prefix
    inline Integer_fake operator++(int);    // postfix
    inline Integer_fake &operator--();      // prefix
    inline Integer_fake operator--(int);    // postfix

    // binary math operators (members)
    inline Integer_fake &operator=(const Integer_fake &);
    inline Integer_fake &operator=(const atom &);
    inline Integer_fake &operator=(const satom &);
    inline Integer_fake &operator+=(const Integer_fake &);
    inline Integer_fake &operator+=(const atom &);
    inline Integer_fake &operator+=(const satom &);
    inline Integer_fake &operator-=(const Integer_fake &);
    inline Integer_fake &operator-=(const atom &);
    inline Integer_fake &operator-=(const satom &);
    inline Integer_fake &operator*=(const Integer_fake &);
    inline Integer_fake &operator*=(const atom &);
    inline Integer_fake &operator*=(const satom &);
    inline Integer_fake &operator/=(const Integer_fake &);
    inline Integer_fake &operator/=(const atom &);
    inline Integer_fake &operator/=(const satom &);
    inline Integer_fake &operator%=(const Integer_fake &);
    inline Integer_fake &operator%=(const atom &);
    inline Integer_fake &operator%=(const satom &);

    // friends: unary math operators (global functions)
    friend Integer_fake operator~(const Integer_fake &);
    friend bool operator!(const Integer_fake &);
    friend Integer_fake operator-(const Integer_fake &);
    friend Integer_fake operator+(const Integer_fake &);

    // friends: binary math operators (global functions)
    friend Integer_fake operator+(const Integer_fake &, const Integer_fake &);
    friend Integer_fake operator+(const Integer_fake &, const atom &);
    friend Integer_fake operator+(const atom &, const Integer_fake &);
    friend Integer_fake operator+(const Integer_fake &, const satom &);
    friend Integer_fake operator+(const satom &, const Integer_fake &);
    friend Integer_fake operator-(const Integer_fake &, const Integer_fake &);
    friend Integer_fake operator-(const Integer_fake &, const atom &);
    friend Integer_fake operator-(const atom &, const Integer_fake &);
    friend Integer_fake operator-(const Integer_fake &, const satom &);
    friend Integer_fake operator-(const satom &, const Integer_fake &);
    friend Integer_fake operator*(const Integer_fake &, const Integer_fake &);
    friend Integer_fake operator*(const Integer_fake &, const atom &);
    friend Integer_fake operator*(const atom &, const Integer_fake &);
    friend Integer_fake operator*(const Integer_fake &, const satom &);
    friend Integer_fake operator*(const satom &, const Integer_fake &);
    friend Integer_fake operator/(const Integer_fake &, const Integer_fake &);
    friend Integer_fake operator/(const Integer_fake &, const atom &);
    friend Integer_fake operator/(const atom &, const Integer_fake &);
    friend Integer_fake operator/(const Integer_fake &, const satom &);
    friend Integer_fake operator/(const satom &, const Integer_fake &);
    friend Integer_fake operator%(const Integer_fake &, const Integer_fake &);
    friend Integer_fake operator%(const Integer_fake &, const atom &);
    friend Integer_fake operator%(const atom &, const Integer_fake &);
    friend Integer_fake operator%(const Integer_fake &, const satom &);
    friend Integer_fake operator%(const satom &, const Integer_fake &);
    friend bool operator==(const Integer_fake &, const Integer_fake &);
    friend bool operator==(const Integer_fake &, const atom &);
    friend bool operator==(const atom &, const Integer_fake &);
    friend bool operator==(const Integer_fake &, const satom &);
    friend bool operator==(const satom &, const Integer_fake &);
    friend bool operator!=(const Integer_fake &, const Integer_fake &);
    friend bool operator!=(const Integer_fake &, const atom &);
    friend bool operator!=(const atom &, const Integer_fake &);
    friend bool operator!=(const Integer_fake &, const satom &);
    friend bool operator!=(const satom &, const Integer_fake &);
    friend bool operator>(const Integer_fake &, const Integer_fake &);
    friend bool operator>(const Integer_fake &, const atom &);
    friend bool operator>(const atom &, const Integer_fake &);
    friend bool operator>(const Integer_fake &, const satom &);
    friend bool operator>(const satom &, const Integer_fake &);
    friend bool operator>=(const Integer_fake &, const Integer_fake &);
    friend bool operator>=(const Integer_fake &, const atom &);
    friend bool operator>=(const atom &, const Integer_fake &);
    friend bool operator>=(const Integer_fake &, const satom &);
    friend bool operator>=(const satom &, const Integer_fake &);
    friend bool operator<(const Integer_fake &, const Integer_fake &);
    friend bool operator<(const Integer_fake &, const atom &);
    friend bool operator<(const atom &, const Integer_fake &);
    friend bool operator<(const Integer_fake &, const satom &);
    friend bool operator<(const satom &, const Integer_fake &);
    friend bool operator<=(const Integer_fake &, const Integer_fake &);
    friend bool operator<=(const Integer_fake &, const atom &);
    friend bool operator<=(const atom &, const Integer_fake &);
    friend bool operator<=(const Integer_fake &, const satom &);
    friend bool operator<=(const satom &, const Integer_fake &);
    friend bool operator&&(const Integer_fake &, const Integer_fake &);
    friend bool operator&&(const Integer_fake &, const bool);
    friend bool operator&&(const bool, const Integer_fake &);
    friend bool operator||(const Integer_fake &, const Integer_fake &);
    friend bool operator||(const Integer_fake &, const bool);
    friend bool operator||(const bool, const Integer_fake &);

    // FIXME: move these to a seperate optional file?
    // other/math functions (global functions)
    friend std::ostream &operator<<(std::ostream &, const Integer_fake &);  // FIXME delete?
    friend void swap(Integer_fake &, Integer_fake &);
    friend Integer_fake abs(const Integer_fake &);
    friend Integer_fake min(const Integer_fake &, const Integer_fake &);
    friend Integer_fake max(const Integer_fake &, const Integer_fake &);
    friend Integer_fake fac(const Integer_fake &);
    friend Integer_fake pow(const Integer_fake &, const Integer_fake &);
    friend void divmod(const Integer_fake &dividend, const Integer_fake
                       &divisor, Integer_fake &quotient, Integer_fake
                       &remainder);

    // exceptions
    // FIXME: throw outofmemory when 'new' fails?  or not?
    class exception { };
    class divisionbyzero : public exception { };    // division by zero
    class outofmemory : public exception { };       // software integer overflow
    class outofrange : public exception { };        // hardware integer overflow
    class badstring : public exception { };         // non-numeric initializer
    class negativefactorial : public exception {};  // can't do x! for x < 0

    // ascii string input and output           FIXME: not internationalized
    char *ascii(const Integer_fake base = 10) const;               // out new[]
    void ascii(atom size, char *, const Integer_fake base = 10) const;// output
    void ascii(const char *, const Integer_fake base = 10);           // input

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
    Integer_fake::satom value_;
};

//#if 0
// functions to help measure code speed (tricks compiler into not optimizing)
void touch(const Integer_fake &);          // empty function, for testing only
void touch(const Integer_fake::atom &);    // empty function, for testing only
//#endif    // 0

// unary math operators (global functions)
inline Integer_fake operator~(const Integer_fake &left);
inline bool operator!(const Integer_fake &left);
inline Integer_fake operator-(const Integer_fake &left);
inline Integer_fake operator+(const Integer_fake &left);

// binary math operators (global functions)
inline Integer_fake operator+(const Integer_fake &left, const Integer_fake &right);
inline Integer_fake operator+(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline Integer_fake operator+(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline Integer_fake operator+(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline Integer_fake operator+(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline Integer_fake operator-(const Integer_fake &left, const Integer_fake &right);
inline Integer_fake operator-(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline Integer_fake operator-(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline Integer_fake operator-(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline Integer_fake operator-(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline Integer_fake operator*(const Integer_fake &left, const Integer_fake &right);
inline Integer_fake operator*(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline Integer_fake operator*(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline Integer_fake operator*(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline Integer_fake operator*(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline Integer_fake operator/(const Integer_fake &left, const Integer_fake &right);
inline Integer_fake operator/(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline Integer_fake operator/(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline Integer_fake operator/(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline Integer_fake operator/(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline Integer_fake operator%(const Integer_fake &left, const Integer_fake &right);
inline Integer_fake operator%(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline Integer_fake operator%(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline Integer_fake operator%(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline Integer_fake operator%(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator==(const Integer_fake &left, const Integer_fake &right);
inline bool operator==(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline bool operator==(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline bool operator==(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline bool operator==(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator!=(const Integer_fake &left, const Integer_fake &right);
inline bool operator!=(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline bool operator!=(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline bool operator!=(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline bool operator!=(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator>(const Integer_fake &left, const Integer_fake &right);
inline bool operator>(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline bool operator>(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline bool operator>(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline bool operator>(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator>=(const Integer_fake &left, const Integer_fake &right);
inline bool operator>=(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline bool operator>=(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline bool operator>=(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline bool operator>=(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator<(const Integer_fake &left, const Integer_fake &right);
inline bool operator<(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline bool operator<(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline bool operator<(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline bool operator<(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator<=(const Integer_fake &left, const Integer_fake &right);
inline bool operator<=(const Integer_fake &left, const Integer_fake::atom &rightatom);
inline bool operator<=(const Integer_fake::atom &leftatom, const Integer_fake &right);
inline bool operator<=(const Integer_fake &left, const Integer_fake::satom &rightatom);
inline bool operator<=(const Integer_fake::satom &leftatom, const Integer_fake &right);
inline bool operator&&(const Integer_fake &left, const Integer_fake &right);
inline bool operator&&(const Integer_fake &left, const bool right);
inline bool operator&&(const bool left, const Integer_fake &right);
inline bool operator||(const Integer_fake &left, const Integer_fake &right);
inline bool operator||(const Integer_fake &left, const bool right);
inline bool operator||(const bool left, const Integer_fake &right);
inline std::ostream &operator<<(std::ostream &, const Integer_fake &);
inline void swap(Integer_fake &, Integer_fake &);
inline Integer_fake abs(const Integer_fake &);
inline Integer_fake min(const Integer_fake &, const Integer_fake &);
inline Integer_fake max(const Integer_fake &, const Integer_fake &);
inline Integer_fake fac(const Integer_fake &);
inline Integer_fake pow(const Integer_fake &, const Integer_fake &);
inline void divmod(const Integer_fake &dividend, const Integer_fake &divisor,
                   Integer_fake &quotient, Integer_fake &remainder);

inline
Integer_fake::Integer_fake()
{
}

inline
Integer_fake::Integer_fake(Integer_fake::sign s, Integer_fake::atom a)
{
    if (s == negative) outofrange();
    value_ = a;
}

inline
Integer_fake::Integer_fake(Integer_fake::atom a)
{
    value_ = (Integer_fake::satom)a;
}

inline
Integer_fake::Integer_fake(Integer_fake::satom i)
{
    value_ = i;
}

inline
Integer_fake::Integer_fake(const Integer_fake &other)
{
    value_ = other.value_;
}

inline
Integer_fake::~Integer_fake()
{
}

inline
Integer_fake::operator bool() const
{
    return value_;
}

inline
Integer_fake::operator unsigned int() const
{
    return value_;
}

inline Integer_fake &
Integer_fake::operator++()    // prefix
{
    ++value_;
    return *this;
}

inline Integer_fake
Integer_fake::operator++(int)    // postfix
{
    Integer_fake result = *this;
    value_++;
    return result;
}

inline Integer_fake &
Integer_fake::operator--()    // prefix
{
    --value_;
    return *this;
}

inline Integer_fake
Integer_fake::operator--(int)    // postfix
{
    Integer_fake result = *this;
    value_--;
    return result;
}

inline Integer_fake &
Integer_fake::operator=(const Integer_fake &right)
{
    value_ = right.value_;
    return *this;
}

inline Integer_fake &
Integer_fake::operator=(const atom &rightatom)
{
    value_ = (Integer_fake::satom)rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator=(const satom &rightatom)
{
    value_ = rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator+=(const Integer_fake &right)
{
    value_ += right.value_;
    return *this;
}

inline Integer_fake &
Integer_fake::operator+=(const atom &rightatom)
{
    value_ += (Integer_fake::satom)rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator+=(const satom &rightatom)
{
    value_ += rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator-=(const Integer_fake &right)
{
    value_ -= right.value_;
    return *this;
}

inline Integer_fake &
Integer_fake::operator-=(const atom &rightatom)
{
    value_ -= (Integer_fake::satom)rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator-=(const satom &rightatom)
{
    value_ -= rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator*=(const Integer_fake &right)
{
    value_ *= right.value_;
    return *this;
}

inline Integer_fake &
Integer_fake::operator*=(const atom &rightatom)
{
    value_ *= (Integer_fake::satom)rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator*=(const satom &rightatom)
{
    value_ *= rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator/=(const Integer_fake &right)
{
    value_ /= right.value_;
    return *this;
}

inline Integer_fake &
Integer_fake::operator/=(const atom &rightatom)
{
    value_ /= (Integer_fake::satom)rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator/=(const satom &rightatom)
{
    value_ /= rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator%=(const Integer_fake &right)
{
    value_ %= right.value_;
    return *this;
}

inline Integer_fake &
Integer_fake::operator%=(const atom &rightatom)
{
    value_ %= (Integer_fake::satom)rightatom;
    return *this;
}

inline Integer_fake &
Integer_fake::operator%=(const satom &rightatom)
{
    value_ %= rightatom;
    return *this;
}

inline Integer_fake
operator~(const Integer_fake &left)    // FIXME see integer.txt complement info
{
    Integer_fake result = left;
    result.value_ = ~result.value_;
    return result;
}

inline bool
operator!(const Integer_fake &left)
{
    return !left.value_;
}

inline Integer_fake
operator-(const Integer_fake &left)
{
    Integer_fake result = left;
    result.value_ = !result.value_;
    return result;
}

inline Integer_fake
operator+(const Integer_fake &left)
{
    Integer_fake result = left;
    return result;
}

inline Integer_fake
operator+(const Integer_fake &left, const Integer_fake &right)
{
    Integer_fake result = left;
    result += right;
    return result;
}

inline Integer_fake
operator+(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    Integer_fake result = left;
    result += rightatom;
    return result;
}

inline Integer_fake
operator+(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result += right;
    return result;
}

inline Integer_fake
operator+(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    Integer_fake result = left;
    result += rightatom;
    return result;
}

inline Integer_fake
operator+(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result += right;
    return result;
}

inline Integer_fake
operator-(const Integer_fake &left, const Integer_fake &right)
{
    Integer_fake result = left;
    result -= right;
    return result;
}

inline Integer_fake
operator-(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    Integer_fake result = left;
    result -= rightatom;
    return result;
}

inline Integer_fake
operator-(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result -= right;
    return result;
}

inline Integer_fake
operator-(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    Integer_fake result = left;
    result -= rightatom;
    return result;
}

inline Integer_fake
operator-(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result -= right;
    return result;
}

inline Integer_fake
operator*(const Integer_fake &left, const Integer_fake &right)
{
    Integer_fake result = left;
    result *= right;
    return result;
}

inline Integer_fake
operator*(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    Integer_fake result = left;
    result *= rightatom;
    return result;
}

inline Integer_fake
operator*(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result *= right;
    return result;
}

inline Integer_fake
operator*(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    Integer_fake result = left;
    result *= rightatom;
    return result;
}

inline Integer_fake
operator*(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result *= right;
    return result;
}

inline Integer_fake
operator/(const Integer_fake &left, const Integer_fake &right)
{
    Integer_fake result = left;
    result /= right;
    return result;
}

inline Integer_fake
operator/(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    Integer_fake result = left;
    result /= rightatom;
    return result;
}

inline Integer_fake
operator/(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result /= right;
    return result;
}

inline Integer_fake
operator/(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    Integer_fake result = left;
    result /= rightatom;
    return result;
}

inline Integer_fake
operator/(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result /= right;
    return result;
}

inline Integer_fake
operator%(const Integer_fake &left, const Integer_fake &right)
{
    Integer_fake result = left;
    result %= right;
    return result;
}

inline Integer_fake
operator%(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    Integer_fake result = left;
    result %= rightatom;
    return result;
}

inline Integer_fake
operator%(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result %= right;
    return result;
}

inline Integer_fake
operator%(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    Integer_fake result = left;
    result %= rightatom;
    return result;
}

inline Integer_fake
operator%(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    Integer_fake result = leftatom;
    result %= right;
    return result;
}

inline bool
operator==(const Integer_fake &left, const Integer_fake &right)
{
    return left.value_ == right.value_;
}

inline bool
operator==(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    return left.value_ == (Integer_fake::satom)rightatom;
}

inline bool
operator==(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    return (Integer_fake::satom)leftatom == right.value_;
}

inline bool
operator==(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    return left.value_ == rightatom;
}

inline bool
operator==(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    return leftatom == right.value_;
}

inline bool
operator!=(const Integer_fake &left, const Integer_fake &right)
{
    return left.value_ != right.value_;
}

inline bool
operator!=(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    return left.value_ != (Integer_fake::satom)rightatom;
}

inline bool
operator!=(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    return (Integer_fake::satom)leftatom != right.value_;
}

inline bool
operator!=(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    return left.value_ != rightatom;
}

inline bool
operator!=(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    return leftatom != right.value_;
}

inline bool
operator>(const Integer_fake &left, const Integer_fake &right)
{
    return left.value_ > right.value_;
}

inline bool
operator>(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    return left.value_ > (Integer_fake::satom)rightatom;
}

inline bool
operator>(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    return (Integer_fake::satom)leftatom > right.value_;
}

inline bool
operator>(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    return left.value_ > rightatom;
}

inline bool
operator>(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    return leftatom > right.value_;
}

inline bool
operator>=(const Integer_fake &left, const Integer_fake &right)
{
    return left.value_ >= right.value_;
}

inline bool
operator>=(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    return left.value_ >= (Integer_fake::satom)rightatom;
}

inline bool
operator>=(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    return (Integer_fake::satom)leftatom >= right.value_;
}

inline bool
operator>=(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    return left.value_ >= rightatom;
}

inline bool
operator>=(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    return leftatom >= right.value_;
}

inline bool
operator<(const Integer_fake &left, const Integer_fake &right)
{
    return left.value_ < right.value_;
}

inline bool
operator<(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    return left.value_ < (Integer_fake::satom)rightatom;
}

inline bool
operator<(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    return (Integer_fake::satom)leftatom < right.value_;
}

inline bool
operator<(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    return left.value_ < rightatom;
}

inline bool
operator<(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    return leftatom < right.value_;
}

inline bool
operator<=(const Integer_fake &left, const Integer_fake &right)
{
    return left.value_ <= right.value_;
}

inline bool
operator<=(const Integer_fake &left, const Integer_fake::atom &rightatom)
{
    return left.value_ <= (Integer_fake::satom)rightatom;
}

inline bool
operator<=(const Integer_fake::atom &leftatom, const Integer_fake &right)
{
    return (Integer_fake::satom)leftatom <= right.value_;
}

inline bool
operator<=(const Integer_fake &left, const Integer_fake::satom &rightatom)
{
    return left.value_ <= rightatom;
}

inline bool
operator<=(const Integer_fake::satom &leftatom, const Integer_fake &right)
{
    return leftatom <= right.value_;
}

inline bool
operator&&(const Integer_fake &left, const Integer_fake &right)
{
    return (bool)left && (bool)right;
}

inline bool
operator&&(const Integer_fake &left, const bool right)
{
    return (bool)left && right;
}

inline bool
operator&&(const bool left, const Integer_fake &right)
{
    return left && (bool)right;
}

inline bool
operator||(const Integer_fake &left, const Integer_fake &right)
{
    return (bool)left || (bool)right;
}

inline bool
operator||(const Integer_fake &left, const bool right)
{
    return (bool)left || right;
}

inline bool
operator||(const bool left, const Integer_fake &right)
{
    return left || (bool)right;
}

#include <unistd.h>    // FIXME:  delete this line?

inline std::ostream &
operator<<(std::ostream &s, const Integer_fake &i)    // FIXME:  needs io manips?
{
    return s << i.value_;
}

inline void
swap(Integer_fake &left, Integer_fake &right)
{
    Integer_fake::atom value = right.value_;
    right.value_ = left.value_;
    left.value_ = value;
}

inline Integer_fake
abs(const Integer_fake &other)
{
    Integer_fake result = other;
    if (result.value_ < 0) result.value_ = -result.value_;
    return result;
}

inline Integer_fake
min(const Integer_fake &left, const Integer_fake &right)
{
    if (left < right)
        return left;
    else
        return right;
}

inline Integer_fake
max(const Integer_fake &left, const Integer_fake &right)
{
    if (left > right)
        return left;
    else
        return right;
}

inline Integer_fake
fac(const Integer_fake &value)
{
    if (value < 0) throw Integer_fake::negativefactorial();
    Integer_fake result = 1;
    if (value > 1)
    {
        Integer_fake next = value;
        do
        {
            result *= next;
            --next;
        }
        while (next > 1);
    }
    return result;
}

inline Integer_fake
pow(const Integer_fake &value, const Integer_fake &exponent)
{
    if (exponent == 0) return 1;
    Integer_fake result = value;
    Integer_fake count = abs(exponent);
    for (--count; count > 0; --count)
        result *= value;
    if (exponent < 0) return 1 / result;    // always rounds to zero!
    return result;
}

inline void
divmod(const Integer_fake &dividend, const Integer_fake &divisor,
       Integer_fake &quotient, Integer_fake &remainder)
{
    quotient = dividend.value_ / divisor.value_;
    remainder = dividend.value_ % divisor.value_;
}

inline    // FIXME: probably should not be inline?
Integer_fake::Integer_fake(const char *asciiz)
{
    // FIXME: decimal string assumed, need to allow base 2, 8, 16, etc.
    // FIXME: also need to check that the base is not larger than maxatom

    value_ = 0;

    unsigned long len = 0;
    for (; asciiz[len] != 0; ++len)
        if (asciiz[len] == ' ') throw badstring();    // no spaces allowed
    if (len == 0) throw badstring();
    if (asciiz[0] == '-') throw outofrange();

    Integer_fake::satom &result = value_, digit = 0, shift, i2;
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
typedef Integer_fake Int;
#endif    // PURENUM_SETTING_INT_DEFAULT

#undef EX

#endif    // INTEGER_FAKE_H

