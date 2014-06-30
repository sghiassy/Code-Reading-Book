////////////////////////////////////////////////////////////////////////////////////
// Below are some macro's defined by Alberto García-Baquero Vega, and posted as
// Tip Of The Day at flipcode (www.flipcode.com). These macro's help you insert TODO
// FIXME's, NOTEs and QUOTEs in your code that will also appear in the compiler output.
// This way you won't forget the TODO's and FIXME's. 
//
// It's recommended that you use these instead of normal comments to insert TODO's and FIXME's in
// the code.
//
// (c) Alberto García-Baquero Vega
////////////////////////////////////////////////////////////////////////////////////
//
// example: (test.cpp)
// int main(int argc, char* argv[])
// {
//	 #pragma TODO(  We have still to do some work here... )
//	 #pragma FIXME( Limits are not controlled in that function or things like that )
//
//	 #pragma todo(  Have a look to flipcode daily ! ) 
//	 #pragma todo(  Sleep... ) 
//
//	 #pragma fixme( It seems that there is some leaks in that object )
//
//	 #pragma FILE_LINE    
//	 #pragma NOTE( " \n\
//	      A free format multiline, comment............\n\
//	                  So I can put a whole text here              \n\
//		    -------------------------------------------------")
//	 return 0;
// }
//
// Output for this example:
//
//	Test.cpp
//	c:\_code\kk\test.cpp(25) : 
//	------------------------------------------------
//	|  TODO :   We have still to do some work here...
//	 -------------------------------------------------
//	c:\_code\kk\test.cpp(26) : 
//	------------------------------------------------
//	|  FIXME :  Limits are not controlled in that function or things like that
//	 -------------------------------------------------
//	c:\_code\kk\test.cpp(28) :  TODO :   Have a look to flipcode daily !
//	c:\_code\kk\test.cpp(29) :  TODO :   Sleep...
//	c:\_code\kk\test.cpp(31) :  FIXME:   It seems that there is some leaks in that object
//	c:\_code\kk\test.cpp(33) : 
//	
//		A free format multiline, comment............
//	                   So I can put a whole text here              
//	      -------------------------------------------------          
//	
//	Test.obj - 0 error(s), 0 warning(s)
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _SRC_BEAUTIFIERS_H
#define _SRC_BEAUTIFIERS_H

//---------------------------------------------------------------------------------------------
// FIXMEs / TODOs / NOTE macros
//---------------------------------------------------------------------------------------------
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )

#define TODO( x )  message( __FILE__LINE__"\n"           \
        " ------------------------------------------------\n" \
        "|  TODO :   " #x "\n" \
        " -------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
        " ------------------------------------------------\n" \
        "|  FIXME :  " #x "\n" \
        " -------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" ) 


#endif // _SRC_BEAUTIFIERS_H