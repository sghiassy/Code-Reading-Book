// ------------------
// Purenum 0.4e alpha
// ------------------
// integer.h
// ------------------

// the "CONFIGURATION" section below must be correct

// see file: "integer.txt" for more info about class Integer


#ifndef INTEGER_H
#define INTEGER_H


////////////////////////////////////////////////////////////////////////////////
// CONFIGURATION (must be reconfigured for each system this is compiled on!)
// CONFIGURATION (must be reconfigured for each system this is compiled on!)
// CONFIGURATION (must be reconfigured for each system this is compiled on!)
// 
// INTEGER_H_BITS --  What is the hardware bit width of the CPU chip?  Must
//     correctly specify the bit width of INTEGER_H_DIGITTYPE (see below) or
//     this software will not work.  (the default digit type is 'unsigned int')
//     Most modern computers are 32-bit, as of November 2000.
// 
//#define INTEGER_H_BITS 8     // base 256 math (2^8)
//#define INTEGER_H_BITS 16    // base 65536 math (2^16)
#define INTEGER_H_BITS 32      // base 4294967296 math (2^32)
//#define INTEGER_H_BITS 64    // base 18446744073709551616 math (2^64)
//
// INTEGER_H_INITZERO --  For easier programming, all Integers may be
// automatically initialized to zero while being constructed, whenever the
// programmer does not specify an immediate initializer.  This prevents the
// tricky bugs which can result from the use of uninitialized values, but is
// slower.  Zeroing is the default because ease-of-use is the top priority for
// this software.  Experts who are confident of their ability to remember to
// initialize all values before use can disable this option for a (usually
// small) speed increase.
// 
#define INTEGER_H_INITZERO    // init Integers to zero during construction
// 
// INTEGER_H_DIGITTYPE --  
// 
//#define INTEGER_H_DIGITTYPE unsigned char     // type of Integer::atom
//#define INTEGER_H_DIGITTYPE unsigned short    // type of Integer::atom
#define INTEGER_H_DIGITTYPE unsigned int        // type of Integer::atom
//#define INTEGER_H_DIGITTYPE unsigned long     // type of Integer::atom
//
//#define INTEGER_H_SDIGITTYPE signed char      // type of Integer::satom
//#define INTEGER_H_SDIGITTYPE signed short     // type of Integer::satom
#define INTEGER_H_SDIGITTYPE signed int         // type of Integer::satom
//#define INTEGER_H_SDIGITTYPE signed long      // type of Integer::satom
// 
// USE FAST BIGNUM ALGORITHMS --  Should the fast, but complicated and tricky,
// code for wide multiplication and wide division be enabled?  Without enabling
// these, the simple, reliable, and very, very, very slow code is used.  Very,
// very, very slow!  Does not affect thin integer math, which is always fast.
// FIXME: remove the slow wide mul/div code before final release?
// 
#define INTEGER_H_FASTMUL    // enable fast wide multiplication code
#define INTEGER_H_FASTDIV    // enable fast wide division code, implies FASTMUL
// 
////////////////////////////////////////////////////////////////////////////////


#ifndef PURENUM_STRING_CONSTRUCTOR_EXPLICIT
#define EX
#else    // PURENUM_STRING_CONSTRUCTOR_EXPLICIT
#define EX explicit
#endif    // PURENUM_STRING_CONSTRUCTOR_EXPLICIT


#include <iostream>
#include <cstdlib>
#include <climits>     // FIXME: instead use <limits> when it becomes available
#include <cfloat>      // FIXME: instead use <limits> when it becomes available
#include <cmath>       // FIXME: instead use <limits> when it becomes available

class Integer
{
public:
    typedef INTEGER_H_DIGITTYPE atom;
    typedef INTEGER_H_SDIGITTYPE satom;
    enum sign { positive, negative };

    inline Integer();                           // see INTEGER_H_INITZERO above
    inline Integer(sign, atom);                 // init from a hardware number
    inline Integer(atom);                       // init from a hardware value
    inline Integer(satom);                      // init from a hardware value
    inline Integer(float);                      // init from a hardware value
    inline Integer(double);                     // init from a hardware value
    inline Integer(long double);                // init from a hardware value
    inline Integer(const Integer &);            // init from another Integer
EX  inline Integer(const char *ascii);          // init from a string number
    inline ~Integer();                          // destructor

    // conversion operators
    // FIXME: need signed int conversion operator
    inline operator bool() const;
    inline operator signed int() const;
    inline operator unsigned int() const;
    inline operator float() const;
    inline operator double() const;
    inline operator long double() const;

    // unary math operators (members)
    inline Integer &operator++();      // prefix
    inline Integer operator++(int);    // postfix
    inline Integer &operator--();      // prefix
    inline Integer operator--(int);    // postfix

    // binary math operators (members)
    inline Integer &operator=(const Integer &);
    inline Integer &operator=(const atom &);
    inline Integer &operator=(const satom &);
    inline Integer &operator+=(const Integer &);
    inline Integer &operator+=(const atom &);
    inline Integer &operator+=(const satom &);
    inline Integer &operator-=(const Integer &);
    inline Integer &operator-=(const atom &);
    inline Integer &operator-=(const satom &);
    inline Integer &operator*=(const Integer &);
    inline Integer &operator*=(const atom &);
    inline Integer &operator*=(const satom &);
    inline Integer &operator/=(const Integer &);
    inline Integer &operator/=(const atom &);
    inline Integer &operator/=(const satom &);
    inline Integer &operator%=(const Integer &);
    inline Integer &operator%=(const atom &);
    inline Integer &operator%=(const satom &);

    // friends: unary math operators (global functions)
    friend Integer operator~(const Integer &);
    friend bool operator!(const Integer &);
    friend Integer operator-(const Integer &);
    friend Integer operator+(const Integer &);

