// ------------------
// Purenum 0.4e alpha
// ------------------
// integer.c
// ------------------

// see file: "integer.txt" for more info about class Integer

// FIXME: do the right thing even when left and/or right are actually 'this'
// FIXME: can addatoms/subatoms be optimized by changing 'diff' to complement?

// FIXME: remove the following <cstdlib> line, here for debugging only
#include <cstdlib>
#include "integer.h"

#ifdef INTEGER_H_FASTDIV
#define INTEGER_H_FASTMUL    // enable fast wide multiplication code
#endif    // INTEGER_H_FASTDIV

#ifndef INTEGER_H_BITS
#error the INTEGER_H_BITS setting must be correctly defined in file 'integer.h'
#endif

#if INTEGER_H_BITS == 8
const Integer::atom Integer::maxatom_ = 255U;                         // 8-bits
#ifdef INTEGER_H_FASTMUL
const Integer::atom Integer::smallmaxatom_ = 15U;
#endif    // INTEGER_H_FASTMUL
#endif    // INTEGER_H_BITS == 8

#if INTEGER_H_BITS == 16
const Integer::atom Integer::maxatom_ = 65535U;                       // 16-bits
#ifdef INTEGER_H_FASTMUL
const Integer::atom Integer::smallmaxatom_ = 255U;
#endif    // INTEGER_H_FASTMUL
#endif    // INTEGER_H_BITS == 16

#if INTEGER_H_BITS == 32
const Integer::atom Integer::maxatom_ = 4294967295U;                  // 32-bits
#ifdef INTEGER_H_FASTMUL
const Integer::atom Integer::smallmaxatom_ = 65535U;
#endif    // INTEGER_H_FASTMUL
#endif    // INTEGER_H_BITS == 32

#if INTEGER_H_BITS == 64
const Integer::atom Integer::maxatom_ = 18446744073709551615U;        // 64-bits
#ifdef INTEGER_H_FASTMUL
const Integer::atom Integer::smallmaxatom_ = 4294967295U;
#endif    // INTEGER_H_FASTMUL
#endif    // INTEGER_H_BITS == 64

#ifdef INTEGER_H_FASTMUL
// NOTE: for the fast, portable, multiplication code to work:
// NOTE:   1. both smallmaxatom_ and maxatom_ must be powers of two minus one
// NOTE:   2. maxatom_ must have a value of 3 or larger
// NOTE:   3. smallmaxatom_ must be ((square root of (maxatom_+1))-1)
// NOTE:                 ... round up the result if it has a fraction!
#endif    // INTEGER_H_FASTMUL

const signed int Integer::minsint_ = INT_MIN;
const signed int Integer::maxsint_ = INT_MAX;

const float Integer::maxfloat_ = FLT_MAX;
const double Integer::maxdouble_ = DBL_MAX;
const long double Integer::maxlongdouble_ = LDBL_MAX;


inline void
Integer::addatomzero(atom &leftatom, bool &carry) const
{
    if (!carry) return;
    if (leftatom != maxatom_)       // addition with previous carry
    {
        carry = false;
        ++leftatom;
    }
    else    // addition with previous carry resulting in another carry
    {
        leftatom = 0;
    }
}

inline void
Integer::subatomzero(atom &leftatom, bool &borrow) const
{
    if (!borrow) return;
    if (leftatom)       // simple subtraction with previous borrow
    {
        borrow = false;
        --leftatom;
    }
    else    // subtraction with previous borrow resulting in another borrow
    {
        leftatom = maxatom_;
    }
}

inline void
Integer::addatoms(atom &leftatom, const atom &rightatom, bool &carry) const
{
    atom diff = maxatom_;
    diff -= leftatom;
    if (!carry)     // no previous carry
    {
        if (rightatom <= diff)      // simple addition
        {
            leftatom += rightatom;
        }
        else                    
        {                       // addition resulting in a carry
            carry = true;
            leftatom = rightatom;
            leftatom -= diff;
            --leftatom;
        }
    }
    else            // previous carry
    {
        if (rightatom < diff)       // addition plus a previous carry
        {
            carry = false;
            leftatom += rightatom;
            ++leftatom;
        }
        else    // addition with previous carry resulting in another carry
        {
            leftatom = rightatom;
            leftatom -= diff;
        }
    }
}

