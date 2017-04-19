all:
	g++ -std=c++11 *.cpp /usr/local/lib/libboost_system.a /usr/local/lib/libboost_regex.a
clean:
	rm -f *.out
	rm -f server
	rm -f *.o