    // friends: binary math operators (global functions)
    friend Integer operator+(const Integer &, const Integer &);
    friend Integer operator+(const Integer &, const atom &);
    friend Integer operator+(const atom &, const Integer &);
    friend Integer operator+(const Integer &, const satom &);
    friend Integer operator+(const satom &, const Integer &);
    friend Integer operator-(const Integer &, const Integer &);
    friend Integer operator-(const Integer &, const atom &);
    friend Integer operator-(const atom &, const Integer &);
    friend Integer operator-(const Integer &, const satom &);
    friend Integer operator-(const satom &, const Integer &);
    friend Integer operator*(const Integer &, const Integer &);
    friend Integer operator*(const Integer &, const atom &);
    friend Integer operator*(const atom &, const Integer &);
    friend Integer operator*(const Integer &, const satom &);
    friend Integer operator*(const satom &, const Integer &);
    friend Integer operator/(const Integer &, const Integer &);
    friend Integer operator/(const Integer &, const atom &);
    friend Integer operator/(const atom &, const Integer &);
    friend Integer operator/(const Integer &, const satom &);
    friend Integer operator/(const satom &, const Integer &);
    friend Integer operator%(const Integer &, const Integer &);
    friend Integer operator%(const Integer &, const atom &);
    friend Integer operator%(const atom &, const Integer &);
    friend Integer operator%(const Integer &, const satom &);
    friend Integer operator%(const satom &, const Integer &);
    friend bool operator==(const Integer &, const Integer &);
    friend bool operator==(const Integer &, const atom &);
    friend bool operator==(const atom &, const Integer &);
    friend bool operator==(const Integer &, const satom &);
    friend bool operator==(const satom &, const Integer &);
    friend bool operator!=(const Integer &, const Integer &);
    friend bool operator!=(const Integer &, const atom &);
    friend bool operator!=(const atom &, const Integer &);
    friend bool operator!=(const Integer &, const satom &);
    friend bool operator!=(const satom &, const Integer &);
    friend bool operator>(const Integer &, const Integer &);
    friend bool operator>(const Integer &, const atom &);
    friend bool operator>(const atom &, const Integer &);
    friend bool operator>(const Integer &, const satom &);
    friend bool operator>(const satom &, const Integer &);
    friend bool operator>=(const Integer &, const Integer &);
    friend bool operator>=(const Integer &, const atom &);
    friend bool operator>=(const atom &, const Integer &);
    friend bool operator>=(const Integer &, const satom &);
    friend bool operator>=(const satom &, const Integer &);
    friend bool operator<(const Integer &, const Integer &);
    friend bool operator<(const Integer &, const atom &);
    friend bool operator<(const atom &, const Integer &);
    friend bool operator<(const Integer &, const satom &);
    friend bool operator<(const satom &, const Integer &);
    friend bool operator<=(const Integer &, const Integer &);
    friend bool operator<=(const Integer &, const atom &);
    friend bool operator<=(const atom &, const Integer &);
    friend bool operator<=(const Integer &, const satom &);
    friend bool operator<=(const satom &, const Integer &);
    friend bool operator&&(const Integer &, const Integer &);
    friend bool operator&&(const Integer &, const bool);
    friend bool operator&&(const bool, const Integer &);
    friend bool operator||(const Integer &, const Integer &);
    friend bool operator||(const Integer &, const bool);
    friend bool operator||(const bool, const Integer &);

    // FIXME: move these to a seperate optional file?
    // other/math functions (global functions)
    friend std::ostream &operator<<(std::ostream &, const Integer &);  // FIXME delete?
    friend void swap(Integer &, Integer &);
    friend Integer abs(const Integer &);
    friend Integer min(const Integer &, const Integer &);
    friend Integer max(const Integer &, const Integer &);
    friend Integer fac(const Integer &);
    friend Integer pow(Integer, Integer);
    friend void divmod(const Integer &dividend, const Integer &divisor,
                       Integer &quotient, Integer &remainder);

    // exceptions
    // FIXME: throw outofmemory when 'new' fails?  or not?
    class exception { };
    class divisionbyzero : public exception { };   // division by zero
    class outofmemory : public exception { };      // software integer overflow
    class outofrange : public exception { };       // hardware integer overflow
    class badstring : public exception { };        // non-numeric initializer
    class negativefactorial : public exception {}; // can't do x! for x < 0

    // ascii string input and output           FIXME: not internationalized
    char *ascii(const Integer base = 10) const;                    // out new[]
    void ascii(atom size, char *, const Integer base = 10) const;  // output
    void ascii(const char *, const Integer base = 10);             // input

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
    atom memorysize_;               // number of atoms in memory_
    bool negative_;                 // is this Integer negative?
    union optimizedmemory_          // .fast needs no heap allocation, speedy!
    {
        atom fast;                  // storage for a thin integer (one digit)
        atom *slow;                 // storage for a wide integer (many digits)
    } memory_;
    static const atom maxatom_;     // largest legal value in an Integer digit
#ifdef INTEGER_H_FASTMUL
    static const atom smallmaxatom_;    // used for wide multiply optimization
#endif    // INTEGER_H_FASTMUL

    static const signed int minsint_;
    static const signed int maxsint_;
    static const float maxfloat_;
    static const double maxdouble_;
    static const long double maxlongdouble_;

    // private thin math (single-atom) subroutines used by public functions
    inline void addatomzero(atom &left, bool &carry) const;
    inline void subatomzero(atom &left, bool &borrow) const;
    inline void addatoms(atom &left, const atom &right, bool &carry) const;
    inline void subatoms(atom &left, const atom &right, bool &borrow) const;
    inline void mulatoms(atom &left, const atom &right, atom &carry) const;
#ifdef INTEGER_H_FASTMUL
    inline void smalladdatoms(atom &leftatom, const atom &rightatom,
                              bool &carry) const;
    inline void smalladdatomzero(atom &leftatom, bool &carry) const;
#endif    // INTEGER_H_FASTMUL

    // private wide math (multi-atom) subroutines used by public functions
    inline void incinner(atom *&);
    inline void decinner(atom *&);
    inline void addinner(const Integer &, atom *&, const atom *&);
    inline void subinner(const Integer &, atom *&, const atom *&);
#ifdef INTEGER_H_FASTDIV
    inline void divbyatom(const atom &, Integer &, Integer &) const;
    inline void divinner(const Integer &, Integer &, Integer &) const;
#endif    // INTEGER_H_FASTDIV

