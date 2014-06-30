// ------------------
// Purenum 0.4e alpha
// ------------------
// array.c
// ------------------


#include "array.h"

//Array::Size Array::size;


// A general array copy.  It works not only for arrays of differing sizes but
// even for arrays of differing dimensions and differing origins.  When the
// origins are different, the data will be preserved anywhere that valid index
// ranges in the new array overlap valid index ranges in the old array.
void
Array::copy(
    Int *oldmemory, Int *oldzero,
     Int::atom *olddimension, Int::atom oldnumberofdimensions,
    Int *newmemory, Int *newzero,
     Int::atom *newdimension, Int::atom newnumberofdimensions
    )
{
    // calculate the flat size of newmemory
    Int::atom newmemorysize = 1, a, b;
    for (a = 0; a < newnumberofdimensions; ++a)
        newmemorysize *= newdimension[a];
    // FIXME: the constructor must allocate one Int for zero dimensional arrays

    // convert each new flat address to a new regular address
    // and map each new regular address to an old regular address
    // and try to convert each old regular address to a old flat address
    // and copy the data
    Int *newaddress = new Int[newnumberofdimensions];
    Int *oldaddress = new Int[oldnumberofdimensions];
    Int::atom divisor, newflat, oldflat;
    for (newflat = 0; newflat < newmemorysize; ++newflat)    // loop over all new flat addresses
    {
        // convert new flat address to new regular address
        divisor = newmemorysize;
        a = newflat;
        for (b = newnumberofdimensions; b > 0; )    // loop over new dimensions
        {
            divisor /= newdimension[--b];
            newaddress[b] = a / divisor;
            newaddress[b] += newzero[b];
            a %= divisor;
        }

        // map new regular address to old regular address
        // if new array has more dimensions map only into it's newzero plane
        for (b = 0; b < oldnumberofdimensions; ++b) // loop over old dimensions
            if (b < newnumberofdimensions)
                oldaddress[b] = newaddress[b];
            else
                oldaddress[b] = 0;
        for (; b < newnumberofdimensions; ++b)  // extra dimensions must be [0]
            if (newaddress[b] != 0)    // do NOT change this 0 to newzero[b]
                goto skipcopy;    // only copy smaller array once into bigger

        // try to convert old regular address to old flat address
        divisor = 1;
        oldflat = 0;
        for (b = 0; b < oldnumberofdimensions; ++b)
        {
            if (oldaddress[b] < oldzero[b]
                    || oldaddress[b] >= oldzero[b] + olddimension[b])
                goto skipcopy;
            oldaddress[b] -= oldzero[b];
            oldflat += (unsigned int)(oldaddress[b] * divisor);
            divisor *= olddimension[b];
        }

        // at long last, the copy 
        newmemory[newflat] = oldmemory[oldflat];
skipcopy:;
    }
    delete [] newaddress;
    delete [] oldaddress;
}

