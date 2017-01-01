# !/bin/bash

CC  = clang++
CXX = clang++
CXXFLAGS = -std=c++11 -g -O3
LDLIBS = -lstdc++

EXEC = main

all: $(EXEC)

OBJS = auction.o misc.o

$(EXEC): % : %.o $(OBJS)

re:
	make clean && make

.PHONY: clean cleanall
clean :
	@-rm -rf *.o *~* *#* $(EXEC) *.dSYM
	@-DateTime=`date +%Y%m%d%H%M%S`; \
	mkdir $$DateTime; \
	find . -type d -empty -delete

cleanall:
	-@make clean && rm -rf LOG_*
