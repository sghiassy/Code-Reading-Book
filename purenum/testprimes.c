// ------------------
// Purenum 0.4e alpha
// ------------------
// testprimes.c
// ------------------

// an optimized prime number searching program
// used here for speed and accuracy testing of class Int


#include <iostream>
#include <iomanip>            // FIXME: why is this being included?
#include "int.h"

using namespace std;


static const Int mintotest = 1U;
static const Int maxtotest = 100000U;

int
main(int, char *[])
{
    static Int n = mintotest;
    if (n < 5U) n = 5U;
    if (n % 2U == 0U) ++n;
    static Int halfn = 0U;
    static Int a = 0U;

    if (n <= 5U)
    {
        cout << "1\n";
        cout << "2\n";
        cout << "3\n";
    }

    while (n < maxtotest)
    {
        halfn = n;
        halfn /= 3U;
        for (a = 3U; (n % a) && (a < halfn); a += 2);

        if (n % a)
        {
            cout << n << "\n";
        }

        n += 2U;
    }
}