    // private wide math (multi-atom) functions used by public functions
    void inc();                         // operator++ prefix
    void dec();                         // operator-- prefix
    void add(const Integer &);          // operator+=
    void sub(const Integer &);          // operator-=
    void mul(const Integer &);          // operator*=
    void div(const Integer &);          // operator/=
    void mod(const Integer &);          // operator%=
    bool equ(const Integer &) const;    // operator== and operator!=
    bool grt(const Integer &) const;    // operator>
    bool gre(const Integer &) const;    // operator>=
    bool let(const Integer &) const;    // operator<
    bool lee(const Integer &) const;    // operator<=
    void divmod(const Integer &, Integer &, Integer &) const;

    // private wide math (multi-atom) functions used by private functions
    inline bool largerthan(const Integer &right) const;     // size greaterthan
    inline bool largerorequal(const Integer &right) const;
    inline bool smallerthan(const Integer &right) const;    // size lessthan
    inline bool smallerorequal(const Integer &right) const;

    // private utility functions
    inline void shiftup(atom places);    // append zeros (in base: maxatom+1)
    inline void shiftdown(atom places);  // truncate places (in base: maxatom+1)

    static char convert[37];
};

// FIXME: check that all operations work even when 'right' is 'this'!

//#if 0
// functions to help measure code speed (tricks compiler into not optimizing)
void touch(const Integer &);          // empty function, for testing only
void touch(const Integer::atom &);    // empty function, for testing only
//#endif    // 0

// unary math operators (global functions)
inline Integer operator~(const Integer &left);
inline bool operator!(const Integer &left);
inline Integer operator-(const Integer &left);
inline Integer operator+(const Integer &left);

// binary math operators (global functions)
inline Integer operator+(const Integer &left, const Integer &right);
inline Integer operator+(const Integer &left, const Integer::atom &rightatom);
inline Integer operator+(const Integer::atom &leftatom, const Integer &right);
inline Integer operator+(const Integer &left, const Integer::satom &rightatom);
inline Integer operator+(const Integer::satom &leftatom, const Integer &right);
inline Integer operator-(const Integer &left, const Integer &right);
inline Integer operator-(const Integer &left, const Integer::atom &rightatom);
inline Integer operator-(const Integer::atom &leftatom, const Integer &right);
inline Integer operator-(const Integer &left, const Integer::satom &rightatom);
inline Integer operator-(const Integer::satom &leftatom, const Integer &right);
inline Integer operator*(const Integer &left, const Integer &right);
inline Integer operator*(const Integer &left, const Integer::atom &rightatom);
inline Integer operator*(const Integer::atom &leftatom, const Integer &right);
inline Integer operator*(const Integer &left, const Integer::satom &rightatom);
inline Integer operator*(const Integer::satom &leftatom, const Integer &right);
inline Integer operator/(const Integer &left, const Integer &right);
inline Integer operator/(const Integer &left, const Integer::atom &rightatom);
inline Integer operator/(const Integer::atom &leftatom, const Integer &right);
inline Integer operator/(const Integer &left, const Integer::satom &rightatom);
inline Integer operator/(const Integer::satom &leftatom, const Integer &right);
inline Integer operator%(const Integer &left, const Integer &right);
inline Integer operator%(const Integer &left, const Integer::atom &rightatom);
inline Integer operator%(const Integer::atom &leftatom, const Integer &right);
inline Integer operator%(const Integer &left, const Integer::satom &rightatom);
inline Integer operator%(const Integer::satom &leftatom, const Integer &right);
inline bool operator==(const Integer &left, const Integer &right);
inline bool operator==(const Integer &left, const Integer::atom &rightatom);
inline bool operator==(const Integer::atom &leftatom, const Integer &right);
inline bool operator==(const Integer &left, const Integer::satom &rightatom);
inline bool operator==(const Integer::satom &leftatom, const Integer &right);
inline bool operator!=(const Integer &left, const Integer &right);
inline bool operator!=(const Integer &left, const Integer::atom &rightatom);
inline bool operator!=(const Integer::atom &leftatom, const Integer &right);
inline bool operator!=(const Integer &left, const Integer::satom &rightatom);
inline bool operator!=(const Integer::satom &leftatom, const Integer &right);
inline bool operator>(const Integer &left, const Integer &right);
inline bool operator>(const Integer &left, const Integer::atom &rightatom);
inline bool operator>(const Integer::atom &leftatom, const Integer &right);
inline bool operator>(const Integer &left, const Integer::satom &rightatom);
inline bool operator>(const Integer::satom &leftatom, const Integer &right);
inline bool operator>=(const Integer &left, const Integer &right);
inline bool operator>=(const Integer &left, const Integer::atom &rightatom);
inline bool operator>=(const Integer::atom &leftatom, const Integer &right);
inline bool operator>=(const Integer &left, const Integer::satom &rightatom);
inline bool operator>=(const Integer::satom &leftatom, const Integer &right);
inline bool operator<(const Integer &left, const Integer &right);
inline bool operator<(const Integer &left, const Integer::atom &rightatom);
inline bool operator<(const Integer::atom &leftatom, const Integer &right);
inline bool operator<(const Integer &left, const Integer::satom &rightatom);
inline bool operator<(const Integer::satom &leftatom, const Integer &right);
inline bool operator<=(const Integer &left, const Integer &right);
inline bool operator<=(const Integer &left, const Integer::atom &rightatom);
inline bool operator<=(const Integer::atom &leftatom, const Integer &right);
inline bool operator<=(const Integer &left, const Integer::satom &rightatom);
inline bool operator<=(const Integer::satom &leftatom, const Integer &right);
inline bool operator&&(const Integer &left, const Integer &right);
inline bool operator&&(const Integer &left, const bool right);
inline bool operator&&(const bool left, const Integer &right);
inline bool operator||(const Integer &left, const Integer &right);
inline bool operator||(const Integer &left, const bool right);
inline bool operator||(const bool left, const Integer &right);
inline std::ostream &operator<<(std::ostream &, const Integer &);
inline void swap(Integer &, Integer &);
inline Integer abs(const Integer &);
inline Integer min(const Integer &, const Integer &);
inline Integer max(const Integer &, const Integer &);
inline Integer fac(const Integer &);
inline Integer pow(Integer, Integer);
inline void divmod(const Integer &dividend, const Integer &divisor,
                   Integer &quotient, Integer &remainder);

#ifdef INTEGER_H_INITZERO
inline
Integer::Integer()
: memorysize_(1), negative_(false)
{
    memory_.fast = 0;
}
#else
inline
Integer::Integer()
: memorysize_(1)
{
}
#endif    // INTEGER_H_INITZERO