inline void
Integer::subatoms(atom &leftatom, const atom &rightatom, bool &borrow) const
{
    atom diff = leftatom;
    if (!borrow)     // no previous borrow
    {
        if (rightatom <= diff)      // simple subtraction
        {
            leftatom -= rightatom;
        }
        else                    
        {                       // subtraction resulting in a borrow
            borrow = true;
            leftatom = maxatom_;
            leftatom -= rightatom;
            leftatom += diff;
            ++leftatom;
        }
    }
    else            // previous borrow
    {
        if (rightatom < diff)       // simple subtraction with previous borrow
        {
            borrow = false;
            --leftatom;
            leftatom -= rightatom;
        }
        else    // subtraction with previous borrow resulting in another borrow
        {
            leftatom = maxatom_;
            leftatom -= rightatom;
            leftatom += diff;
        }
    }
}

#ifdef INTEGER_H_FASTMUL
inline void
Integer::smalladdatoms(atom &leftatom, const atom &rightatom, bool &carry)
    const
{
    register atom diff = smallmaxatom_;
    diff -= leftatom;
    if (!carry)     // no previous carry
    {
        if (rightatom <= diff)      // simple addition
        {
            leftatom += rightatom;
        }
        else                    
        {                       // addition resulting in a carry
            carry = true;
            leftatom = rightatom;
            leftatom -= diff;
            --leftatom;
        }
    }
    else            // previous carry
    {
        if (rightatom < diff)       // addition plus a previous carry
        {
            carry = false;
            leftatom += rightatom;
            ++leftatom;
        }
        else    // addition with previous carry resulting in another carry
        {
            leftatom = rightatom;
            leftatom -= diff;
        }
    }
}

inline void
Integer::smalladdatomzero(atom &leftatom, bool &carry) const
{
    if (!carry) return;
    if (leftatom != smallmaxatom_)       // addition with previous carry
    {
        carry = false;
        ++leftatom;
    }
    else    // addition with previous carry resulting in another carry
    {
        leftatom = 0;
    }
}

inline void
Integer::mulatoms(atom &left, const atom &right, atom &carry) const
{
    // convert to a smaller base
    atom smallbase = smallmaxatom_ + 1;
    atom smallleft[2], smallright[2];
    smallleft[0] = left % smallbase;
    smallleft[1] = left / smallbase;
    smallright[0] = right % smallbase;
    smallright[1] = right / smallbase;

    // multiply left by right

    atom product[4];
    product[0] = smallleft[0] * smallright[0];
    atom P0L = product[0] % smallbase;
    atom P0H = product[0] / smallbase;
    product[1] = smallleft[1] * smallright[0];
    atom P1L = product[1] % smallbase;
    atom P1H = product[1] / smallbase;
    product[2] = smallleft[0] * smallright[1];
    atom P2L = product[2] % smallbase;
    atom P2H = product[2] / smallbase;
    product[3] = smallleft[1] * smallright[1];
    atom P3L = product[3] % smallbase;
    atom P3H = product[3] / smallbase;

    bool carry2 = false;
    atom sum[4];
    sum[0] = 0;
    smalladdatoms(sum[0], P0L, carry2);
    sum[1] = 0;
    smalladdatoms(sum[1], P0H, carry2);
    smalladdatoms(sum[1], P1L, carry2);
    sum[2] = 0;
    smalladdatomzero(sum[2], carry2);
    smalladdatoms(sum[1], P2L, carry2);
    smalladdatomzero(sum[2], carry2);
    sum[3] = 0;
    smalladdatomzero(sum[3], carry2);
    smalladdatoms(sum[2], P1H, carry2);
    smalladdatomzero(sum[3], carry2);
    smalladdatoms(sum[2], P2H, carry2);
    smalladdatomzero(sum[3], carry2);
    smalladdatoms(sum[2], P3L, carry2);
    smalladdatomzero(sum[3], carry2);
    smalladdatoms(sum[3], P3H, carry2);

    // convert the final answer back to the original base
    left = (sum[1] * smallbase) + sum[0];
    carry = (sum[3] * smallbase) + sum[2];
}
#endif    // INTEGER_H_FASTMUL

inline void
Integer::incinner(atom *&leftmemory)
{
    bool flow = true;
    atom digit;

    for (digit = 0; digit < memorysize_; ++digit)
        addatomzero(leftmemory[digit],flow);

    if (flow)    // overflow, so allocate one more atom of memory
    {
        atom *newmemory = new atom[memorysize_ + 1];
        atom a = 0;
        for (; a < memorysize_; ++a)
            newmemory[a] = leftmemory[a];
        newmemory[a] = 1;
        if (memorysize_ > 1) delete [] leftmemory;
        ++memorysize_;
        leftmemory = memory_.slow = newmemory;
    }
}

