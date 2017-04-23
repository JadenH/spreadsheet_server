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
#include <stack>
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
		void Save();

  private:
    std::map<std::string, std::string> _cells;
    std::stack<CellChange> _history;
    std::map<int, Session*> _sessions;
    std::map<int, std::string> _currentCell;
    std::string _name;
		std::mutex _mtx;
		//This mutex is used to lock a file while it's being saved to.
		std::mutex _fileMutex;

		void _loadFromFile();
		void _saveToFile();
		std::string _getFilename() const;

		//Handle specific messages
		void _handleEdit(std::string cellName, std::string cellContents);
		void _handleIsTyping(std::string clientID, std::string cellName);
		void _handleUndo();
		void _sendStartup(int id);

		bool _isValidCellname(std::string inp);

		void _broadcastMessage(std::string msg);
};

#endif
