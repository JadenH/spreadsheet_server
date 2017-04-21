all: 
	g++ -lpthread -std=c++11 *.cpp /usr/local/lib/libboost_system.a /usr/local/lib/libboost_regex.a /usr/local/lib/libboost_thread.a

clean:
	rm -f *.out
	rm -f server
	rm -f *.o