inline void
Integer::decinner(atom *&leftmemory)
{
    bool flow = true;
    atom digit;

    for (digit = 0; digit < memorysize_; ++digit)
        subatomzero(leftmemory[digit],flow);

    for (--digit; digit > 0 && leftmemory[digit] == 0; --digit)
        ;    // find the most significant nonzero digit
    if (++digit != memorysize_)    // trim any leading zeros off of left
    {
        memorysize_ = digit;
        if (memorysize_ > 1)
        {
            atom *newmemory = new atom[memorysize_];
            atom a = 0;
            for (; a < memorysize_; ++a)
                newmemory[a] = leftmemory[a];
            delete [] leftmemory;
            leftmemory = memory_.slow = newmemory;
        }
        else    // (memorysize_ == 1)
        {
            memory_.fast = leftmemory[0];
            delete [] leftmemory;
            leftmemory = &memory_.fast;
        }
    }
}

inline void
Integer::addinner(const Integer &right, atom *&leftmemory,
                  const atom *&rightmemory)
{
    bool flow = false;
    atom digit;

    if (memorysize_ >= right.memorysize_)    // add
    {
        for (digit = 0; digit < right.memorysize_; ++digit)
            addatoms(leftmemory[digit],rightmemory[digit],flow);
        for (; digit < memorysize_; ++digit)
            addatomzero(leftmemory[digit],flow);
    }
    else                                     // widen left, add
    {
        atom *newmemory = new atom[right.memorysize_];
        atom a = 0;
        for (; a < memorysize_; ++a)
            newmemory[a] = leftmemory[a];
        for (; a < right.memorysize_; ++a)
            newmemory[a] = 0;
        if (memorysize_ > 1) delete [] leftmemory;
        memorysize_ = right.memorysize_;
        leftmemory = memory_.slow = newmemory;

        for (digit = 0; digit < memorysize_; ++digit)
            addatoms(leftmemory[digit],rightmemory[digit],flow);
    }

    if (flow)    // widen left, add overflow
    {
        atom *newmemory = new atom[memorysize_ + 1];
        atom a = 0;
        for (; a < memorysize_; ++a)
            newmemory[a] = leftmemory[a];
        newmemory[a] = 0;
        if (memorysize_ > 1) delete [] leftmemory;
        ++memorysize_;
        leftmemory = memory_.slow = newmemory;

        addatomzero(leftmemory[digit],flow);
    }
}

inline void
Integer::subinner(const Integer &right, atom *&leftmemory,
                  const atom *&rightmemory)
{
    bool flow = false;
    atom digit;

    if (memorysize_ >= right.memorysize_)    // subtract
    {
        for (digit = 0; digit < right.memorysize_; ++digit)
            subatoms(leftmemory[digit],rightmemory[digit],flow);
        for (; digit < memorysize_; ++digit)
            subatomzero(leftmemory[digit],flow);
    }
    else                                     // widen left, subtract
    {
        atom *newmemory = new atom[right.memorysize_];
        atom a = 0;
        for (; a < memorysize_; ++a)
            newmemory[a] = leftmemory[a];
        for (; a < right.memorysize_; ++a)
            newmemory[a] = 0;
        if (memorysize_ > 1) delete [] leftmemory;
        memorysize_ = right.memorysize_;
        leftmemory = memory_.slow = newmemory;

        for (digit = 0; digit < memorysize_; ++digit)
            subatoms(leftmemory[digit],rightmemory[digit],flow);
    }

    if (flow)    // underflow, complement+1 because subtraction changed the sign
    {
        negative_ = !negative_;
        atom a = 0;
        flow = true;
        for (; a < memorysize_; ++a)
        {
            leftmemory[a] = ~leftmemory[a];
            addatoms(leftmemory[a],0,flow);
        }
    }

    for (--digit; digit > 0 && leftmemory[digit] == 0; --digit)
        ;    // find the most significant nonzero digit
    if (++digit != memorysize_)    // trim any leading zeros off of left
    {
        memorysize_ = digit;
        if (memorysize_ > 1)
        {
            atom *newmemory = new atom[memorysize_];
            atom a = 0;
            for (; a < memorysize_; ++a)
                newmemory[a] = leftmemory[a];
            delete [] leftmemory;
            leftmemory = memory_.slow = newmemory;
        }
        else    // (memorysize_ == 1)
        {
            memory_.fast = leftmemory[0];
            delete [] leftmemory;
            leftmemory = &memory_.fast;
        }
    }

    if (memorysize_ == 1 && leftmemory[0] == 0)    // catch negative zeros
        negative_ = false;
}

