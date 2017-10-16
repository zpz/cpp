CC = g++-4.7
#CCFLAGS := -std=c++11 -Wfatal-errors -ggdb3 -Wall
CCFLAGS = -std=c++11 -Wfatal-errors -ggdb3 -Wall -O2
LLFLAGS =
INCLUDES = -I../
LIBS = -L../ -lhdf5util -lhdf5_hl -lhdf5

all: test_make_read_number

test_make_read_number: test_make_read_number.o
	$(CC) $(LLFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp ../hdf5util.h ../libhdf5util.so
	$(CC) -c $(CCFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f *.o
	rm -f test_make_read_number
	rm -f *h5