inline
Integer::Integer(Integer::sign s, Integer::atom a)
: memorysize_(1), negative_(s == negative && a != 0)
{
    memory_.fast = a;
}

inline
Integer::Integer(atom a)
: memorysize_(1), negative_(false)
{
    memory_.fast = a;
}

inline
Integer::Integer(satom value)
: memorysize_(1), negative_(false)
{
    memory_.fast = 0;

    // do some contortions that avoid subtle overflow conditions
    // many computers fail on "x = -x" for max and min int values!
    // (yes this problem includes all Pentium chips!)

    if (value >= 0)
        for (; value > 0; --value)
            ++(*this);
    else
        for (; value < 0; ++value)
            --(*this);

    // NOTE: One of the most confusing things about signed integers is that on
    //       most systems (two's-complement systems) there are a different
    //       number of positive numbers available than negative ones.  So this
    //       constructor can not simply invert the sign of negative int numbers
    //       passed to it or it will not be portable, it must go thru
    //       contortions to preserve both the magnitude and the sign.  Imagine
    //       the case where -32768 is passed in but where the highest positive
    //       number able to be stored in a signed int is 32767 (this case
    //       happens to be true for 16-bit signed ints on most PCs) so
    //       inverting the -32768 sign will not work or could even crash.  ANSI
    //       C++ does not define the result, but on my x86 PC it happens that
    //       -(-32768) is equal to (-32768), clearly unacceptable.  Similarly
    //       for 32-bit ints... -(-2147483648) == (-2147483648).

    // FIXME: could probably be even more portable if numeric_limits worked!
}

inline
Integer::Integer(float value)
: memorysize_(1), negative_(false)
{
    memory_.fast = 0;

    Integer multiplier = Integer(maxatom_) + Integer(1);
    float flatvalue = floor(fabsf(value));
    float atomplus = float(maxatom_) + 1.0F;    // FIXME: not portable?
    if (atomplus == 0.0F) throw outofrange();    // just in case of overflow

    Integer shift = 1;
    for (; flatvalue > 0; flatvalue /= atomplus)
    {
        *this += atom(fmod(flatvalue,atomplus)) * shift;
        shift *= multiplier;
    }

    if (value < 0) negative_ = true;
}

inline
Integer::Integer(double value)
: memorysize_(1), negative_(false)
{
    memory_.fast = 0;

    Integer multiplier = Integer(maxatom_) + Integer(1);
    double flatvalue = floor(fabs(value));
    double atomplus = double(maxatom_) + 1.0F;    // FIXME: not portable?
    if (atomplus == 0.0F) throw outofrange();    // just in case of overflow

    Integer shift = 1;
    for (; flatvalue > 0; flatvalue /= atomplus)
    {
        *this += atom(fmod(flatvalue,atomplus)) * shift;
        shift *= multiplier;
    }

    if (value < 0) negative_ = true;
}

inline
Integer::Integer(long double value)
: memorysize_(1), negative_(false)
{
    memory_.fast = 0;

    Integer multiplier = Integer(maxatom_) + Integer(1);
    long double flatvalue = floor(fabsl(value));
    long double atomplus = (long double)(maxatom_) + 1.0F;    // FIXME: not portable?
    if (atomplus == 0.0F) throw outofrange();    // just in case of overflow

    Integer shift = 1;
    for (; flatvalue > 0; flatvalue /= atomplus)
    {
        *this += atom(fmod(flatvalue,atomplus)) * shift;
        shift *= multiplier;
    }

    if (value < 0) negative_ = true;
}

inline
Integer::Integer(const Integer &other)
: memorysize_(other.memorysize_), negative_(other.negative_)
{
    if (memorysize_ == 1)
        memory_.fast = other.memory_.fast;
    else    // FIXME: make a function call to this smarter logic
    {
        memory_.slow = new atom[memorysize_];
        for (atom a = 0; a < memorysize_; ++a)
            memory_.slow[a] = other.memory_.slow[a];
    }
}

inline
Integer::~Integer()
{
    // (minor) FIXME: use function call here if shorter than delete [] for x86?
    if (memorysize_ > 1) delete [] memory_.slow;
}

inline
Integer::operator bool() const
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (memory_.fast == 0)
            return false;
        else
            return true;
    }
    return true;    // zero is always stored within one memory location
}

inline
Integer::operator signed int() const
{
    if (*this > Integer(maxsint_) || *this < Integer(minsint_))
        throw outofrange();

    Integer self = *this;
    satom result = 0;

    if (!negative)
        for (; self > 0; --self)
            ++result;
    else
        for (; self < 0; ++self)
            --result;

    if (Integer(result) != *this) throw outofrange();
    return result;
// NOTE: no I don't like this function at all, but see also Integer(satom)
}

inline
Integer::operator unsigned int() const
{
    if (memorysize_ != 1) throw outofrange();
    return memory_.fast;
}

inline
Integer::operator float() const
{
    if (abs(*this) > Integer(maxfloat_)) throw outofrange();

    Integer self = *this;
    float result = 0.0F;

    if (!negative_)
        for (; self > 0; --self)
            ++result;
    else
        for (; self < 0; ++self)
            --result;

    if (Integer(result) != *this) throw outofrange();
    return result;
// NOTE: no I don't like this function at all, but see also Integer(satom)
}

inline
Integer::operator double() const
{
    if (abs(*this) > Integer(maxdouble_)) throw outofrange();

    Integer self = *this;
    double result = 0.0F;

    if (!negative_)
        for (; self > 0; --self)
            ++result;
    else
        for (; self < 0; ++self)
            --result;

    if (Integer(result) != *this) throw outofrange();
    return result;
// NOTE: no I don't like this function at all, but see also Integer(satom)
}

inline
Integer::operator long double() const
{
    if (abs(*this) > Integer(maxlongdouble_)) throw outofrange();

    Integer self = *this;
    long double result = 0.0F;

    if (!negative_)
        for (; self > 0; --self)
            ++result;
    else
        for (; self < 0; ++self)
            --result;

    if (Integer(result) != *this) throw outofrange();
    return result;
// NOTE: no I don't like this function at all, but see also Integer(satom)
}

