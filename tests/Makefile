CC = g++
CCFLAGS = -std=c++17 -Wall -Wextra -Wfatal-errors
INCLUDES = -I../include
LIBS = -lavrocpp

# -lstdc++fs provides <experimental/filesystem>
# -flto : link-time optimizations; needs to be passed to both compile and link commands.

# -fPIC



TARGETS = test_avro test_date test_string_view test_typeinfo test_typequery test_unique_ptr 

all: $(TARGETS)

%: %.cc
	$(CC) $(CCFLAGS) $(INCLUDES) $^ $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f *.so
	rm -f $(TARGETS)
