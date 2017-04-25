#ifndef SESSION_H
#define SESSION_H
/* Session.h
 *
 * A session on the server.
 *
 */

// Forward Declaration
class Sheet;

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include "RegexUtils.h"
#include "Sheet.h"

class Session : public std::enable_shared_from_this<Session>
{
  public:
    Session(boost::asio::ip::tcp::socket socket);
    void DoWrite(std::string message);
    void DoRead();
    void Start();

  private:
    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1000000000 };
    char data_[max_length];
    Sheet* _currentSpreadsheet;
    int _ID;
    static int _currentID;
    void _closeSocket(boost::system::error_code ec);
		std::string buffer_;
};

#endif