void
Integer::inc()    // called by operator++ prefix
{
    atom *leftmemory;                    // locate the left number in memory
    if (memorysize_ == 1)
        leftmemory = &memory_.fast;
    else
        leftmemory = memory_.slow;

    if (!negative_)
        incinner(leftmemory);
    else
        decinner(leftmemory);
}

void
Integer::dec()    // called by operator-- prefix
{
    atom *leftmemory;                    // locate the left number in memory
    if (memorysize_ == 1)
        leftmemory = &memory_.fast;
    else
        leftmemory = memory_.slow;

    if (negative_)
        incinner(leftmemory);
    else
        decinner(leftmemory);
}

void
Integer::add(const Integer &right)    // called by operator+=
{
    atom *leftmemory;                    // locate the left number in memory
    if (memorysize_ == 1)
        leftmemory = &memory_.fast;
    else
        leftmemory = memory_.slow;

    const atom *rightmemory;             // locate the right number in memory
    if (right.memorysize_ == 1)
        rightmemory = &right.memory_.fast;
    else
        rightmemory = right.memory_.slow;

    if (negative_ == right.negative_)    // add the absolute values
        addinner(right,leftmemory,rightmemory);
    else                                 // subtract the absolute values
        subinner(right,leftmemory,rightmemory);
}

void
Integer::sub(const Integer &right)    // called by operator-=
{
    atom *leftmemory;                    // locate the atoms in memory
    if (memorysize_ == 1)
        leftmemory = &memory_.fast;
    else
        leftmemory = memory_.slow;

    const atom *rightmemory;             // locate the atoms in memory
    if (right.memorysize_ == 1)
        rightmemory = &right.memory_.fast;
    else
        rightmemory = right.memory_.slow;

    if (negative_ != right.negative_)    // add the absolute values
        addinner(right,leftmemory,rightmemory);
    else                                 // subtract the absolute values
        subinner(right,leftmemory,rightmemory);
}

inline void
Integer::shiftup(atom places)
{
    if (places == 0) return;

    atom newmemorysize = memorysize_;
    optimizedmemory_ newmemory;
    bool carry = false;
    addatoms(newmemorysize,places,carry);
    if (carry) throw outofmemory();    // wow that's a big big big big integer

    atom count = memorysize_;
    atom newcount = newmemorysize;

    if (memorysize_ == 1)
    {
        if (memory_.fast == 0) return;    // zero shifted up is still zero
        newmemory.slow = new atom[newmemorysize];
        --newcount;
        newmemory.slow[newcount] = memory_.fast;
    }
    else
    {
        newmemory.slow = new atom[newmemorysize];
        do
        {
            --count;
            --newcount;
            newmemory.slow[newcount] = memory_.slow[count];
        }
        while (count > 0);
        delete [] memory_.slow;
    }

//cout << "shifting " << places << " places\n";
    do
    {
        --newcount;
        newmemory.slow[newcount] = 0;
    }
    while(newcount > 0);
    memory_.slow = newmemory.slow;
    memorysize_ = newmemorysize;
}

inline void
Integer::shiftdown(atom places)
{
    if (places >= memorysize_)
    {
        *this = 0U;
        return;
    }
    if (places == 0) return;

    atom newmemorysize = memorysize_;
    optimizedmemory_ newmemory;
    bool borrow = false;
    subatoms(newmemorysize,places,borrow);

    atom count = memorysize_;
    atom newcount = newmemorysize;

    if (newmemorysize == 1)
    {
        --count;
        newmemory.fast = memory_.slow[count];
        delete [] memory_.slow;
        memory_.fast = newmemory.fast;
        memorysize_ = newmemorysize;
    }
    else
    {
        newmemory.slow = new atom[newmemorysize];
        do
        {
            --count;
            --newcount;
            newmemory.slow[newcount] = memory_.slow[count];
        }
        while (newcount > 0);
        delete [] memory_.slow;
        memory_.slow = newmemory.slow;
        memorysize_ = newmemorysize;
    }
}

