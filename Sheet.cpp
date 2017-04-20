#include "Sheet.h"
#include <vector>
#include <string>

Sheet::Sheet(std::string name)
{
  _name = name;
  _cells = std::map<char*, char*>();
  _history = std::queue<CellChange>();
  _sessions = std::map<int, Session*>();
}

void Sheet::ReceiveMessage(std::string message)
{
	_broadcastMessage(message);

	std::vector<std::string> msg = RegexUtils::Split(message, '\t');
}

void Sheet::SubscribeSession(int clientID, Session *sesh)
{
	_sessions.insert(std::pair<int, Session*>(clientID, sesh));
}

void Sheet::UnsubscribeSession(int clientID)
{
	_sessions.erase(clientID);
}

//Sends a message to all clients subscribed to this sheet
void Sheet::_broadcastMessage(std::string msg)
{
	std::map<int, Session*>::iterator it = _sessions.begin();

	while(it != _sessions.end())
	{
		it->second->DoWrite(msg);
		it++;
	}
}
