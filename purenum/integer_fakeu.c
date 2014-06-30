// ------------------
// Purenum 0.4e alpha
// ------------------
// integer_fakeu.c
// ------------------


#include "integer_fakeu.h"


char *
Integer_fakeu::ascii(const Integer_fakeu base) const
{
    if (base.value_ < 2) throw outofrange();

    Integer_fakeu::atom i2, digits = 1;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        digits++;

    char *buffer;
    buffer = new char[((Integer_fakeu::atom)digits) + 1];
    buffer[digits] = 0;    // terminator for ASCIIZ string

    Integer_fakeu::atom place = digits;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        buffer[--place] = (i2 % base.value_) + ((int)'0');
    buffer[--place] = (i2) + ((int)'0');
    return buffer;
}

void
Integer_fakeu::ascii(atom size, char *buffer, const Integer_fakeu base)
                                                                          const
{
    if (base.value_ < 2) throw outofrange();
    if (size < 1) throw outofrange();

    Integer_fakeu::atom i2, digits = 1;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        digits++;
    if (digits >= size) throw badstring();
    buffer[digits] = 0;

    Integer_fakeu::atom place = digits;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        buffer[--place] = ((Integer_fakeu::atom)(i2 % base.value_)) + '0';
    buffer[--place] = ((Integer_fakeu::atom)i2) + '0';
}

void
Integer_fakeu::ascii(const char *, const Integer_fakeu base)
{
    if (base.value_ < 2) throw outofrange();
}


//#if 0
void
touch(const Integer_fakeu::atom &)
{
}

void
touch(const Integer_fakeu &)
{
}
//#endif    // 0