void
Integer::mul(const Integer &right)    // called by operator*=
{
    bool leftnegative = negative_;
    if (right.negative_) leftnegative = !leftnegative;

#ifdef INTEGER_H_FASTMUL
    const atom *leftmemory;              // locate the atoms in memory
    if (memorysize_ == 1)
        leftmemory = &memory_.fast;
    else
        leftmemory = memory_.slow;

    const atom *rightmemory;             // locate the atoms in memory
    if (right.memorysize_ == 1)
        rightmemory = &right.memory_.fast;
    else
        rightmemory = right.memory_.slow;

    Integer result, temp;
    Integer base = 1;
    base.shiftup(1);
    atom leftcount, rightcount, placecount1, placecount2, mult, over;
    placecount1 = 0;
    for (rightcount = 0; rightcount < right.memorysize_; rightcount++)
    {
        placecount2 = placecount1;
        for (leftcount = 0; leftcount < memorysize_; leftcount++)
        {
            mult = leftmemory[leftcount];
            mulatoms(mult,rightmemory[rightcount],over);
            temp = over;
            if (temp != 0U)
            {
                temp.shiftup(1);
            }
            temp += mult;
            temp.shiftup(placecount2);
            result += temp;
            ++placecount2;    // FIXME: possible (rare) overflow here?
        }
        ++placecount1;    // FIXME: possible (rare) overflow here?
    }

    // FIXME: is the sign correct at this point?

    *this = result;

#else

    Integer remain, repeat;
    if (right <= *this)
    {
        remain = *this;
        repeat = right;
    }
    else
    {
        remain = right;
        repeat = *this;
    }
    remain.negative_ = false;
    repeat.negative_ = false;
    *this = 0;

    for (; repeat > 0U; --repeat)    // FIXME: dumb and slow but it works
        *this += remain;
#endif    // INTEGER_H_FASTMUL

    if (memorysize_ == 1 && memory_.fast == 0)
        negative_ = false;
    else
        negative_ = leftnegative;
}

