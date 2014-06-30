# Well, this doesn't work unless you used g++ to compile
# the library, but whatever.
CXX       = g++
FLAGS     = -D_FILE_OFFSET_BITS=64 -O2 -ansi -W -Wall -I../../include
STRIP	  = strip

PROGS     = base64 bzip hasher hex md5 md5_fd

all: $(PROGS)

base64: base64.cpp
	$(CXX) $(FLAGS) $? -L../.. -lopencl -o $@
	$(STRIP) $@

bzip: bzip.cpp
	$(CXX) $(FLAGS) $? -L../.. -lopencl -o $@
	$(STRIP) $@

hasher: hasher.cpp
	$(CXX) $(FLAGS) $? -L../.. -lopencl -o $@
	$(STRIP) $@

hex: hex.cpp
	$(CXX) $(FLAGS) $? -L../.. -lopencl -o $@
	$(STRIP) $@

md5: md5.cpp
	$(CXX) $(FLAGS) $? -L../.. -lopencl -o $@
	$(STRIP) $@

md5_fd: md5_fd.cpp
	$(CXX) $(FLAGS) $? -L../.. -lopencl -o $@
	$(STRIP) $@

clean:
	rm -f $(PROGS)

