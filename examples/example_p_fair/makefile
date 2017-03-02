-include ../../makefile.marco

CFLAGS = -g -O2 -std=c++11 -Wall -I. -I$(SYSRT)/src -I$(SYSTEMC_DIR)/include
LFLAGS = -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc -L$(SYSRT)/lib -lsysrt \
	 -Wl,-rpath,$(SYSTEMC_DIR)/lib-linux64 

all: main

test: main
	-$(TIME) ./main

main: test2.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

test2.o:  
	$(CC) $(CFLAGS) -c test2.cpp

clean:
	-$(RM) *.o main *~ dump*
