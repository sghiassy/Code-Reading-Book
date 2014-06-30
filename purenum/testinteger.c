// ------------------
// Purenum 0.4e alpha
// ------------------
// testinteger.c
// ------------------

// some testing procedures for class Integer
// runs thru many math operations to test for correctness


#include <iostream>
#include <strstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "int.h"

using namespace std;


void
test1()    // run thru some sample bignum tests to show that bignums work
{
    cout << endl << "class Int (Purenum 0.4e alpha)" << endl;
    cout << "biggest possible integer on this hardware is " << ~0UL << endl;

    char *bigstring1 = (char *)"5000000000";
    Int bignum = bigstring1;
    cout << "\nInt created from string \"" << bigstring1 << "\" is "
         << bignum << endl;
    ++bignum;
    cout << "Int incremented is " << bignum << endl;
    ++bignum;
    cout << "Int incremented is " << bignum << endl;
    --bignum;
    cout << "Int decremented is " << bignum << endl;
    bignum += bignum;
    cout << "Int plus itself is " << bignum << endl;

    unsigned int i1 = 1234567890;
    bignum = i1;
    cout << "\nInt reinitialized from unsigned int " << i1
         << " is " << bignum << endl;
    bignum *= 10;
    cout << "Int multiplied by 10 is " << bignum << endl;
    bignum /= 2;
    cout << "Int divided by 2 is " << bignum << endl;
    bignum /= 5;
    cout << "Int divided by 5 is " << bignum << endl;

    char *bigstring2 = (char *)"4294967296";
    bignum = bigstring2;
    cout << "\nInt reinitialized from string \"" << bigstring2 << "\" is "
         << bignum << endl;
    bignum *= bignum;
    cout << "Int squared is " << bignum << endl;
    const Int test1("10000000000");
    bignum /= test1;
    cout << "Int divided by " << test1 << " is " << bignum << endl;

    char *bigstring3 = (char *)"333333333322222222221111111111";
    bignum = bigstring3;
    cout << "\nInt from \"" << bigstring3 << "\" is "
         << bignum << endl;
         
    bignum *= 2;
    cout << "Int multiplied by 2 is " << bignum << endl;
    bignum *= bignum;
    cout << "Int squared is " << bignum << endl;

    cout << endl;
}

