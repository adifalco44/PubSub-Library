CXX=       	g++
CXXFLAGS= 	-g -gdwarf-2 -std=gnu++11 -Wall
LD=		g++
LDFLAGS=	-pthread
PROGRAMS=       $(C_SOURCE:.cpp=)
TARGETS=	client_lib \
		client_lib.o \
		callback.o

all:    	$(TARGETS)

client_lib:	client_lib.o callback.o
		@$(LD) $(LDFLAGS) -o $@ client_lib.o callback.o

callback.o:	callback.cpp message.h
		$(CXX) $(CXXFLAGS) -c -o callback.o callback.cpp

client_lib.o:	client_lib.cpp queue.h thread.h callback.h
		$(CXX) $(CXXFLAGS) -c -o client_lib.o client_lib.cpp

clean:
		rm -f $(TARGETS)

.PHONY: all clean
