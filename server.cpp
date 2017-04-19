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
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "SpreadsheetManager.h"
#include "Session.h"

// Using the boost asio tcp ip namespaces?
using boost::asio::ip::tcp;

class server
{
  public:
    server(boost::asio::io_service& io_service, short port)
      : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      socket_(io_service)
    {
      do_accept();
      _spreadsheetManager = new SpreadsheetManager();
    }


  private:
    SpreadsheetManager* _spreadsheetManager;

    void do_accept()
    {
      acceptor_.async_accept(socket_,
                             [this](boost::system::error_code ec)
    {
      if (!ec)
      {
        std::make_shared<Session>(std::move(socket_), _spreadsheetManager)->Start();
      }

      do_accept();
    });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

// Hard code the port into the server for this assignment
const int ourPort = 2112;
/*
 * Main - Here is where the service is 'created' and started up
 */
int main(int argc, char* argv[])
{

  // Declaration of the io_service
  boost::asio::io_service io_service;

  // Create the server with the io_service variable and the port number
  server s(io_service, ourPort);

  // Start up the listening service
  io_service.run();

  return 0;
}
