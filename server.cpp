/*
 * Gray Marchese
 * Adam Waggoner
 * Logan Ropelato
 * Jaden Holladay
 * Team: Alpha Squad
 * CS 3505 Spring 2017
 * University of Utah
 *
 *
 * Asychronous TCP server
 * This code was originally based on the 
 * async_tcp_echo_server written and copywrited by 2Christopher M. Kohlhoff (chris at kohlhoff dot com) 2003-2015 
 * made available publicly through GitHub.
 * No infringement or plagiarism is intended through the use of this code, it simply provided our team with a solid 
 * starting point for our project.
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 *file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/regex.hpp>

//Using the boost asio tcp ip namespaces?
using boost::asio::ip::tcp;

/*
 * Session class - This represents a connection session between a client and the server 
 */
class session
	  : public std::enable_shared_from_this<session>
	{
	public:
	  session(tcp::socket socket)
	    : socket_(std::move(socket))
	  {
	  }

	  void start()
	  {
	    do_read();
	  }

	private:
	  void do_read()
	  {
	    auto self(shared_from_this());
	    socket_.async_read_some(boost::asio::buffer(data_, max_length),
		   [this, self](boost::system::error_code ec, std::size_t length)
		   {
		     if (!ec)
		     {
		       do_write(length);
		       std::cout << "Do read has been called!\n" << std::endl;
		                   
		                   std::string theData;
		                   theData = data_;
		                   
		                   std::cout << theData << std::endl;
		                   
		                   
		     }
		   });
	  }

	  void do_write(std::size_t length)
	  {
	    auto self(shared_from_this());
	    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
		   [this, self](boost::system::error_code ec, std::size_t /*length*/)
		   {
		     if (!ec)
		     {
		       do_read();
		       std::cout << "Do write has been called!\n"  << std::endl;

		     }
		   });
	  }

	  tcp::socket socket_;
	  enum { max_length = 1000000000 };
	  char data_[max_length];
};

class server
{
	public:
	  server(boost::asio::io_service& io_service, short port)
	    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		 socket_(io_service)
	  {
	    do_accept();
	  }

	private:
	  void do_accept()
	  {
	    acceptor_.async_accept(socket_,
		   [this](boost::system::error_code ec)
		   {
		     if (!ec)
		     {
		       std::make_shared<session>(std::move(socket_))->start();
		     }

		     do_accept();
		   });
	  }

	  tcp::acceptor acceptor_;
	  tcp::socket socket_;
};

//Hard code the port into the server for this assignment
const int ourPort = 2112;
/*
 * Main - Here is where the service is 'created' and started up
 */
int main(int argc, char* argv[])
{
 
    //Declaration of the io_service 
    boost::asio::io_service io_service;

    //Create the server with the io_service variable and the port number
    server s(io_service, ourPort);

    //Start up the listening service 
    io_service.run();

  return 0;
}
