all:
	g++ -std=c++11 server.cpp /usr/local/lib/libboost_system.a
clean:
	rm -f *.out
	rm -f server
	rm -f *.o
