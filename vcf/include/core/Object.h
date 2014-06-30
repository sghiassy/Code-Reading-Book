/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/



#ifndef OBJECT_H
#define OBJECT_H


#include <list>


namespace VCF{


class Class;


#define OBJECT_CLASSID		"ED88C099-26AB-11d4-B539-00C04F0196DA"

/**
*Base class for entire Visual Component Framework 
*New as of July 5,2001, Object will also support 
*refcounting for those who need this kind of support.
*There are several places in the ApplicationKit that will need to make use
*of this fairly soon. 
*Also Object supports a new technique for instantiating and destroying Object
*derived instances. In general, in C++, when creating a new 
*instance on the heap (free-store) you would use the operator new() function. This is
*being phased in the VCF for a variety of reasons, in favor of a new (no pun intended)
*scheme. New classes should hide their constructor's and destructor's, making 
*them private or protected. Proper object instantiation will now look like this:
<pre>
FooObject* f = FooObject::create();

//or if we needed to have a constructor that took an int...

FooObject* f = FooObject::create(12);
</pre>

*create() is a static method that will need to put on every class, and will be paired with 
*each constructor of the class. So if the class only needs a default constructor, then create() is 
*OK, if the class has a default constructor, a copy constructor, and a constructor that takes 
*a String, then there need to be 3 create() methods, create(), create( const class& ), and 
*create( String ), and so forth.
*
*The pattern for implementing create() is as follows:
*create the object on the heap via the operator new() call
*call the new created object's init() method, which is virtual 
*return the object to the caller.
*
*The init() method is intended to place common shared startup code. Because of the desire to 
*make it virtual it cannot be called in the constructor, thus the use of the create() call.
*init() does not have to be implemented.
*
*To facilitate this there will be macros that do just this declared in ClassInfo.h.
*These macros will allow for create() methods with up to six(6) arguments passed in
*The macros will NOT make the constructors for you, this is something that I think needs to 
*done by hand, and should be visible so that you can see what is going on. Using the Macros
*is optional. 
*
*Destroying the memory now happens in two ways: The first is the equivalent of the operator delete()
*call. This involves calling the objects free() method, which will call the virtual destroy() method 
*before calling operator delete() on itself;
*The second way an object can be destroyed is if it's refcount drops to 0, at which point the object's
*free() method will be invoked. Calling the object's release() method decrements the reference count 
*on the object.
*
*Like init(), destroy() is a virtual method, where common shared cleanup may take place, again
*due to the restrictions of C++, virtual methods are not allowed in destructors, but we get 
*around this with the free()/destroy() pattern.
*
*Semantics of the addRef/release usage are as follows:
*
*Note that this whole scheme is aimed at heap based objects. Many objects that are created on the stack
*do not need this, though if they have code in their init() method, this will have to be called.
*/
class FRAMEWORK_API Object {
public:    
	Object();

	virtual ~Object();

	static Object* create() {
		Object* result = new Object();
		result->init();
		return result;
	}

	virtual void init();

	/**
	*call this method to free up the memory of the class 
	*for heap based objects
	*/
	void free();

	/**
	*increments the reference count of the object
	*@param Object* the optional owner of the new referenced object
	*for use in future, more sophisticated refcounting schemes
	*@return unsigned long the current reference count of the object
	*/
	virtual unsigned long addRef(Object* owner=NULL);

	/**
	*decrements the reference count of the object
	*@param Object* the optional owner of the new referenced object
	*for use in future, more sophisticated refcounting schemes
	*when the m_refCount drops to 0 the object is destroyed
	*/
	virtual unsigned long release(Object* owner=NULL);

	/**
	*returns the number of outstanding references for this object
	*/
	unsigned long getRefCount() {
		return m_refCount;
	}

    /**
     * returns a string representation of the object 
     */
    virtual String toString();

    /**
     * returns the class name of the object. 
     */
    String getClassName();    
	
	virtual bool isEqual( Object* object ){
		return ( object == this );
	};

	virtual void copy( Object* source ){};

	/**
	*returns the RTTI Class instance associated object 
	*of this type
	*/
	Class* getClass();
	
	/**
	*returns a hash value that represents the object instance
	*/
	virtual unsigned long hash();

	/**
	*Ptr is a smart pointer class for use with refcounted objects.
	*This class should <b><i>NEVER</i></b> be created on the heap,
	*instead create instances on the stack.
	*/
	template <class OBJECT_TYPE> class Ptr {
	public:
		Ptr(){
			m_ptr = NULL;
		}
		
		/**
		*constructor for the Ptr passing in a 
		*new instance of an OBJECT_TYPE
		*/
		Ptr( OBJECT_TYPE* o ){
			m_ptr = o;
		}

		/**
		*allows access to the underlying pointer
		*/
		OBJECT_TYPE* operator ->() {
			return m_ptr;
		}

		/**
		*conversion operator
		*@return OBJECT_TYPE returns the underlying pointer
		*/
		operator OBJECT_TYPE* () {
			return m_ptr;
		}

		/**
		*equality operator.
		*Compares the object passed in with the internal 
		*object.
		**@param OBJECT_TYPE the object to compare with. Current
		*comparison is only a simple pointer comparison. This may
		*become more sophisticated.
		*/
		bool operator ==( OBJECT_TYPE* object ) const	{
			return 	m_ptr == object;
		}

		/**
		*assignment operator.
		*assigns a new object value to the Ptr instance.
		*/
		Ptr& operator= ( OBJECT_TYPE* o ) {			
			m_ptr = o;
			return *this;
		}

		bool operator !()	{
			return (NULL == m_ptr ) ? true: false;
		}

		OBJECT_TYPE& operator *() {
			return *m_ptr;	
		}

		OBJECT_TYPE** operator &( ) {
			return &m_ptr;
		}

		virtual ~Ptr(){
			if ( NULL != m_ptr ){
				m_ptr->release();
			}
		}
	private:
		OBJECT_TYPE* m_ptr;
	};

#ifdef USE_VCF_NEW
	
	void* operator new( size_t allocSize );
	
	void operator delete( void *memPtr );	

	static void dump();

private:
//	static std::list<void*> allocatedObjects; 
#endif

protected:
	/**
	*called by the free() method. Should be overriden 
	*and any clean up code performed here
	*/
	virtual void destroy();

	unsigned long m_refCount;
private:
	
};

};


#endif //OBJECT_H__