#ifndef SHEET_H
#define SHEET_H
/* Sheet.h
 *
 * Represents one spreadsheet on the server.
 *
 */

//Forward declare Session
class Session;

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <map>
#include <queue>
#include <vector>

#include "Session.h"
#include "CellChange.h"


class Sheet
{
  public:
    Sheet(std::string name);
    void ReceiveMessage(std::string message);
		void SubscribeSession(int id, Session *sesh);
		void UnsubscribeSession(int id);

  private:
    std::map<char*, char*> _cells;
    std::queue<CellChange> _history;
    std::map<int, Session*> _sessions;
    std::string _name;

		void _broadcastMessage(std::string msg);
};

#endif
