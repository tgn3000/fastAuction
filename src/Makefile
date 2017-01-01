# !/bin/bash

CC  = g++
CXX = g++
CXXFLAGS = -std=c++11 -g -O3
LDLIBS = -lstdc++

EXEC = main

all: $(EXEC)

OBJS = auction.o misc.o

$(EXEC): % : %.o $(OBJS)

re:
	make clean && make

run:
	@-make --no-print-directory clean && make --no-print-directory
	@-dataDir=../Data; \
	name=`ls $$dataDir`; \
	for i in $$name; do \
		./main $$dataDir/$$i; \
	done

.PHONY: clean
clean :
	@-rm -rf *.o *~* *#* $(EXEC) *.dSYM

