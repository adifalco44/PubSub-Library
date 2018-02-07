all:    test

test: testcallback.cpp callback.o
	        g++ testcallback.cpp callback.cpp -o test

callback.o: callback.cpp
	        g++ -c callback.cpp

clean:
	        rm -f *.o test;
