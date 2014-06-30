// ------------------
// Purenum 0.4e alpha
// ------------------
// integer_gmp.c
// ------------------


#include "integer_gmp.h"


char *
Integer_GMP::ascii(const Integer_GMP base) const
{
    if (base < 2) throw outofrange();

    unsigned long strsize = mpz_sizeinbase(value_,base) + 2;
    char *str = new char[strsize];
    mpz_get_str(str,base,value_);
    return str;
}

void
Integer_GMP::ascii(atom size, char *buffer, const Integer_GMP base) const
{
    if (base < 2) throw outofrange();
    if (size < 1) throw outofrange();

    unsigned long strsize = mpz_sizeinbase(value_,base) + 2;
    if (strsize > size) throw badstring();
    mpz_get_str(buffer,base,value_);
}

void
Integer_GMP::ascii(const char *, const Integer_GMP base)
{
    if (base < 2) throw outofrange();
}

//#if 0
void
touch(const Integer_GMP::atom &)
{
}

void
touch(const Integer_GMP &)
{
}
//#endif    // 0

