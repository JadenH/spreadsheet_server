// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/*

COMPILE WITH THE FOLLOWING:

g++ -std=c++11 syncServer.cpp -lpthread /usr/local/lib/libboost_system.a

*/

#include <cstdlib>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

//FORWARD DECLARATIONS
void start_communication( tcp::socket socket);


int main()
{
	try
	{
		//Provides core I/O functionality for socket objects
		boost::asio::io_service io_service;

		//Object for accepting a set of socket objects
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 2112));
	
		//Main listening loop
		while(true)
		{
			//Creates a socket
			tcp::socket socket(io_service);
			
			//Establishes a connection, binds it to socket
			acceptor.accept(socket);
			
			//Creates a thread, detatches it from the main thread
			//The new thread performs 'start_communication' with the new socket
			std::thread(start_communication, std::move(socket)).detach();
		}
		
	
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

/*
Each accepted socket runs this code
*/
void start_communication(tcp::socket socket)
{
	//Main communication loop
	try
	{
		while(true)
		{
		
		//Buffer size is 1024 bytes
		char data[1024];

		boost::system::error_code error;
		
		/*
		Right now, breaks if we recieve '\0' as a message.
		
		not sure where/when buffer is cleared.
		*/
		size_t length = socket.read_some(boost::asio::buffer(data), error);
		
		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
		
		else if (error)
			throw boost::system::system_error(error); // Some other error.
		
		std::cout << data;
	
		//boost::asio::write(sock, boost::asio::buffer(data, length));
		}
		
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}