inline Integer &
Integer::operator++()    // prefix
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (!negative_)
        {
            atom &leftatom = memory_.fast;
            if (leftatom < maxatom_)    // overflow prediction
            {
                ++leftatom;
                return *this;
            }
        }
        else
        {
            atom &leftatom = memory_.fast;
            --leftatom;
            if (leftatom == 0) negative_ = false;
            return *this;
        }
    }
    inc();    // fall back to slow multi-atom case
    return *this;
}

inline Integer
Integer::operator++(int)    // postfix
{
    Integer result = *this;
    this->operator++();
    return result;
}

inline Integer &
Integer::operator--()    // prefix
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (negative_)
        {
            atom &leftatom = memory_.fast;
            if (leftatom < maxatom_)    // overflow prediction
            {
                ++leftatom;
                return *this;
            }
        }
        else
        {
            atom &leftatom = memory_.fast;
            if (leftatom != 0)    // underflow prediction
            {
                --leftatom;
                if (leftatom == 0) negative_ = false;
                return *this;
            }
            else
            {
                leftatom = 1U;
                negative_ = true;
                return *this;
            }
        }
    }
    dec();    // fall back to slow multi-atom case
    return *this;
}

inline Integer
Integer::operator--(int)    // postfix
{
    Integer result = *this;
    this->operator--();
    return result;
}

inline Integer &
Integer::operator=(const Integer &right)
{
    if (right.memorysize_ == 1)
    {
        if (memorysize_ > 1) delete [] memory_.slow;
        memorysize_ = right.memorysize_;
        negative_ = right.negative_;
        memory_.fast = right.memory_.fast;
    }
    else    // FIXME: make a function call to this smarter logic
    {
        if (memory_.slow != right.memory_.slow)    // catch assignment to self
        {
            if (memorysize_ > 1) delete [] memory_.slow;
            memorysize_ = right.memorysize_;
            negative_ = right.negative_;
            memory_.slow = new atom[memorysize_];
            for (atom a = 0; a < memorysize_; ++a)
                memory_.slow[a] = right.memory_.slow[a];
        }
    }
    return *this;
}

inline Integer &
Integer::operator=(const atom &rightatom)
{
    if (memorysize_ > 1) delete [] memory_.slow;
    memorysize_ = 1;
    negative_ = false;

    memory_.fast = rightatom;

    return *this;
}

inline Integer &
Integer::operator=(const satom &rightatom)
{
    return *this = Integer(rightatom);
}

inline Integer &
Integer::operator+=(const Integer &right)
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (negative_ == right.negative_)
        {
            atom &leftatom = memory_.fast;
            const atom &rightatom = right.memory_.fast;
            if (leftatom <= maxatom_ - rightatom)    // overflow prediction
            {
                leftatom += rightatom;
                return *this;
            }
        }
        else
        {
            atom &leftatom = memory_.fast;
            const atom &rightatom = right.memory_.fast;
            if (leftatom >= rightatom)    // underflow prediction
            {
                leftatom -= rightatom;
                if (leftatom == 0)
                    negative_ = false;
                return *this;
            }
            else
            {
                leftatom = rightatom - leftatom;
                negative_ = !negative_;
                return *this;
            }
        }
    }
    add(right);    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator+=(const atom &rightatom)
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (!negative_)
        {
            atom &leftatom = memory_.fast;
            if (leftatom <= maxatom_ - rightatom)    // overflow prediction
            {
                leftatom += rightatom;
                return *this;
            }
        }
        else
        {
            atom &leftatom = memory_.fast;
            if (leftatom >= rightatom)    // underflow prediction
            {
                leftatom -= rightatom;
                if (leftatom == 0)
                    negative_ = false;
                return *this;
            }
            else
            {
                leftatom = rightatom - leftatom;
                negative_ = !negative_;
                return *this;
            }
        }
    }
    add(Integer(rightatom));    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator+=(const satom &rightatom)
{
    return *this += Integer(rightatom);
}

inline Integer &
Integer::operator-=(const Integer &right)
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (negative_ != right.negative_)
        {
            atom &leftatom = memory_.fast;
            const atom &rightatom = right.memory_.fast;
            if (leftatom <= maxatom_ - rightatom)    // overflow prediction
            {
                leftatom += rightatom;
                return *this;
            }
        }
        else
        {
            atom &leftatom = memory_.fast;
            const atom &rightatom = right.memory_.fast;
            if (leftatom >= rightatom)    // underflow prediction
            {
                leftatom -= rightatom;
                if (leftatom == 0)
                    negative_ = false;
                return *this;
            }
            else
            {
                leftatom = rightatom - leftatom;
                negative_ = !negative_;
                return *this;
            }
        }
    }
    sub(right);    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator-=(const atom &rightatom)
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (negative_)
        {
            atom &leftatom = memory_.fast;
            if (leftatom <= maxatom_ - rightatom)    // overflow prediction
            {
                leftatom += rightatom;
                return *this;
            }
        }
        else
        {
            atom &leftatom = memory_.fast;
            if (leftatom >= rightatom)    // underflow prediction
            {
                leftatom -= rightatom;
                if (leftatom == 0)
                    negative_ = false;
                return *this;
            }
            else
            {
                leftatom = rightatom - leftatom;
                negative_ = !negative_;
                return *this;
            }
        }
    }
    sub(Integer(rightatom));    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator-=(const satom &rightatom)
{
    return *this -= Integer(rightatom);
}

inline Integer &
Integer::operator*=(const Integer &right)
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        atom &leftatom = memory_.fast;
        const atom &rightatom = right.memory_.fast;
        if (rightatom == 0 || leftatom <= maxatom_ / rightatom)
        {                                               // overflow prediction
            leftatom *= rightatom;
            if (leftatom == 0)
                negative_ = false;
            else if (right.negative_)
                negative_ = !negative_;
            return *this;
        }
    }
    mul(right);    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator*=(const atom &rightatom)
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        atom &leftatom = memory_.fast;
        if (rightatom == 0 || leftatom <= maxatom_ / rightatom)
        {                                               // overflow prediction
            leftatom *= rightatom;
            if (leftatom == 0)
                negative_ = false;
            return *this;
        }
    }
    mul(Integer(rightatom));    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator*=(const satom &rightatom)
{
    return *this *= Integer(rightatom);
}

