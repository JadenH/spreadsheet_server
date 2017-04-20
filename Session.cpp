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

void Session::DoRead()
{
  auto self(shared_from_this());

	socket_.async_read_some(boost::asio::buffer(data_, max_length),
                          [this, self](boost::system::error_code ec, std::size_t BytesRead)
   {
		if (!ec)
		{
		  data_[BytesRead] = '\0';
		  std::cout << "Message Received: " << data_ << std::endl;

			//Get a list of each field in the message (delimited by tabs)
			std::vector<std::string> msg = RegexUtils::Split(data_, '\t');
			
			//If a connect message is received, we have to connect to a specific spreadsheet
			if (msg[0] == "Connect" && msg.size() == 2 && msg[1] != "")
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
				_currentSpreadsheet->ReceiveMessage(_ID,data_);
			}
		  /*boost::smatch matches;
		  if (RegexUtils::RegexFind(theData, "^[^:{]*", matches))
		  {
		    if (matches[0] == "Connect")
		    {
		      if (RegexUtils::RegexFind(theData, "([a-zA-Z0-9]*)}", matches))
		      {
		        std::cout << "Connecting to sheet: " << matches[1] << std::endl;
            _currentSpreadsheet = matches[1];
						SpreadsheetManager::GetInstance()->GetSpreadsheet(_currentSpreadsheet)->SubscribeSession(this);
		      }
		    }
		    else
		    {
          if(_currentSpreadsheet != "")
  		      SpreadsheetManager::GetInstance()->GetSpreadsheet(_currentSpreadsheet)->ReceiveMessage(theData);
		    }
		  }*/

		  //std::cout <<"Data: " << theData << std::endl;
		  DoRead();
		}
		else
		{
		  //Remove from spreadsheets and disconnect socket.
		  try
		  {
			  std::string error;
			  std:: string eof1 ("End of file");
			  std:: string eof2 ("Connection reset by peer");
			  error = ec.message();
			  //std::cout<<"Error Message:" << error <<'\n';
			  if ((error.compare(eof1) == 0 || error.compare(eof2) == 0))
				  {
				  	socket_.shutdown(socket_.shutdown_both);
					socket_.close();
					std::cout << "Safely closed socket after client disconnect within try catch." << std::endl;
					return;		  
				  }
		  }
		  catch(boost::system::system_error)
			  {
			  	std:: cout << "Caught the system error: Connection has been closed. " << '\n';
			  }

		}//end else			
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
