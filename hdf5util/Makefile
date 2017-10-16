# Do not add machine specific lib paths.
#
# Instead of including required paths in the makefile,
# add them to the environ variable
#
#   DYLD_FALLBACK_LIBRARY_PATH
#
# on a Mac and
#
#   LD_LIBRARY_PATH
#
# on Linux.


CC = g++-4.7
#CCFLAGS = -std=c++11 -Wfatal-errors -ggdb3 -fPIC -Wall
CCFLAGS = -std=c++11 -Wfatal-errors -ggdb3 -fPIC -Wall -O2
LLFLAGS = -m64 -shared
LIBS = -lhdf5_hl -lhdf5
INCLUDES =
INSTALLDIR = $(HOME)/usr

all: libhdf5util.so

libhdf5util.so: hdf5util.o
	$(CC) $(LLFLAGS) $^ $(LIBS) -o $@
	install $@ $(INSTALLDIR)/lib/
	cp -f hdf5util.h $(INSTALLDIR)/include/

hdf5util.o: hdf5util.cpp hdf5util.h
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
	rm -f libhdf5util.so
	rm -f $(INSTALLDIR)/lib/libhdf5util.so
	rm -f $(INSTALLDIR)/include/hdf5util.h
