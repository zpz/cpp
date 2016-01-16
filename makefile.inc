#FC = g95
FC = gfortran
FFLAGS = \
    -fimplicit-none -Wall -O2

# -fmodule-private -std=f2003 -Wall -fimplicit-none \
# -fzero -fshort-circuit \
# -O2
# G95_INCLUDE_PATH should have been set properly in the environment

CC = gcc
CFLAGS = \
  -std=c89 \
  -Wall \
  -O2

CCC = g++
CCFLAGS = \
  -Wall \
  -O2

RANLIB = ranlib

INSTALL = install -p
UNINSTALL = rm -f

INSTALL_ROOT = $(HOME)/usr/local
LIBDIR = $(INSTALL_ROOT)/lib
INCLUDEDIR = $(INSTALL_ROOT)/include

