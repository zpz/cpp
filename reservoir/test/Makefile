CC = g++-4.7
#CCFLAGS := -std=c++11 -Wfatal-errors -ggdb3 -Wall
CCFLAGS = -std=c++11 -Wfatal-errors -ggdb3 -Wall -O2
LLFLAGS =
INCLUDES = -I../ -I./ -I$(HOME)/usr/include
LIBS = -L$(HOME)/usr/lib -lrandom -lhdf5util

all: test_reservoir

test_reservoir: test_reservoir.o
	$(CC) $(LLFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp ../random.h ../librandom.so
	$(CC) -c $(CCFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f *.o
	rm -f test_reservoir
	rm -f reservoir.h5 reservoir_rewrite.h5
