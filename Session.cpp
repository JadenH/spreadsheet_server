#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include "RegexUtils.h"
#include "Session.h"
#include "SpreadsheetManager.h"

int Session::_currentID = 0;

using boost::asio::ip::tcp;

/*
 * Session class - This represents a connection session between a client and the server
 */

Session::Session(tcp::socket socket)
  : socket_(std::move(socket))
{
  _currentSpreadsheet = NULL;
  _ID = _currentID++;
}

void Session::Start()
{
  DoRead();
}

// Reads a message from the client.
void Session::DoRead()
{
  auto self(shared_from_this());
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
                          [this, self](boost::system::error_code ec, std::size_t BytesRead)
  {
    if (!ec)
    {
			if(BytesRead == 0 || (BytesRead == 1 && data_[0] == '\n'))
			{
				DoRead();
				return;
			}
      data_[BytesRead] = '\0';

      // Split incoming messages on newline character.
      std::vector<std::string> messages = RegexUtils::Split(data_, '\n');

      // Iterate through received messages.
      std::vector<std::string>::iterator itr = messages.begin();
      while(itr != messages.end())
      {
        std::cout << "Message Received: " << *itr << std::endl;

        // Get a list of each field in the message (delimited by tabs)
        std::vector<std::string> msg = RegexUtils::Split(*itr, '\t');

        // If a connect message is received, we have to connect to a specific spreadsheet
        // Format Connect\tSheetName
        // Check for filename size < 255
        if (msg[0] == "Connect" && msg.size() == 2 && msg[1] != "" && RegexUtils::IsValidFilename(msg[1]))
        {
          if(_currentSpreadsheet != NULL)
          {
            _currentSpreadsheet->UnsubscribeSession(_ID);
          }
          _currentSpreadsheet = SpreadsheetManager::GetInstance()->GetSpreadsheet(msg[1]);
          _currentSpreadsheet->SubscribeSession(_ID, this);
        }
        else if(_currentSpreadsheet != NULL)
        {
          // If the message is corrupt of malformed, disconnect that user
          if(!_currentSpreadsheet->ReceiveMessage(_ID, *itr))
          {
            _closeSocket(boost::system::errc::make_error_code(boost::system::errc::permission_denied));
          }
        }
        else
        {
          _closeSocket(boost::system::errc::make_error_code(boost::system::errc::permission_denied));
        }

        ++itr;
      }

      // Continue the read loop
      DoRead();
    }
    else
    {
      // If an error is caught we close the socket
      _closeSocket(ec);
    }
  });
}

// Asynchronously sends a message to the client in this session
void Session::DoWrite(std::string message)
{
  auto self(shared_from_this());

  std::cout << "Message Sent: " << (message + "\n") << std::endl;

  boost::asio::async_write(socket_, boost::asio::buffer(message + "\n", (message + "\n").length()),
                           [this, self](boost::system::error_code ec, std::size_t /*length*/)
  {
    if (ec)
    {
      _closeSocket(ec);
    }
  });
}

void Session::_closeSocket(boost::system::error_code ec)
{
  // Remove from spreadsheets and disconnect socket.
  try
  {
    std::string error;
    std:: string eof1 ("End of file");
    std:: string eof2 ("Connection reset by peer");
    error = ec.message();
    // std::cout<<"Error Message:" << error <<'\n';
    if (ec)
    {
      // Remove from spreadsheets and disconnect socket.
      if(socket_.is_open())
      {
        if (_currentSpreadsheet != NULL)
        {
          _currentSpreadsheet->UnsubscribeSession(_ID);
          _currentSpreadsheet = NULL;
        }
        socket_.shutdown(socket_.shutdown_both);
        socket_.close();
        std::cout << "Safely closed socket after client disconnect within try catch." << std::endl;
      }
    }
  }
  catch(boost::system::system_error)
  {
    if (_currentSpreadsheet != NULL)
    {
      _currentSpreadsheet->UnsubscribeSession(_ID);
      _currentSpreadsheet = NULL;
    }

    std:: cout << "Caught the system error: Connection has been closed. " << '\n';
  }
}
