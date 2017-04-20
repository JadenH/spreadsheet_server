#include "Sheet.h"
#include <vector>
#include <string>

Sheet::Sheet(std::string name)
{
  _name = name;
  _cells = std::map<std::string, std::string>();
  _history = std::queue<CellChange>();
  _sessions = std::map<int, Session*>();
}

void Sheet::ReceiveMessage(int clientID, std::string message)
{
	std::vector<std::string> msg = RegexUtils::Split(message, '\t');

	if(msg.size() == 0)
		return;

	//Handle each different message type
	if(msg[0] == "Edit")
	{
		//Edit\tcellName\tcellContent
		if(msg.size() == 3)
			_handleEdit(message, msg[1], msg[2]);

		return;
	}
	if(msg[0] == "Undo")
	{	
		_handleUndo();
		return;
	}
	
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

//Handles receiving an "Edit" message
void Sheet::_handleEdit(std::string msg, std::string cellName, std::string cellContents)
{
	//If there is already a cell defined with this cellName
	if(_cells.find(cellName) != _cells.end())
	{
		//Add a change to the history
		std::string prev = _cells[cellName];
		_history.push(CellChange(cellName, prev, cellContents));
	}
	else
	{
		_cells.insert(std::pair<std::string, std::string>(cellName, cellContents));
	}

	_broadcastMessage(msg);

}

void Sheet::_handleUndo()
{
	//Exit early if there is no history
	if(_history.size() == 0)
		return;

	CellChange tmp = _history.back();
	_history.pop();

	_broadcastMessage("Edit\t" + tmp.cell_name + "\t" + tmp.prev_value + "\n");
}

