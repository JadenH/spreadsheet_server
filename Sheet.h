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
#include <mutex>

class Sheet
{
  public:
    Sheet(std::string name);
    void ReceiveMessage(int id, std::string message);
		void SubscribeSession(int id, Session *sesh);
		void UnsubscribeSession(int id);

  private:
    std::map<std::string, std::string> _cells;
    std::queue<CellChange> _history;
    std::map<int, Session*> _sessions;
    std::string _name;
		std::mutex _mtx;

		void _loadFromFile();
		void _saveToFile();
		std::string _getFilename() const;

		//Handle specific messages
		void _handleEdit(std::string msg, std::string cellName, std::string cellContents);
		void _handleUndo();
		void _sendStartup(int id);

		void _broadcastMessage(std::string msg);
};

#endif