#ifdef INTEGER_H_FASTDIV
inline void
Integer::divbyatom(const atom &rightatom, Integer &quotient,
                   Integer &remainder) const
{    // FIXME: divbyatom() does not correctly handle signs yet
//cout << "/starting divbyatom/\n" << flush;
    // (#1 #2 #3 #4)  initalize working variables for long division
    const Integer &dividend = *this;
    const atom &divisor = rightatom;
    atom place = memorysize_, subplace;
    atom digit;
    quotient = 0U;
    remainder = 0U;
    Integer subremainder = 0U;
    Integer subquotient;
//cout << "&dividend" << dividend << "&divisor" << divisor << "&\n";

    // (#4a)  divide the atomic base by the divisor
    atom frac, fracr;
    frac = maxatom_ / divisor;
    fracr = maxatom_ % divisor;
    ++fracr;
    if (fracr == divisor)    // normalize (can not be greater than divisor)
    {
        ++frac;
        fracr = 0;
    }
//    bool carry = false;
//    addatoms(fracr,1,carry);
//    if (carry) ++frac;
//cout << "&frac" << frac << "&fracr" << fracr << "&\n";

    // (#10)  repeat for each digit in divisor
    do
    {
        // (#9)  get highest unprocessed digit of dividend
        --place;
        subplace = place;
        if (dividend.memorysize_ == 1)
            digit = dividend.memory_.fast;
        else
            digit = dividend.memory_.slow[subplace];
//cout << "&digit" << digit << "&\n";

        // (#5)  divide digit by divisor and add result into quotient
        subquotient = digit / divisor;
//        subquotient.memory_.fast = digit / divisor;  // FIXME: okay?
        subremainder = digit % divisor;
//cout << " !!! subplace: " << subplace << "\n" << flush;
//cout << " !!! subquotient.memorysize_: " << subquotient.memorysize_ << "\n" << flush;
        subquotient.shiftup(subplace);
//cout << " !!! subquotient.memorysize_: " << subquotient.memorysize_ << "\n" << flush;
//cout << " &subquotient" << subquotient << "&\n";
//cout << " &subremainder" << subremainder << "&\n";
        quotient += subquotient;
//cout << " &quotient" << quotient << "&\n";

        // (#7)  calculate the fractional portion of (digit / divisor)
        atom newfrac, newfracr;
        atom overflow, overflowr;
//cout << " &starting fraction&\n";
        while (subplace > 0)
        {
//cout << "  &fraction place " << subplace << "&\n";
            // (#6)  multiply frac,fracr by subremainder and add to quotient

            newfrac = frac;
            newfracr = fracr;
//cout << "  setting &newfrac" << newfrac << "&newfracr" << newfracr << "&\n";
            overflow = 0;
            overflowr = 0;
            bool carry = false;
//            newfrac *= subremainder;
//            newfracr *= subremainder;
            mulatoms(newfrac,subremainder.memory_.fast,overflow);
            mulatoms(newfracr,subremainder.memory_.fast,overflowr);
            addatoms(newfrac,overflowr,carry);  // FIXME: verify cannot carry?

//cout << "  &BEFORE newfrac" << newfrac << "&newfracr" << newfracr << "&\n";
            for (; newfracr >= divisor; )    // normalize
            {
//cout << "  &newfrac" << newfrac << "&newfracr" << newfracr << "&\n";
                ++newfrac;
                newfracr -= divisor;
            }
//cout << "  &AFTER newfrac" << newfrac << "&newfracr" << newfracr << "&\n";

//            if (newfracr >= divisor)    // normalize
//            {
//                newfrac += (newfracr / divisor);
//                newfracr %= divisor;
//            }

            --subplace;
            subquotient = newfrac;
            subremainder = newfracr;
            subquotient.shiftup(subplace);
//cout << "  &subquotient" << subquotient << "&\n";
//cout << "  &subremainder" << subremainder << "&\n";
            quotient += subquotient;
//cout << "  &quotient" << quotient << "&\n";
        }

        // (#8)  add subremainder to the remainder, handling fractions >= 1
        bool carry = false;    // FIXME: verify that these addatoms never carry?
        if (remainder.memorysize_ == 1)
            addatoms(remainder.memory_.fast,subremainder.memory_.fast,carry);
        else
            addatoms(remainder.memory_.slow[0],subremainder.memory_.fast,carry);
        if (remainder >= divisor)
        {
            quotient += remainder / divisor;
            remainder %= divisor;
        }
//cout << " &remainder" << remainder << "&\n";
    }
    while (place > 0);
//cout << "/done with divbyatom/\n" << flush;
}
#endif    // INTEGER_H_FASTDIV

#ifdef INTEGER_H_FASTDIV
inline void
Integer::divinner(const Integer &right, Integer &quotient, Integer &remainder)
                                                                          const
{
//cout << "/starting divinner/\n" << flush;

    const Integer &dividend = *this;    // quantity to be divided (left side)
    const Integer &divisor = right;     // quantity divided by (right side)

    // thin divisor, wide dividend
    if (divisor.memorysize_ == 1)
    {
//cout << "THIN DIVISOR" << "\n" << flush;
        divbyatom(divisor.memory_.fast,quotient,remainder);
//cout << "/done with divinner/\n" << flush;
        return;
    }

    // wide divisor, wide dividend
//cout << "WIDE DIVISOR" << "\n" << flush;
    quotient = 0U;
    remainder = 0U;
    Integer subquotient, discard;
    atom round, shift;
    remainder = dividend;
    while (remainder >= divisor)
    {
        // round divisor up to nearest atom followed by all zeros (A)
        shift = divisor.memorysize_ - 1;
        round = divisor.memory_.slow[shift];
        if (round < maxatom_)
        {
            ++round;
        }
        else
        {
            round = 1;
            ++shift;
        }
        // FIXME: check if rounded > dividend... might hang?

        // divide for an approximate quotient <= to actual quotient (B)
        // FIXME: don't discard remainder from approximate division?
        remainder.divbyatom(round,subquotient,discard);
        subquotient.shiftdown(shift);
        quotient += subquotient;

        // multiply for an approximate dividend <= the actual dividend (C)
        subquotient *= divisor;
        remainder -= subquotient;   // subtract approx. divid. from remainder
    }

//cout << "/done with divinner/\n" << flush;
}
#endif    // INTEGER_H_FASTDIV

