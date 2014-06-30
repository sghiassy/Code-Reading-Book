# ------------------
# Purenum 0.4e alpha
# ------------------
# makefile
# ------------------


# makefile for Purenum, by Shawn Yarbrough
# designed for use with the GNU C++ compiler (g++) and GNU make
# thanks to the Free Software Foundation for making GNU (www.gnu.org)
# Purenum is developed on Debian GNU/Linux


# CONFIGURATION CONFIGURATION CONFIGURATION

CC=g++

# define here which kind of Integer is referred to by the Int type
# uncomment exactly one "integer=" line here
# also uncomment a relevant "integerlink=" line if appropriate

integer=integer
#integer=integer_2c
#integer=integer_gmp
#integer=integer_fake
#integer=integer_fakeu
#integer=integer_x86

#integerlink=-lgmp

# define here the name of the Purenum directory (this is only used by make tar)

directory=purenum0.4e


# print out some usage info if make is run with no options
default:
	@echo
	@echo "Usage: make <option>"
	@echo "       make final ------ compile for product release (optimizations on)"
	@echo "       make debug ------ compile for development (debugging on)"
	@echo "       make what  ------ report all the things this makefile can build"
	@echo "       make tar   ------ make a tar of this entire directory"
	@echo "       make gprof ------ compile final plus gprof information"
	@echo "       make clean ------ clean up temporary files"
	@echo


# list of all of the final products of this makefile
# the directory name of the current directory (no slashes)

products=testinteger testprimes testarray


# all make targets that are using the Int type...
#   ...must compile depending on int.h
#   ...must compile depending on $(integer).h
#   ...must compile using command line option $(DEFINT)
#   ...must link depending on $(integer).o
#   ...must link with $(integer).o
#   ...must link using command line option $(integerlink)

DEFINT=-DPURENUM_INT_DEFAULT_HEADER=\"$(integer).h\"


# compiler options for the GNU C++ compiler, gcc
OPTIONS=-ansi -pedantic -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Winline -Wall -Werror -Wwrite-strings -fverbose-asm $(DEFINT)

integer.o: makefile integer.c integer.h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c integer.c -o $@

integer_fake.o: makefile integer_fake.c integer_fake.h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c integer_fake.c -o $@

integer_fakeu.o: makefile integer_fakeu.c integer_fakeu.h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c integer_fakeu.c -o $@

integer_gmp.o: makefile integer_gmp.c integer_gmp.h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c integer_gmp.c -o $@

array.o: makefile array.c array.h int.h $(integer).h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c array.c -o $@

testinteger.o: makefile testinteger.c int.h $(integer).h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c testinteger.c -o $@

testprimes.o: makefile testprimes.c int.h $(integer).h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c testprimes.c -o $@

testarray.o: makefile testarray.c array.h int.h $(integer).h
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) -c testarray.c -o $@

testinteger: makefile $(integer).o testinteger.o
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) testinteger.o $(integer).o -o $@ $(integerlink)

testprimes: makefile $(integer).o testprimes.o
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) testprimes.o $(integer).o -o $@ $(integerlink)

testarray: makefile $(integer).o array.o testarray.o
	@echo "$@"
	@$(CC) $$MAKEMODE $(OPTIONS) testarray.o array.o $(integer).o -o $@ $(integerlink)


# various high-level make targets

.PHONY: final final2 debug debug2 gprof gprof2 strip clean tidy tar what

what:
	@echo
	@echo $(products)
	@echo

tar:
	@echo "[ packing up the directory in it's current state ]"
	@rm -f $(directory).tar ; touch .* * ; cd .. ; tar --numeric-owner -cvsf $(directory).tar $(directory)/ ; mv $(directory).tar $(directory)/

tidy:
	@rm -f *.o *.ii *.s core nohup.out
	@echo "[ temporary files removed ]"

clean:
	@rm -f *.o *.ii *.s core nohup.out
	@echo "[ temporary files removed ]"
	@rm -f $(products) $(directory).tar
	@echo "[ final product files removed ]"

strip:
	@strip $(products)
	@echo "[ final product files stripped ]"

# called by 'make debug', do not call directly
debug2: $(products)

# compile with debugger support turned on and no optimizations
debug:
	@echo
	@export MAKEMODE=-g3 ; $(MAKE) --no-print-directory debug2
	@echo

# called by 'make final', do not call directly
final2: clean $(products) strip tidy

# compile with maximum optimization and clean up everything for final release
final:
	@echo
	@export MAKEMODE=-O3 ; $(MAKE) --no-print-directory final2
	@echo

# called by 'make gprof', do not call directly
gprof2: clean $(products) tidy

# compile similar to make final but with added gprof profiling information
gprof:
	@echo
	@export MAKEMODE="-pg" ; $(MAKE) --no-print-directory gprof2
	@echo

