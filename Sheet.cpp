#include "Sheet.h"
#include <vector>
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>

Sheet::Sheet(std::string name)
{
  _name = name;
  _cells = std::map<std::string, std::string>();
  _history = std::stack<CellChange>();
  _sessions = std::map<int, Session*>();

	//Check if a file exists for this sheet and load it if it does
	std::ifstream fs(_getFilename().c_str());
	if(fs.good())
		_loadFromFile();
}

void Sheet::ReceiveMessage(int clientID, std::string message)
{
	std::vector<std::string> msg = RegexUtils::Split(message, '\t');

	if(msg.size() == 0)
		return;

	//Handle each different message type
	if(msg[0] == "Edit")
	{
		//Edit\tcellName\tcellContent\t\n
		if(msg.size() == 4 && msg[3] == "\n")
			_handleEdit(msg[1], msg[2]);

		return;
	}
	if(msg[0] == "Undo")
	{
		_handleUndo();
		return;
	}
	if(msg[0] == "IsTyping")
	{
		_broadcastMessage(message);
		return;
	}
	if(msg[0] == "DoneTyping")
	{
		_broadcastMessage(message);
		return;
	}
}

//Adds a user to this sheet
void Sheet::SubscribeSession(int clientID, Session *sesh)
{
	_mtx.lock();

	//If this client is already subscribed, return
	if(_sessions.find(clientID) != _sessions.end())
	{
		_mtx.unlock();
		return;
	}
	_sessions.insert(std::pair<int, Session*>(clientID, sesh));
	_mtx.unlock();
	_sendStartup(clientID);
}

//Removes a user from this sheet
void Sheet::UnsubscribeSession(int clientID)
{
	_mtx.lock();
	_sessions.erase(clientID);
	_mtx.unlock();
}

//Sends a message to all clients subscribed to this sheet
void Sheet::_broadcastMessage(std::string msg)
{
	_mtx.lock();
	std::map<int, Session*>::iterator it = _sessions.begin();

	while(it != _sessions.end())
	{
		it->second->DoWrite(msg);
		it++;
	}
	_mtx.unlock();
}

//Handles receiving an "Edit" message
void Sheet::_handleEdit(std::string cellName, std::string cellContents)
{
	_mtx.lock();
	//If there is already a cell defined with this cellName
	if(_cells.find(cellName) != _cells.end())
	{
		//Add a change to the history
		std::string prev = _cells[cellName];
		_cells[cellName] = cellContents;
		_history.push(CellChange(cellName, prev, cellContents));
	}
	else
	{
		_cells.insert(std::pair<std::string, std::string>(cellName, cellContents));
		_history.push(CellChange(cellName, "", cellContents));
	}
	_mtx.unlock();

	_broadcastMessage("Change\t" + cellName + "\t" + cellContents + "\t\n");

	_saveToFile();
}

void Sheet::_handleUndo()
{
	//Exit early if there is no history
	_mtx.lock();
	if(_history.size() < 1)
	{
		_mtx.unlock();
		return;
	}

	CellChange tmp = _history.top();
	_history.pop();

	_cells[tmp.cell_name] = tmp.prev_value;
	_mtx.unlock();

	_broadcastMessage("Change\t" + tmp.cell_name + "\t" + tmp.prev_value + "\t\n");

	_saveToFile();
}

//Send the Startup message to a newly connected client
void Sheet::_sendStartup(int clientID)
{
	std::string msg = "Startup\t" + std::to_string(clientID) + "\t";

	_mtx.lock();
	for(std::map<std::string, std::string>::iterator it = _cells.begin(); it != _cells.end(); ++it)
	{
		msg = msg + it->first + "\t" + it->second + "\t";
	}

	_mtx.unlock();

	msg = msg + "\n";

	_sessions[clientID]->DoWrite(msg);

}

void Sheet::_loadFromFile()
{
	_mtx.lock();

	std::ifstream file(_getFilename());

	char c; //Current character being parsed
	std::string word = ""; //Current word being assembled
	std::string name = ""; //A Cell's name

	bool isContents = false;

	while(file.get(c))
	{
		if(c == '\n')
			break;

		//If we get to a tab, we have a cell name or contents
		if(c == '\t')
		{
			if (isContents)
			{
				isContents = false;  //The next word will be a name

				//Add <name,contents> to the cells map
				_cells.insert( std::pair<std::string,std::string>(name,word));
			}
			else
			{
				isContents = true;// The next word
				name = word; //This word is the name of a cell
			}

			word = ""; //Reset the current word word

			continue; //Move on to the next characterl
		}

		word = word+c;

	}

	word = "";

	int id = 0;
	CellChange tmp_change = CellChange("","","");

	while(file.get(c))
	{
		if(c == '\n')
			continue;

		//If we get to a tab, we have a cell name or contents
		if(c == '\t')
		{
			if (id==0)
			{
				id++;//The next word will be a prev_value
				tmp_change.cell_name = word;
				//Add <name,contents> to the cells map
				_cells.insert( std::pair<std::string,std::string>(name,word));
			}
			else if(id == 1)
			{
				id++;// The next word
				tmp_change.prev_value = word;
			}
			else if(id ==2)
			{
				id = 0;
				tmp_change.next_value = word;
				std::cout <<"Loading: " << tmp_change.cell_name << " | " << tmp_change.prev_value << " | " << tmp_change.next_value << std::endl;
				_history.push(tmp_change);
			}

			word = ""; //Reset the current word word

			continue; //Move on to the next characterl
		}

		word = word+c;

	}

	file.close();

	_mtx.unlock();

}

void Sheet::Save()
{
	_saveToFile();
}

void Sheet::_saveToFile()
{

	_mtx.lock();
	std::cout << "Writing to file" << std::endl;
	std::ofstream fs;
	fs.open(_getFilename(), std::fstream::out);

	for(std::map<std::string, std::string>::iterator it = _cells.begin(); it != _cells.end(); ++it)
	{
		fs << it->first + "\t" + it->second + "\t";
	}

	fs << "\n";

	std::stack<CellChange> tmp = _history;

	while(tmp.size() > 0)
	{
		CellChange tmpChange = tmp.top();
		tmp.pop();

		fs << tmpChange.cell_name << "\t" << tmpChange.prev_value << "\t" << tmpChange.next_value << "\t";
	}

	fs.close();


	_mtx.unlock();
}

std::string Sheet::_getFilename() const
{
	return "Sheets/" + _name + ".txt";
}