inline Integer &
Integer::operator/=(const Integer &right)
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        atom &leftatom = memory_.fast;
        const atom &rightatom = right.memory_.fast;
        if (rightatom != 0)    // division by zero prediction
        {
            leftatom /= rightatom;
            if (leftatom == 0)
                negative_ = false;
            else if (right.negative_)
                negative_ = !negative_;
            return *this;
        }
        else
        {
            throw divisionbyzero();
        }
    }
    div(right);    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator/=(const atom &rightatom)
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        atom &leftatom = memory_.fast;
        if (rightatom != 0)    // division by zero prediction
        {
            leftatom /= rightatom;
            if (leftatom == 0)
                negative_ = false;
            return *this;
        }
        else
        {
            throw divisionbyzero();
        }
    }
    div(Integer(rightatom));    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator/=(const satom &rightatom)
{
    return *this /= Integer(rightatom);
}

inline Integer &
Integer::operator%=(const Integer &right)
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        atom &leftatom = memory_.fast;
        const atom &rightatom = right.memory_.fast;
        if (rightatom != 0)    // division by zero prediction
        {
            leftatom %= rightatom;
            if (leftatom == 0)
                negative_ = false;
            return *this;
        }
        else
        {
            throw divisionbyzero();
        }
    }
    mod(right);    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator%=(const atom &rightatom)
{
    if (memorysize_ == 1)    // optimize for fast single-atom case
    {
        atom &leftatom = memory_.fast;
        if (rightatom != 0)    // division by zero prediction
        {
            leftatom %= rightatom;
            if (leftatom == 0)
                negative_ = false;
            return *this;
        }
        else
        {
            throw divisionbyzero();
        }
    }
    mod(Integer(rightatom));    // fall back to slow multi-atom case
    return *this;
}

inline Integer &
Integer::operator%=(const satom &rightatom)
{
    return *this %= Integer(rightatom);
}

inline Integer
operator~(const Integer &left)    // FIXME see integer.txt complement info
{
    Integer result = left;

    if (result.memorysize_ == 1)    // optimize for fast single-atom case
    {
        result.memory_.fast = ~result.memory_.fast;
        return result;
    }

    Integer::atom a = 0;
    for (; a < result.memorysize_; ++a)
        result.memory_.slow[a] = ~result.memory_.slow[a];
    return result;
}

inline bool
operator!(const Integer &left)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (!left.memory_.fast)
            return false;
        else
            return true;
    }
    Integer right = 0U;
    return !left.equ(right);    // fall back to slow multi-atom case
}

inline Integer
operator-(const Integer &left)
{
    Integer result = left;
    if (result.memorysize_ == 1 && result.memory_.fast == 0)
        result.negative_ = false;
    else
        result.negative_ = !result.negative_;
    return result;
}

inline Integer
operator+(const Integer &left)
{
    Integer result = left;
    return result;
}

inline Integer
operator+(const Integer &left, const Integer &right)
{
    Integer result = left;
    result += right;
    return result;
}

inline Integer
operator+(const Integer &left, const Integer::atom &rightatom)
{
    Integer result = left;
    result += rightatom;
    return result;
}

inline Integer
operator+(const Integer::atom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result += right;
    return result;
}

inline Integer
operator+(const Integer &left, const Integer::satom &rightatom)
{
    Integer result = left;
    result += rightatom;
    return result;
}

inline Integer
operator+(const Integer::satom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result += right;
    return result;
}

inline Integer
operator-(const Integer &left, const Integer &right)
{
    Integer result = left;
    result -= right;
    return result;
}

inline Integer
operator-(const Integer &left, const Integer::atom &rightatom)
{
    Integer result = left;
    result -= rightatom;
    return result;
}

inline Integer
operator-(const Integer::atom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result -= right;
    return result;
}

inline Integer
operator-(const Integer &left, const Integer::satom &rightatom)
{
    Integer result = left;
    result -= rightatom;
    return result;
}

inline Integer
operator-(const Integer::satom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result -= right;
    return result;
}

inline Integer
operator*(const Integer &left, const Integer &right)
{
    Integer result = left;
    result *= right;
    return result;
}

inline Integer
operator*(const Integer &left, const Integer::atom &rightatom)
{
    Integer result = left;
    result *= rightatom;
    return result;
}

inline Integer
operator*(const Integer::atom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result *= right;
    return result;
}

inline Integer
operator*(const Integer &left, const Integer::satom &rightatom)
{
    Integer result = left;
    result *= rightatom;
    return result;
}

inline Integer
operator*(const Integer::satom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result *= right;
    return result;
}

inline Integer
operator/(const Integer &left, const Integer &right)
{
    Integer result = left;
    result /= right;
    return result;
}

inline Integer
operator/(const Integer &left, const Integer::atom &rightatom)
{
    Integer result = left;
    result /= rightatom;
    return result;
}

inline Integer
operator/(const Integer::atom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result /= right;
    return result;
}

inline Integer
operator/(const Integer &left, const Integer::satom &rightatom)
{
    Integer result = left;
    result /= rightatom;
    return result;
}

inline Integer
operator/(const Integer::satom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result /= right;
    return result;
}

inline Integer
operator%(const Integer &left, const Integer &right)
{
    Integer result = left;
    result %= right;
    return result;
}

inline Integer
operator%(const Integer &left, const Integer::atom &rightatom)
{
    Integer result = left;
    result %= rightatom;
    return result;
}

inline Integer
operator%(const Integer::atom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result %= right;
    return result;
}

inline Integer
operator%(const Integer &left, const Integer::satom &rightatom)
{
    Integer result = left;
    result %= rightatom;
    return result;
}

inline Integer
operator%(const Integer::satom &leftatom, const Integer &right)
{
    Integer result = leftatom;
    result %= right;
    return result;
}

inline bool
operator==(const Integer &left, const Integer &right)
{
    if (left.memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_ != right.negative_)
            return false;
        return left.memory_.fast == right.memory_.fast;
    }
    return left.equ(right);    // fall back to slow multi-atom case
}

inline bool
operator==(const Integer &left, const Integer::atom &rightatom)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_)
            return false;
        else
            return left.memory_.fast == rightatom;
    }
    Integer right = rightatom;
    return left.equ(right);    // fall back to slow multi-atom case
}