void
Integer::div(const Integer &right)    // called by operator/=
{
    if (right == 0U) throw divisionbyzero();

#ifdef INTEGER_H_FASTDIV

//cout << "/starting fastdiv/\n" << flush;

    Integer quotient, remainder;
    divinner(right, quotient, remainder);
    *this = quotient;

//cout << "/done with fastdiv/\n" << flush;

#else

    bool leftnegative = negative_;
    if (right.negative_) leftnegative = !leftnegative;
    negative_ = false;

    Integer remain = *this;
    *this = 0;
    Integer right2 = right;
    right2.negative_ = false;

    for (; remain >= right2; ++(*this))    // FIXME: dumb and slow but it works
        remain -= right2;

    if (memorysize_ == 1 && memory_.fast == 0)
        negative_ = false;
    else
        negative_ = leftnegative;
#endif    // INTEGER_H_FASTDIV
}

void
Integer::mod(const Integer &right)    // called by operator%=
{
    if (right == 0U) throw divisionbyzero();

#ifdef INTEGER_H_FASTDIV

    Integer quotient, remainder;
    divinner(right, quotient, remainder);
    *this = remainder;

#else

    bool leftnegative = negative_;
//    if (right.negative_) leftnegative = !leftnegative;
    negative_ = false;

    Integer repeat = 0U;
    Integer right2 = right;
    right2.negative_ = false;

    for (; *this >= right2; ++repeat)    // FIXME: dumb and slow but it works
        *this -= right2;

    if (memorysize_ == 1 && memory_.fast == 0)
        negative_ = false;
    else
        negative_ = leftnegative;

#endif    // INTEGER_H_FASTDIV
}

void
Integer::divmod(const Integer &right, Integer &quotient, Integer &remainder)
                                                                          const
{
#ifdef INTEGER_H_FASTDIV

    if (right == 0U) throw divisionbyzero();
    divinner(right, quotient, remainder);

#else

    if (right == 0U) throw divisionbyzero();
    const Integer &dividend = *this;
    const Integer &divisor = right;

    const Integer magdividend = abs(dividend);
    const Integer magdivisor = abs(divisor);
    quotient = 0;
    remainder = magdividend;

    for (; remainder >= divisor; ++quotient)    // FIXME: slow (but it works)
        remainder -= divisor;

    if (!divisor.negative_)    // dividing by a positive number
    {
        quotient.negative_ = dividend.negative_;
        remainder.negative_ = dividend.negative_;
    }
    else                       // dividing by a negative number
    {
        quotient.negative_ = !dividend.negative_;
        remainder.negative_ = dividend.negative_;
    }

#endif    // INTEGER_H_FASTDIV
}


inline bool
Integer::largerthan(const Integer &right) const
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
        return memory_.fast > right.memory_.fast;
    // FIXME move the following into a largerthanwide() function?
    if (memorysize_ != right.memorysize_)
        return memorysize_ > right.memorysize_;
    atom a = memorysize_;
    for (--a; a > 0; --a)
        if (memory_.slow[a] != right.memory_.slow[a])
            return memory_.slow[a] > right.memory_.slow[a];
    return memory_.slow[a] > right.memory_.slow[a];
}

inline bool
Integer::largerorequal(const Integer &right) const
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
        return memory_.fast >= right.memory_.fast;
    // FIXME move the following into a largerorequalwide() function?
    if (memorysize_ != right.memorysize_)
        return memorysize_ > right.memorysize_;
    atom a = memorysize_;
    for (--a; a > 0; --a)
        if (memory_.slow[a] != right.memory_.slow[a])
            return memory_.slow[a] > right.memory_.slow[a];
    return memory_.slow[a] >= right.memory_.slow[a];
}

inline bool
Integer::smallerthan(const Integer &right) const
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
        return memory_.fast < right.memory_.fast;
    // FIXME move the following into a smallerthanwide() function?
    if (memorysize_ != right.memorysize_)
        return memorysize_ < right.memorysize_;
    atom a = memorysize_;
    for (--a; a > 0; --a)
        if (memory_.slow[a] != right.memory_.slow[a])
            return memory_.slow[a] < right.memory_.slow[a];
    return memory_.slow[a] < right.memory_.slow[a];
}

inline bool
Integer::smallerorequal(const Integer &right) const
{
    if (memorysize_ == 1 &&
        right.memorysize_ == 1)    // optimize for fast single-atom case
        return memory_.fast <= right.memory_.fast;
    // FIXME move the following into a smallerorequalwide() function?
    if (memorysize_ != right.memorysize_)
        return memorysize_ < right.memorysize_;
    atom a = memorysize_;
    for (--a; a > 0; --a)
        if (memory_.slow[a] != right.memory_.slow[a])
            return memory_.slow[a] < right.memory_.slow[a];
    return memory_.slow[a] <= right.memory_.slow[a];
}

