// ------------------
// Purenum 0.4e alpha
// ------------------
// array.h
// ------------------

// An easy-to-use yet very powerful array class.

// Multidimensional, variable origin, and resizable.  Multidimensional indexes
// are accessed thru the conventional C++ [] syntax.  The origin may be moved
// to any arbitrary bignum position.  Moving the origin is fast... it does not
// touch the array data itself.  After resizing an array, as much data as
// possible is preserved.  Any overlap between the new and old array bounds
// will still contain the original data after the resize.  This even applies
// when the number of dimensions is changed.

// Unfortunantly, the C++ array declaration syntax is broken and unusable for
// this class.  For example to store five Ints in an array: "Array a[5]" would
// be five Arrays (wrong!) and "Int a[5]" would be five Ints (correct) but with
// none of the nice features listed above (wrong!).

// So for now an Array must be first defined and then seperately dimensioned.
// Only these two Array definition formats are legal:
//     Array array1;             // empty array (default constructor)
//         -or-
//     Array array1 = array2;    // duplicated array (copy constructor)

// Any time after the definition it is legal to dimension the array.  This can
// be done as many times as desired:
//     array1.size(4,4);    // now array1 is 16 Ints arranged in a square grid
//     array1.size(16);     // now array1 is 16 Ints arranged in a line
//     array1.size(8,2,1);  // now array1 is 16 Ints arranged in a rectangle

// Resizing an array requires it to also be rezeroed.


#ifndef ARRAY_H
#define ARRAY_H

#include "int.h"

class Array
{
public:
    class Index;
//    class Size;
    inline Array()
        : numberofdimensions_(0), dimension_(0),
          zero_(0), memory_(new Int[1]) {  }
    inline Array(const Array &right)
        : numberofdimensions_(right.numberofdimensions_)
    {
        unsigned int i;
        if (numberofdimensions_ != 0)
            dimension_ = new unsigned int[numberofdimensions_];
        else
            dimension_ = 0;
        for (i = 0; i < numberofdimensions_; ++i)
            dimension_[i] = right.dimension_[i];
        if (numberofdimensions_ != 0)
            zero_ = new Int[numberofdimensions_];
        else
            zero_ = 0;
        for (i = 0; i < numberofdimensions_; ++i)
            zero_[i] = right.zero_[i];
        Int size = 1;
        for (i = 0; i < numberofdimensions_; ++i)
            size *= dimension_[i];
        unsigned int memorysize = size;
        memory_ = new Int[size];
        for (i = 0; i < memorysize; ++i)
            memory_[i] = right.memory_[i];
    }
    inline ~Array()
    {
        if (dimension_ != 0) delete [] dimension_; 
        if (zero_ != 0) delete [] zero_; 
        delete [] memory_;
    }
    inline Array &operator=(const Array &);
//    inline Array &operator=(const Size &);
    inline Index operator[](const Int &);
    inline Index operator[](const Int::atom &);
    inline Index operator[](const Int::satom &);
    class exception { };
    class wrongdimensions : public exception { };    // impossible indexing
    class outofrange : public exception { };
    inline void dimensions(Int &dimensions);
    inline const Int dimensions();
//    static Size size;
    void size(Int x)
    {
        Int::atom oldnumberofdimensions = numberofdimensions_;
        Int::atom *olddimension = dimension_;
        Int *oldzero = zero_;
        Int *oldmemory = memory_;

        numberofdimensions_ = 1;
        dimension_ = new unsigned int[numberofdimensions_];
        dimension_[0] = x;
        zero_ = new Int[numberofdimensions_];
        zero_[0] = 0;
        Int size = Int(x);
        memory_ = new Int[(unsigned int)(size)];

        copy(oldmemory,oldzero,olddimension,oldnumberofdimensions,
            memory_,zero_,dimension_,numberofdimensions_);
        if (olddimension != 0) delete [] olddimension;
        if (oldzero != 0) delete [] oldzero;
        delete [] oldmemory;
    }
    void size(Int x, Int y)
    {
        Int::atom oldnumberofdimensions = numberofdimensions_;
        Int::atom *olddimension = dimension_;
        Int *oldzero = zero_;
        Int *oldmemory = memory_;

        numberofdimensions_ = 2;
        dimension_ = new unsigned int[numberofdimensions_];
        dimension_[0] = x;
        dimension_[1] = y;
        zero_ = new Int[numberofdimensions_];
        zero_[0] = 0;
        zero_[1] = 0;
        Int size = Int(x) * Int(y);
        memory_ = new Int[(unsigned int)(size)];

        copy(oldmemory,oldzero,olddimension,oldnumberofdimensions,
            memory_,zero_,dimension_,numberofdimensions_);
        if (olddimension != 0) delete [] olddimension;
        if (oldzero != 0) delete [] oldzero;
        delete [] oldmemory;
    }
    void size(Int x, Int y, Int z)
    {
        Int::atom oldnumberofdimensions = numberofdimensions_;
        Int::atom *olddimension = dimension_;
        Int *oldzero = zero_;
        Int *oldmemory = memory_;

        numberofdimensions_ = 3;
        dimension_ = new unsigned int[numberofdimensions_];
        dimension_[0] = x;
        dimension_[1] = y;
        dimension_[2] = z;
        zero_ = new Int[numberofdimensions_];
        zero_[0] = 0;
        zero_[1] = 0;
        zero_[2] = 0;
        Int size = Int(x) * Int(y) * Int(z);
        memory_ = new Int[(unsigned int)(size)];

        copy(oldmemory,oldzero,olddimension,oldnumberofdimensions,
            memory_,zero_,dimension_,numberofdimensions_);
        if (olddimension != 0) delete [] olddimension;
        if (oldzero != 0) delete [] oldzero;
        delete [] oldmemory;
    }
    void zero(Int x)
    {
        if (numberofdimensions_ != 1) throw wrongdimensions();
        zero_[0] += x;
    }
    void zero(Int x, Int y)
    {
        if (numberofdimensions_ != 2) throw wrongdimensions();
        zero_[0] += x;
        zero_[1] += y;
    }
    void zero(Int x, Int y, Int z)
    {
        if (numberofdimensions_ != 3) throw wrongdimensions();
        zero_[0] += x;
        zero_[1] += y;
        zero_[2] += z;
    }
private:
public:    // FIXME: delete this line now!
    Int::atom numberofdimensions_;
    Int::atom *dimension_;
    Int *zero_;
    Int *memory_;

