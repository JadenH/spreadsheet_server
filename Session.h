#ifndef SESSION_H
#define SESSION_H
/* Session.h
 *
 * A session on the server.
 *
 */

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

class Session : public std::enable_shared_from_this<Session>
{
  public:
    Session(tcp::socket socket, SpreadsheetManager* spreadsheetManager);
    void DoWrite(std::string message);
    void DoRead();
    void Start();

  private:
    tcp::socket socket_;
    enum { max_length = 1000000000 };
    char data_[max_length];
    SpreadsheetManager* _spreadsheetManager;
    Sheet* _currentSpreadsheet;

};

#endif