inline bool
operator==(const Integer::atom &leftatom, const Integer &right)
{
    if (right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (right.negative_)
            return false;
        else
            return leftatom == right.memory_.fast;
    }
    Integer left = leftatom;
    return left.equ(right);    // fall back to slow multi-atom case
}

inline bool
operator==(const Integer &left, const Integer::satom &rightatom)
{
    return left == Integer(rightatom);
}

inline bool
operator==(const Integer::satom &leftatom, const Integer &right)
{
    return Integer(leftatom) == right;
}

inline bool
operator!=(const Integer &left, const Integer &right)
{
    if (left.memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_ != right.negative_)
            return true;
        return left.memory_.fast != right.memory_.fast;
    }
    return !left.equ(right);    // fall back to slow multi-atom case
}

inline bool
operator!=(const Integer &left, const Integer::atom &rightatom)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_)
            return true;
        else
            return left.memory_.fast != rightatom;
    }
    Integer right = rightatom;
    return !left.equ(right);    // fall back to slow multi-atom case
}

inline bool
operator!=(const Integer::atom &leftatom, const Integer &right)
{
    if (right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (right.negative_)
            return true;
        else
            return leftatom != right.memory_.fast;
    }
    Integer left = leftatom;
    return !left.equ(right);    // fall back to slow multi-atom case
}

inline bool
operator!=(const Integer &left, const Integer::satom &rightatom)
{
    return left != Integer(rightatom);
}

inline bool
operator!=(const Integer::satom &leftatom, const Integer &right)
{
    return Integer(leftatom) != right;
}

inline bool
operator>(const Integer &left, const Integer &right)
{
    if (left.memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_ != right.negative_)
            return !left.negative_;
        if (!left.negative_)
            return left.memory_.fast > right.memory_.fast;
        else
            return left.memory_.fast < right.memory_.fast;
    }
    return left.grt(right);    // fall back to slow multi-atom case
}

inline bool
operator>(const Integer &left, const Integer::atom &rightatom)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_)
            return false;
        else
            return left.memory_.fast > rightatom;
    }
    Integer right = rightatom;
    return left.grt(right);    // fall back to slow multi-atom case
}

inline bool
operator>(const Integer::atom &leftatom, const Integer &right)
{
    if (right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (right.negative_)
            return true;
        else
            return leftatom > right.memory_.fast;
    }
    Integer left = leftatom;
    return left.grt(right);    // fall back to slow multi-atom case
}

inline bool
operator>(const Integer &left, const Integer::satom &rightatom)
{
    return left > Integer(rightatom);
}

inline bool
operator>(const Integer::satom &leftatom, const Integer &right)
{
    return Integer(leftatom) > right;
}

inline bool
operator>=(const Integer &left, const Integer &right)
{
    if (left.memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_ != right.negative_)
            return !left.negative_;
        if (!left.negative_)
            return left.memory_.fast >= right.memory_.fast;
        else
            return left.memory_.fast <= right.memory_.fast;
    }
    return left.gre(right);    // fall back to slow multi-atom case
}

inline bool
operator>=(const Integer &left, const Integer::atom &rightatom)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_)
            return false;
        else
            return left.memory_.fast >= rightatom;
    }
    Integer right = rightatom;
    return left.gre(right);    // fall back to slow multi-atom case
}

inline bool
operator>=(const Integer::atom &leftatom, const Integer &right)
{
    if (right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (right.negative_)
            return true;
        else
            return leftatom >= right.memory_.fast;
    }
    Integer left = leftatom;
    return left.gre(right);    // fall back to slow multi-atom case
}

inline bool
operator>=(const Integer &left, const Integer::satom &rightatom)
{
    return left >= Integer(rightatom);
}

inline bool
operator>=(const Integer::satom &leftatom, const Integer &right)
{
    return Integer(leftatom) == right;
}

inline bool
operator<(const Integer &left, const Integer &right)
{
    if (left.memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_ != right.negative_)
            return left.negative_;
        if (!left.negative_)
            return left.memory_.fast < right.memory_.fast;
        else
            return left.memory_.fast > right.memory_.fast;
    }
    return left.let(right);    // fall back to slow multi-atom case
}

inline bool
operator<(const Integer &left, const Integer::atom &rightatom)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_)
            return true;
        else
            return left.memory_.fast < rightatom;
    }
    Integer right = rightatom;
    return left.let(right);    // fall back to slow multi-atom case
}

inline bool
operator<(const Integer::atom &leftatom, const Integer &right)
{
    if (right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (right.negative_)
            return false;
        else
            return leftatom < right.memory_.fast;
    }
    Integer left = leftatom;
    return left.let(right);    // fall back to slow multi-atom case
}

inline bool
operator<(const Integer &left, const Integer::satom &rightatom)
{
    return left < Integer(rightatom);
}

inline bool
operator<(const Integer::satom &leftatom, const Integer &right)
{
    return Integer(leftatom) == right;
}

inline bool
operator<=(const Integer &left, const Integer &right)
{
    if (left.memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_ != right.negative_)
            return left.negative_;
        if (!left.negative_)
            return left.memory_.fast <= right.memory_.fast;
        else
            return left.memory_.fast >= right.memory_.fast;
    }
    return left.lee(right);    // fall back to slow multi-atom case
}

inline bool
operator<=(const Integer &left, const Integer::atom &rightatom)
{
    if (left.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (left.negative_)
            return true;
        else
            return left.memory_.fast <= rightatom;
    }
    Integer right = rightatom;
    return left.lee(right);    // fall back to slow multi-atom case
}

inline bool
operator<=(const Integer::atom &leftatom, const Integer &right)
{
    if (right.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (right.negative_)
            return false;
        else
            return leftatom <= right.memory_.fast;
    }
    Integer left = leftatom;
    return left.lee(right);    // fall back to slow multi-atom case
}

inline bool
operator<=(const Integer &left, const Integer::satom &rightatom)
{
    return left <= Integer(rightatom);
}

inline bool
operator<=(const Integer::satom &leftatom, const Integer &right)
{
    return Integer(leftatom) <= right;
}

inline bool
operator&&(const Integer &left, const Integer &right)
{
    return (bool)left && (bool)right;
}

inline bool
operator&&(const Integer &left, const bool right)
{
    return (bool)left && right;
}

