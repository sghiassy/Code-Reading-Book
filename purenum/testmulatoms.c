// testmulatoms.c


#include <iostream>

using namespace std;


typedef unsigned long atom;
atom base_ = 65536;
atom maxatom_ = base_ - 1;
atom smallbase_ = 256;
atom smallmaxatom_ = smallbase_ - 1;

inline void
smalladdatoms(atom &leftatom, const atom &rightatom, bool &carry)
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
smalladdatomzero(atom &leftatom, bool &carry)
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
mulatoms(atom &left, const atom &right, atom &carry)
{
    // convert to a smaller base
    atom smallbase = smallbase_;
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

#if 0
inline void
smalladdatoms(atom &leftatom, const atom &rightatom, bool &carry)
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
smalladdatomzero(atom &leftatom, bool &carry)
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
mulatoms(atom &left, const atom &right, atom &carry)
{
    // convert to a smaller base
    atom smallleft[2], smallright[2];
    smallleft[0] = left % smallbase_;
    smallleft[1] = left / smallbase_;
    smallright[0] = right % smallbase_;
    smallright[1] = right / smallbase_;

    // multiply left by right

    atom product[4];
    product[0] = smallleft[0] * smallright[0];
    atom P0L = product[0] % smallbase_;
    atom P0H = product[0] / smallbase_;
    product[1] = smallleft[1] * smallright[0];
    atom P1L = product[1] % smallbase_;
    atom P1H = product[1] / smallbase_;
    product[2] = smallleft[0] * smallright[1];
    atom P2L = product[2] % smallbase_;
    atom P2H = product[2] / smallbase_;
    product[3] = smallleft[1] * smallright[1];
    atom P3L = product[3] % smallbase_;
    atom P3H = product[3] / smallbase_;

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
    left = (sum[1] * smallbase_) + sum[0];
    carry = (sum[3] * smallbase_) + sum[2];
}

inline void
mulatoms(atom &left, const atom &right, atom &carry)
{
    atom smallleft[3], smallright[3], product[4], sum[4];
//    const atom smallbase = smallmaxatom_ + 1;

    // convert to a smaller base
    smallleft[0] = left % smallbase_;
    smallleft[1] = left / smallbase_;
    smallright[0] = right % smallbase_;
    smallright[1] = right / smallbase_;

    // multiply left by right
    product[0] = smallleft[0] * smallright[0];
    product[1] = smallleft[1] * smallright[0];
    product[2] = smallleft[0] * smallright[1];
    product[3] = smallleft[1] * smallright[1];

    // sum up the first product part 
    if (product[0] < smallbase_)
    {
        sum[0] = product[0];
        sum[1] = 0;
        sum[2] = 0;
        sum[3] = 0;
    }
    else    // overflow
    {
        sum[0] = product[0] % smallbase_;
        sum[1] = product[0] / smallbase_;
        sum[2] = 0;
        sum[3] = 0;
    }

    // sum up the second product part 
    if (product[1] < smallbase_)
    {
        sum[0] += 0;
        sum[1] += product[1];
        sum[2] += 0;
        sum[3] += 0;
    }
    else    // overflow
    {
        sum[0] += 0;
        sum[1] += product[1] % smallbase_;
        if (sum[1] >= smallbase_)    // overflow
        {
            sum[2] += sum[1] / smallbase_;
            sum[1] %= smallbase_;
        }
        sum[2] += product[1] / smallbase_;
        sum[3] += 0;
    }

    // sum up the third product part 
    if (product[2] < smallbase_)
    {
        sum[0] += 0;
        sum[1] += product[2];
        sum[2] += 0;
        sum[3] += 0;
    }
    else    // overflow
    {
        sum[0] += 0;
        sum[1] += product[2] % smallbase_;
        if (sum[1] >= smallbase_)    // overflow
        {
            sum[2] += sum[1] / smallbase_;
            sum[1] %= smallbase_;
        }
        sum[2] += product[2] / smallbase_;
        if (sum[2] >= smallbase_)    // overflow
        {
            sum[3] += sum[2] / smallbase_;
            sum[2] %= smallbase_;
        }
        sum[3] += 0;
    }

    // sum up the fourth product part 
    if (product[3] < smallbase_)
    {
        sum[0] += 0;
        sum[1] += 0;
        sum[2] += product[3];
        sum[3] += 0;
    }
    else    // overflow
    {
        sum[0] += 0;
        sum[1] += 0;
        sum[2] += product[3] % smallbase_;
        if (sum[2] >= smallbase_)    // overflow
        {
            sum[3] += sum[2] / smallbase_;
            sum[2] %= smallbase_;
        }
        sum[3] += product[3] / smallbase_;
    }

    // convert the final answer back to the original base
    left = sum[1];
    left *= smallbase_;
    left += sum[0];
    carry = sum[3];
    carry *= smallbase_;
    carry += sum[2];
}
#endif    // 0

int
main(int argc, char *argv[])
{
    atom left, right, carry, answer, answer2;

//    int count;
//    for (count = 0; count < 300; ++count)
//    {

    for (left = 0; left < base_; ++left)
        for (right = 0; right < base_; ++right)
        {
            answer = left;
            mulatoms(answer,right,carry);
//#if 0
            if (answer > base_) cout << "YIKES!" << endl;
            if (carry > base_) cout << "YIKES!" << endl;
            answer += carry * base_;
            answer2 = left * right;
            if (answer != answer2)
                cout << hex << left << " * " << right << " = " << answer
                     << "     ERROR  (should be " << answer2 << ")" << endl;
//            else
//                cout << hex << left << " * " << right << " = " << answer
//                     << endl;
//#endif    // 0
        }

//    }

    return 0;
}

