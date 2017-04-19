#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include "RegexUtils.h"
#include "Sheet.h"
#include "SpreadsheetManager.h"
#include "Session.h"

using boost::asio::ip::tcp;

/*
 * Session class - This represents a connection session between a client and the server
 */

Session::Session(tcp::socket socket, SpreadsheetManager* spreadsheetManager)
  : socket_(std::move(socket))
{
  _spreadsheetManager = spreadsheetManager;
}

void Session::Start()
{
  DoRead();
}

void Session::DoRead()
{
  auto self(shared_from_this());

  socket_.async_read_some(boost::asio::buffer(data_, max_length),
                          [this, self](boost::system::error_code ec, std::size_t length)
  {
    if (!ec)
    {
      std::cout << "Do read has been called!\n" << std::endl;

      std::string theData;
      theData = data_;

      boost::smatch matches;
      if (RegexUtils::RegexFind(theData, "^[^:{]*", matches))
      {
        if (matches[0] == "Connect")
        {
          if (RegexUtils::RegexFind(theData, "([a-zA-Z0-9]*)}", matches))
          {
            DoWrite(matches[1]);
            std::cout << matches[1] << std::endl;
          }
        }
        else
        {
          //  current_spreadsheet->
        }
      }

      std::cout << theData << std::endl;
    }
    else
    {
      std::cout << ec << '\n';
      // TODO: Remove from spreadsheets and disconnect socket.
    }
  });


}

void Session::DoWrite(std::string message)
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, boost::asio::buffer(message + "\n", (message + "\n").length()),
                           [this, self](boost::system::error_code ec, std::size_t /*length*/)
  {
    if (!ec)
    {
      DoRead();
      std::cout << "Do write has been called!\n"  << std::endl;

    }
  });
}
