# The following variable should be set by the caller.
libname ?=
testlib ?=

# 'libarc' is the lib archive.
libarc = lib$(libname).a
libinc = $(libname).h

testsrc = $(wildcard test.c test.cc test.C test.CC)
testobj = $(addsuffix .o,$(basename $(testsrc)))
testprog = $(basename $(testsrc))

ifeq ($(suffix $(testsrc)),.c)
  testlinker=$(CC)
else ifeq ($(suffix $(testsrc)),.C)
  testlinker=$(CC)
else ifeq ($(suffix $(testsrc)),.cc)
  testlinker=$(CCC)
else ifeq ($(suffix $(testsrc)),.CC)
  testlinker=$(CCC)
else
  testlinker=
endif

srcs = $(filter-out test.%,$(wildcard *.c *.cc *.C *.CC))
objs = $(addsuffix .o,$(basename $(srcs)))

build: $(objs)
	$(AR) $(ARFLAGS) $(libarc) $(objs)
	$(RANLIB) $(libarc)


install: build
	$(INSTALL) $(libarc) $(LIBDIR)
	$(INSTALL) $(libinc) $(INCLUDEDIR)


check: build $(testobj)
  ifdef testlinker
	$(testlinker) $(testobj) $(libarc) $(testlib) -o $(testprog)
	@echo
	@echo "running test program---"
	./$(testprog)
  else
	@echo
	@echo "No test program was found."
  endif


clean:
	rm -f $(objs)
	rm -f $(libarc)
	rm -f $(testobj) $(testprog)


uninstall:
	-$(UNINSTALL) $(LIBDIR)/$(libarc)
	-$(UNINSTALL) $(INCLUDEDIR)/$(libinc)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	$(CCC) $(CCFLAGS) -c $< -o $@