bool
Integer::equ(const Integer &right) const    // called by operator== operator!=
{
    if (memorysize_ != right.memorysize_)
        return false;
    if (negative_ != right.negative_)
        return false;
    atom a = memorysize_;
    for (--a; ; --a)
    {
        if (memory_.slow[a] != right.memory_.slow[a])
            return false;
        if (a == 0) return true;
    }
}

bool
Integer::grt(const Integer &right) const    // called by operator>
{
    if (negative_ != right.negative_)
        return !negative_;
    if (!negative_)
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ > right.memorysize_;
        return largerthan(right);
    }
    else
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ < right.memorysize_;
        return smallerthan(right);
    }
}

bool
Integer::gre(const Integer &right) const    // called by operator>=
{
    // FIXME move the following into a grewide() function?
    if (negative_ != right.negative_)
        return !negative_;
    if (!negative_)
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ > right.memorysize_;
        return largerorequal(right);
    }
    else
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ < right.memorysize_;
        return smallerorequal(right);
    }
}

bool
Integer::let(const Integer &right) const    // called by operator<
{
    if (negative_ != right.negative_)
        return negative_;
    if (!negative_)
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ < right.memorysize_;
        return smallerthan(right);
    }
    else
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ > right.memorysize_;
        return largerthan(right);
    }
}

bool
Integer::lee(const Integer &right) const    // called by operator<=
{
    // FIXME move the following into a letwide() function?
    if (negative_ != right.negative_)
        return negative_;
    if (!negative_)
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ < right.memorysize_;
        return smallerorequal(right);
    }
    else
    {
        if (memorysize_ != right.memorysize_)
            return memorysize_ > right.memorysize_;
        return largerorequal(right);
    }
}

char
Integer::convert[37]
    = "0123456789abcdefghijklmnopqrstuvwxyz";

char *
Integer::ascii(Integer base) const
{
    if (base < 2U || base > 36U) throw outofrange();

    Integer::atom digits = 1;
    Integer i2;
    if (negative_) ++digits;
    for (i2 = *this; i2 >= base; i2 /= base)
        digits++;

    char *buffer;
    buffer = new char[digits + 1];
    buffer[digits] = 0;    // terminator for ASCIIZ string

    Integer::atom place = digits;
    for (i2 = *this; i2 >= base; i2 /= base)
        buffer[--place] = (Integer::atom)convert[(Integer::atom)(i2 % base)];
    buffer[--place] = (Integer::atom)convert[(Integer::atom)i2];
    if (negative_) buffer[--place] = ((int)'-');
    return buffer;
}

void
Integer::ascii(atom size, char *buffer, Integer base) const
{
    if (base < 2 || base > 36) throw outofrange();
    if (size < 1) throw outofrange();

    Integer::atom digits = 1;
    Integer i2;
    if (negative_) ++digits;
    for (i2 = *this; i2 >= base; i2 /= base)
        digits++;
    if (digits >= size) throw badstring();
    buffer[digits] = 0;

    Integer::atom place = digits;
    for (i2 = *this; i2 >= base; i2 /= base)
        buffer[--place] = (Integer::atom)convert[i2 % base];
    buffer[--place] = (Integer::atom)convert[i2];
    if (negative_) buffer[--place] = '-';
}

void
Integer::ascii(const char *, Integer base)    // FIXME: empty function
{
    if (base < 2 || base > 36) throw outofrange();
}

#if 0
// Output this integer as an ASCII string.
// Conversion to any base is done automatically.
char *
Integer::string(atom base) const
{
    // FIXME: hacked to produce decimal text output
    printf("memorysize_ = %u\n",memorysize_);
    fflush(stdout);
    if (negative_)
    {
        printf("-");
        fflush(stdout);
    }
    else
    {
        printf("+");
        fflush(stdout);
    }
    atom digit;
    for (digit = memorysize_ - 1; ; --digit)
    {
        if (memory_.slow[digit] <= 9)
            printf("%u",memory_.slow[digit]);
        else
            printf("X");
        if (digit == 0) break;
    }
    printf("\n");
    fflush(stdout);
    return 0;
}
#endif    // 0

//#if 0
void
touch(const Integer::atom &)
{
}

void
touch(const Integer &)
{
}
//#endif    // 0