void
test2()    // compare bignum operations on a range of numbers to hardware ones
{
//    const int range = 255;
    const int range = 46340;
    cout << "testing bignum operations on all numbers from " << -range 
         << " thru " << range << endl;
    cout << "comparing the software results to the hardware ones" << endl;
    cout << "starting test..." << endl;

    int hard1, hard2, hard3;
    Int soft1, soft2, soft3;
    string hardstr, softstr;
    ostrstream hardostr, softostr;
//    for (hard1 = 0; hard1 < range; ++hard1)
//        for (hard2 = 0; hard2 < range; ++hard2)


    hard1 = -range;
    soft1 = Int(hard1);
    do
    {
        hard2 = -range;
        soft2 = Int(hard2);
        do
        {
            // add
            hard3 = hard1 + hard2;
            soft3 = soft1 + soft2;
//            hardostr << hard3 << ends;
//            hardstr = hardostr.str();
//            hardostr.freeze(false);
//            hardostr.seekp(0,ios::beg);
//            softostr << soft3 << ends;
//            softstr = softostr.str();
//            softostr.freeze(false);
//            softostr.seekp(0,ios::beg);
//            cout << "hardware: " << hardstr << " = " << hard1
//                 << " + " << hard2 << endl;
//            cout << "software: " << softstr << " = " << soft1
//                 << " + " << soft2 << endl;
//            if (hardstr != softstr) { cout << "FAILED!\n"; return; }
            if (hard3 != soft3) { cout << "FAILED!\n"; return; }

            // subtract
            hard3 = hard1 - hard2;
            soft3 = soft1 - soft2;
//            hardostr << hard3 << ends;
//            hardstr = hardostr.str();
//            hardostr.freeze(false);
//            hardostr.seekp(0,ios::beg);
//            softostr << soft3 << ends;
//            softstr = softostr.str();
//            softostr.freeze(false);
//            softostr.seekp(0,ios::beg);
//            cout << "hardware: " << hardstr << " = " << hard1
//                 << " - " << hard2 << endl;
//            cout << "software: " << softstr << " = " << soft1
//                 << " - " << soft2 << endl;
//            if (hardstr != softstr) { cout << "FAILED!\n"; return; }
            if (hard3 != soft3) { cout << "FAILED!\n"; return; }

            // multiply
            hard3 = hard1 * hard2;
            soft3 = soft1 * soft2;
//            hardostr << hard3 << ends;
//            hardstr = hardostr.str();
//            hardostr.freeze(false);
//            hardostr.seekp(0,ios::beg);
//            softostr << soft3 << ends;
//            softstr = softostr.str();
//            softostr.freeze(false);
//            softostr.seekp(0,ios::beg);
//            cout << "hardware: " << hardstr << " = " << hard1
//                 << " * " << hard2 << endl;
//            cout << "software: " << softstr << " = " << soft1
//                 << " * " << soft2 << endl;
//            if (hardstr != softstr) { cout << "FAILED!\n"; return; }
            if (hard3 != soft3) { cout << "FAILED!\n"; return; }

            if (hard2 != 0)
            {
                // divide
                hard3 = hard1 / hard2;
                soft3 = soft1 / soft2;
//                hardostr << hard3 << ends;
//                hardstr = hardostr.str();
//                hardostr.freeze(false);
//                hardostr.seekp(0,ios::beg);
//                softostr << soft3 << ends;
//                softstr = softostr.str();
//                softostr.freeze(false);
//                softostr.seekp(0,ios::beg);
//                cout << "hardware: " << hardstr << " = " << hard1
//                     << " / " << hard2 << endl;
//                cout << "software: " << softstr << " = " << soft1
//                     << " / " << soft2 << endl;
//                if (hardstr != softstr) { cout << "FAILED!\n"; return; }
                if (hard3 != soft3) { cout << "FAILED!\n"; return; }

                // modulo
                hard3 = hard1 % hard2;
                soft3 = soft1 % soft2;
//                hardostr << hard3 << ends;
//                hardstr = hardostr.str();
//                hardostr.freeze(false);
//                hardostr.seekp(0,ios::beg);
//               softostr << soft3 << ends;
//                softstr = softostr.str();
//                softostr.freeze(false);
//                softostr.seekp(0,ios::beg);
//                cout << "hardware: " << hardstr << " = " << hard1
//                     << " % " << hard2 << endl;
//                cout << "software: " << softstr << " = " << soft1
//                     << " % " << soft2 << endl;
//                if (hardstr != softstr) { cout << "FAILED!\n"; return; }
                if (hard3 != soft3) { cout << "FAILED!\n"; return; }
            }

//            cout << endl;

            ++soft2;
        }
        while (hard2++ < range);

        ++soft1;
cout << hard1 << " passed\n";
    }
    while (hard1++ < range);

    cout << "all tests passed sucessfully\n";
}

void
test3()    // run a heavy multiplication-intensive test
{
    Int i = 1000;
//    int loop;
//    for (loop = 0; loop < 100; ++loop)    // 100
//        for (i = 1; i <= 50; ++i)    // 50
            touch(fac(i));
//            cout << endl << i << "! = " << fac(i) << endl;
}

void
test4()    // run a heavy multiplication-intensive test
{
    const Int value = 3, exponent = 21;
    cout << "3^21 = " << pow(value,exponent) << endl;
}

void
test5()    // test the speed of divmod()
{
    const Int dividend = -10000;
    const Int divisor = -999;
    Int quotient;
    Int remainder;
    for (Int::atom loop = 0; loop < 10000000; ++loop)
    {
//        quotient = dividend / divisor;
//        remainder = dividend % divisor;
        divmod(dividend,divisor,quotient,remainder);
        touch(quotient);
        touch(remainder);
    }
    cout << dividend << " / " << divisor << " = "
         << quotient << "R" << remainder << endl;
}

int
main(int argc, char *argv[]) try
{
    test1();

    return 0;
}
catch(Int::divisionbyzero &)
{
    cout << "\ntestinteger: division by zero Int exception caught!\n\n";
}
catch(Int::outofmemory &)
{
    cout << "\ntestinteger: out of memory Int exception caught!\n\n";
}
catch(Int::outofrange &)
{
    cout << "\ntestinteger: out of range Int exception caught!\n\n";
}
catch(Int::badstring &)
{
    cout << "\ntestinteger: bad string Int exception caught!\n\n";
}
catch(Int::exception &)
{
    cout << "\ntestinteger: unknown Int exception caught!\n\n";
}
catch(...)
{
    cout << "\ntestinteger: unknown exception caught!\n\n";
}