inline bool
operator&&(const bool left, const Integer &right)
{
    return left && (bool)right;
}

inline bool
operator||(const Integer &left, const Integer &right)
{
    return (bool)left || (bool)right;
}

inline bool
operator||(const Integer &left, const bool right)
{
    return (bool)left || right;
}

inline bool
operator||(const bool left, const Integer &right)
{
    return left || (bool)right;
}

inline std::ostream &
operator<<(std::ostream &s, const Integer &i)    // FIXME:  needs io manips?
{
    if (i.memorysize_ == 1)    // optimize for fast single-atom case
    {
        if (i.negative_)
            s << "-";
        return s << i.memory_.fast;
    }

    char *output;
    output = i.ascii();    // FIXME: this defaults allowing base 10 output only
    s << output;

    delete [] output;
    return s;


#ifdef BETTERCODE
    const Integer base = 10U;
    Integer i2, digits = 1U;
    for (i2 = i; i2 >= base; i2 /= base)    // FIXME: incorrectly optimized
        digits++;
    const unsigned int fastresultsize = 100;
    unsigned char fastresult[fastresultsize];
    unsigned char *result;
    if (digits > fastresultsize)
        result = new unsigned char[(Integer::atom)digits];
        // FIXME: previous line can go out of range
    else
        result = fastresult;
    Integer::atom place = digits;
    for (i2 = i; i2 >= base; i2 /= base)
        result[--place] = ((Integer::atom)(i2 % base));
    result[--place] = ((Integer::atom)i2);
    for (; place < digits; ++place)
        s << (unsigned char)(result[place] + '0');
    if (digits > fastresultsize)
        delete [] result;
#endif    // BETTERCODE
}

inline void
swap(Integer &left, Integer &right)
{
    bool negative = right.negative_;
    right.negative_ = left.negative_;
    left.negative_ = negative;

    Integer::atom memorysize = right.memorysize_;
    right.memorysize_ = left.memorysize_;
    left.memorysize_ = memorysize;

    Integer::optimizedmemory_ memory;
    if (memorysize == 1)
        memory.fast = right.memory_.fast;
    else
        memory.slow = right.memory_.slow;
    if (right.memorysize_ == 1)
        right.memory_.fast = left.memory_.fast;
    else
        right.memory_.slow = left.memory_.slow;
    if (left.memorysize_ == 1)
        left.memory_.fast = memory.fast;
    else
        left.memory_.slow = memory.slow;
}

inline Integer
abs(const Integer &other)
{
    Integer result = other;
    if (result.negative_) result.negative_ = false;
    return result;
}

inline Integer
min(const Integer &left, const Integer &right)
{
    if (left < right)
        return left;
    else
        return right;
}

inline Integer
max(const Integer &left, const Integer &right)
{
    if (left > right)
        return left;
    else
        return right;
}

inline Integer
fac(const Integer &value)
{
    if (value < 0) throw Integer::negativefactorial();
    Integer result = 1;
    if (value > 1)
    {
        Integer next = value;
        do
        {
            result *= next;
            --next;
        }
        while (next > 1);
    }
    return result;
}

inline Integer
pow(Integer value, Integer exponent)
{
    if (exponent.negative_) return 0;    // return 1/answer
    Integer result = 1U;                 // answer 1 if the exponent is zero
    while (exponent >= 1U)
    {
        if (exponent % 2U == 1U)         // if the exponent is odd...
        {
            result *= value;             // calculate more of the answer
            --exponent;                  // reduce the exponent to even
        }
        else                             // if the exponent is even...
        {
            value *= value;              // square the unfinished value
            exponent /= 2U;              // half the exponent
        }
    }
    return result;                       // the final answer
}

#if 0
inline Integer
pow(const Integer &value, const Integer &exponent)
{
    if (exponent == 0) return 1;
    Integer result = value;
    Integer count = abs(exponent);
    for (--count; count > 0; --count)
        result *= value;
    if (exponent < 0) return 1 / result;    // always rounds to zero!
    return result;
}
#endif    // 0

inline void
divmod(const Integer &dividend, const Integer &divisor,
       Integer &quotient, Integer &remainder)
{
    if (dividend.memorysize_ == 1 &&
        divisor.memorysize_ == 1)    // optimize for fast single-atom case
    {
        const Integer::atom &leftatom = dividend.memory_.fast;
        const Integer::atom &rightatom = divisor.memory_.fast;
        if (rightatom != 0)    // division by zero prediction
        {
            quotient = leftatom / rightatom;
            remainder = leftatom % rightatom;
            if (leftatom == 0)
                quotient.negative_ = false;
            else if (!divisor.negative_)
                quotient.negative_ = dividend.negative_;
            else
                quotient.negative_ = !dividend.negative_;
            remainder.negative_ = dividend.negative_;
            return;
        }
        else
        {
            throw Integer::divisionbyzero();
        }
    }
    dividend.divmod(divisor,quotient,remainder);    // fall back to slow case
}

inline    // FIXME: probably should not be inline?
Integer::Integer(const char *asciiz)
{
    // FIXME: decimal string assumed, need to allow base 2, 8, 16, etc.
    // FIXME: also need to check that the base is not larger than maxatom

    memorysize_ = 1;
    negative_ = false;
    memory_.fast = 0;

    unsigned long len = 0;
    for (; asciiz[len] != 0; ++len)
        if (asciiz[len] == ' ') throw badstring();    // no spaces allowed
    if (len == 0) throw badstring();

    Integer &result = *this, digit, shift, i2;
    unsigned long i = len - 1;
    Integer::atom value;
    for (;; i--)
    {
        if (asciiz[i] == '-')    // FIXME: what is this crap?
        {
            if (result.memorysize_ != 1 || result.memory_.fast != 0)
                result.negative_ = true;
            break;
        }
        value = asciiz[i] - '0';
        shift = 1;
        for (i2 = 0; i2 < digit; i2++)
            shift *= 10;    // FIXME: only works for base 10 strings
        result += Integer(value) * shift;
        ++digit;
        if (i == 0) break;
    }

//    memorysize_ = 1;
//    *this = result;
}

#ifdef PURENUM_SETTING_INT_DEFAULT
typedef Integer Int;
#endif    // PURENUM_SETTING_INT_DEFAULT

#undef EX

#endif    // INTEGER_H

