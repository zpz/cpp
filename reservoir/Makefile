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
#CCFLAGS := -std=c++11 -Wfatal-errors -ggdb3 -fPIC -Wall
CCFLAGS = -std=c++11 -Wfatal-errors -ggdb3 -fPIC -Wall -O2
LLFLAGS = -m64 -shared
LIBS = -L$(HOME)/usr/lib -lhdf5util -lhdf5_hl -lhdf5
INCLUDES = -I$(HOME)/usr/include
INSTALLDIR = $(HOME)/usr

librandom.so: random.o
	$(CC) $(LLFLAGS) -o $@ $^ $(LIBS)
	install $@ $(INSTALLDIR)/lib/
	cp -f random.h $(INSTALLDIR)/include/

random.o: random.cpp random.h
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f random.o
	rm -f librandom.so
	rm -f $(INSTALLDIR)/lib/librandom.so
	rm -f $(INSTALLDIR)/include/random.h