    static void copy(
        Int *oldmemory, Int *oldzero,
        Int::atom *olddimension, Int::atom oldnumberofdimensions,
        Int *newmemory, Int *newzero,
        Int::atom *newdimension, Int::atom newnumberofdimensions
        );

    friend class Index;
};

class Array::Index    // index for dimension into a true multidimensional Array
{                     // good compilers will optimize away this entire class!
public:
    inline Index(Array &array, const Int &value)
        : array_(array),
          whichdimension_(0),
          span_(1)
    {
        if (array_.numberofdimensions_ == whichdimension_)
            throw Array::wrongdimensions();
        if (value < array_.zero_[whichdimension_]
                || value >= array_.zero_[whichdimension_]
                   + array_.dimension_[whichdimension_])
            throw Array::outofrange();
        flatindex_ = value - array_.zero_[whichdimension_];
    }
    inline Index(Index &nextindex, const Int &value)
        : array_(nextindex.array_)
    {
        whichdimension_ = nextindex.whichdimension_ + 1;
        if (array_.numberofdimensions_ == whichdimension_)
            throw Array::wrongdimensions();
        span_ = nextindex.span_
                * array_.dimension_[nextindex.whichdimension_];
        if (value < array_.zero_[whichdimension_]
                || value >= array_.zero_[whichdimension_]
                   + array_.dimension_[whichdimension_])
            throw Array::outofrange();
        flatindex_ = nextindex.flatindex_ 
                       + ((value - array_.zero_[whichdimension_]) * span_);
    }
    inline operator Int&()
    {
        if (whichdimension_ != array_.numberofdimensions_ - 1)
            throw wrongdimensions();
        return *(array_.memory_ + flatindex_);
    }
    // FIXME: I think all public Int member operators need be duplicated here
    inline Index &operator=(const Int &right)
        { (Int &)*this = right; return *this; }
    inline Index &operator+=(const Int &right)
        { (Int &)*this += right; return *this; }
    inline Index &operator-=(const Int &right)
        { (Int &)*this -= right; return *this; }
    inline Index &operator*=(const Int &right)
        { (Int &)*this *= right; return *this; }
    inline Index &operator/=(const Int &right)
        { (Int &)*this /= right; return *this; }
    inline Index &operator%=(const Int &right)
        { (Int &)*this %= right; return *this; }
    inline Index &operator++()       // prefix
        { ((Int &)*this)++; return *this; }
    inline Index &operator++(int)    // postfix
        { ((Int &)*this)++; return *this; }
    inline Index &operator--()       // prefix
        { ((Int &)*this)--; return *this; }
    inline Index &operator--(int)    // postfix
        { ((Int &)*this)--; return *this; }
    inline Index operator[](const Int &);
    inline Index operator[](const Int::atom &);
    inline Index operator[](const Int::satom &);
private:
    Array &array_;                        // array that this index is for
    Int::atom whichdimension_;            // which array dimension is this
    Int::atom span_;                      // offset into array for dimension
    Int::atom flatindex_;                 // offset into array memory

    Index();
    friend class Array;
};

//class Array::Size
//{
//public:
//    inline Size operator[](const Int &);
//    inline Size operator[](const Int::atom &);
//    inline Size operator[](const Int::satom &);
//private:
//    Size() {}
//    Size(const Size &);
//    Size &operator=(const Size &);
//    friend class Array;
//};

inline Array::Index
Array::operator[](const Int &index)
{
    return Index(*this,index);
}

inline Array::Index
Array::operator[](const Int::atom &index)
{
    return operator[](Int(index));
}

inline Array::Index
Array::operator[](const Int::satom &index)
{
    return operator[](Int(index));
}

//inline Array &
//Array::operator=(const Array::Size &)
//{
//    // FIXME: resize the array here
//    return *this;
//}

inline Array::Index
Array::Index::operator[](const Int &index)
{
    return Index(*this,index);
}

inline Array::Index
Array::Index::operator[](const Int::atom &index)
{
    return operator[](Int(index));
}

inline Array::Index
Array::Index::operator[](const Int::satom &index)
{
    return operator[](Int(index));
}

//inline Array::Size
//Array::Size::operator[](const Int &index)
//{
//    return *this;    // Size(*this,index);
//}

//inline Array::Size
//Array::Size::operator[](const Int::atom &index)
//{
//    return operator[](Int(index));
//}

//inline Array::Size
//Array::Size::operator[](const Int::satom &index)
//{
//    return operator[](Int(index));
//}

//inline
//Array::Size::Size(const Size &right)
//{
//}

#endif    // ARRAY_H

