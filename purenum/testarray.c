// ------------------
// Purenum 0.4e alpha
// ------------------
// testarray.c
// ------------------


#include <iostream>
#include "array.h"

using namespace std;


int
main(int argc, char *argv[]) try
{
    Array array;           // too bad that we can't just do Int array[4][4][4]
                           // see the comments below main() for more info

    array.size(4,4,4);     // create a 4x4x4 cube array
    array.zero(0,-1,0);    // shift the array's Y indexes by -1

//    for (Int x = 0; x < 64; ++x)
//        array.memory_[x] = x + 1;

    Int x, y, z;
    for (z = 0; z < 4; ++z)
        for (y = -1; y < 3; ++y)
            for (x = 0; x < 4; ++x)
                array[x][y][z] = (x+0) + ((y+1)*4) + ((z+0)*4*4) + 1;

    array[1][1][3] = 99;     // in the output the above .zero() moves this 99

    Array array2 = array;    // this single declaration copies the first array
                             // new array has all the properties of the old

    for (z = 0; z < 4; ++z)  // print out the new array
    {
        for (y = -1; y < 3; ++y)
        {
            for (x = 0; x < 4; ++x)
            {
                cout.width(3);
                cout << array2[x][y][z];
            }
            cout << endl;
        }
        cout << endl;
    }

    cout << "-------------" << endl << endl;

    array2.zero(0,0,-3);    // put the fourth Z plane at the origin
                            // so during this next downsize it will survive

    array2.size(4,4);       // convert to a 4x4 square array

    for (y = 0; y < 4; ++y)    // print out the new array
    {
        for (x = 0; x < 4; ++x)
        {
            cout.width(3);
            cout << array2[x][y];
        }
        cout << endl;
    }
    cout << endl;

    return 0;
}
catch(Array::wrongdimensions)
{
    cout << "Array access using wrong dimensions!\n";
}
catch(Array::outofrange)
{
    cout << "Array access out of bounds!\n";
}
catch(Int::exception)
{
    cout << "unknown Int exception!\n";
}
catch(...)
{
    cout << "unknown exception!\n";
}


// Why isn't there an "array class" syntax built into C++?  For example:

//  class Int[]
//  {
//  public:
//      Int[](int size);              // perhaps called for each dimension?
//      Int[](int dimensions, int *sizes);           // or maybe only once?
//      Int[](int xsize);                            // or maybe like this?
//      Int[](int xsize, int ysize);                           // and this?
//      Int[](int xsize, int ysize, int zsize);                // and this?
//      Int &operator[](Int &index);
//      Int &operator[](int index);
//  private:
//      Int *array_;    // the actual data
//  };

// C and C++ arrays have long been regarded as "broken".  This would fix them.

// Of course class Int would have to be previously defined and if Int[] did not
// define a member operator[] then the array members could never be accessed
// because the compiler couldn't be expected to find them on it's own.

// Essentially class Array provides the functionality I am wanting but not as
// cleanly as a class Int[] would.  "Array integers[5];" is a C++ array of
// five Arrays, probably not what the user intended.  "Int integers[5]" is
// an array of five Ints (correct) but without bounds checking, without
// array resizing, etc., because C++ has sole control over allocating and
// indexing into this array.

// I can't think of any reason why class Int[] should not be allowed... it
// doesn't even seem to be difficult to implement compilerwise, although I am
// no compiler-internals expert.  The main change is the declaration syntax.
// Except for that, the class does all of the work for the compiler, the
// compiler just needs to set up that array constuctor call correctly.  But if
// anybody can think of a reason why this really would not work please send it
// to:
//     mailto:info@nailstorm.com     with subject "array class syntax"

// By the way, it would also be natural to have a "pointer class" syntax:

// class Int* 
// {
// public:
//     Int*(Int[]);
//     Int &operator*() { return array_[index_]; }
// private:
//     Int[] *array_;
//     Int index_;
// };

// This sample pointer class provides an alternate pointer implementation which
// would not become invalid even if the array memory was moved around.  These
// smart pointers would be constructed silently by the compiler every time the
// address of an Int was taken so to the user everything would "just work".

// It would probably also be natural to allow "reference class" syntax.

