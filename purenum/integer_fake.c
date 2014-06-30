// ------------------
// Purenum 0.4e alpha
// ------------------
// integer_fake.c
// ------------------


#include "integer_fake.h"


char *
Integer_fake::ascii(const Integer_fake base) const
{
    if (base.value_ < 2) throw outofrange();

    Integer_fake::satom i2, digits = 1;
    if (value_ < 0) ++digits;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        digits++;

    char *buffer;
    buffer = new char[((Integer_fake::satom)digits) + 1];
    buffer[digits] = 0;    // terminator for ASCIIZ string

    Integer_fake::satom place = digits;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        buffer[--place] = (i2 % base.value_) + ((int)'0');
    buffer[--place] = (i2) + ((int)'0');
    if (value_ < 0) buffer[--place] = ((int)'-');
    return buffer;
}

void
Integer_fake::ascii(atom size, char *buffer, const Integer_fake base)
                                                                          const
{
    if (base.value_ < 2) throw outofrange();
    if (size < 1) throw outofrange();

    Integer_fake::satom i2, digits = 1;
    if (value_ < 0) ++digits;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        digits++;
    if (size < 0) throw outofrange();
    if (digits >= (satom)size) throw badstring();
    buffer[digits] = 0;

    Integer_fake::satom place = digits;
    for (i2 = value_; i2 >= base.value_; i2 /= base.value_)
        buffer[--place] = ((Integer_fake::satom)(i2 % base.value_)) + '0';
    buffer[--place] = ((Integer_fake::satom)i2) + '0';
    if (value_ < 0) buffer[--place] = '-';
}

void
Integer_fake::ascii(const char *, const Integer_fake base)
{
    if (base.value_ < 2) throw outofrange();
}

//#if 0
void
touch(const Integer_fake::satom &)
{
}

void
touch(const Integer_fake &)
{
}
//#endif    // 0

