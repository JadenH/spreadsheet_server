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
  _currentCell = std::map<int, std::string>();

	//Check if a file exists for this sheet and load it if it does
	std::ifstream fs(_getFilename().c_str());
	if(fs.good())
		_loadFromFile();
}

//Handles a message from any session connected to this session.
bool Sheet::ReceiveMessage(int clientID, std::string message)
{
	std::vector<std::string> msg = RegexUtils::Split(message, '\t');

	if(msg.size() == 0)
		return false;

	//Handle each different message type

	//Edit\tcellName\tcellContent\t\n
	if(msg[0] == "Edit")
	{
		if(msg.size() == 4 && _isValidCellname(msg[1]) && msg[3] == "\n")
			_handleEdit(msg[1], msg[2]);
		else
			return false;
		return true;
	}
	//Undo\t\n
	if(msg[0] == "Undo" && msg.size() == 2 && msg[1] == "\n")
	{
		_handleUndo();
		return true;
	}
	//IsTyping\tuserID\tCellname\t\n
	if(msg[0] == "IsTyping" && msg.size() == 4 && RegexUtils::IsInt(msg[1]) && _isValidCellname(msg[2]) && msg[3] == "\n")
	{
		_handleIsTyping(msg[1],msg[2]);
		_broadcastMessage(message);
		return true;
	}
	//IsTyping\tuserID\tCellname\t\n
	if(msg[0] == "DoneTyping" && msg.size() == 4 && RegexUtils::IsInt(msg[1]) && _isValidCellname(msg[2]) && msg[3] == "\n")
	{
		_broadcastMessage(message);
		return true;
	}

	std::cout << "Corrupt message, disconnecting client." << std::endl;
	return false;
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

	std::cout << "Unsubscribed Session." << std::endl;

  _sessions.erase(clientID);
  
  std::string clientCell = _currentCell[clientID]; 
  std::string endMessage = "DoneTyping\t" + std::to_string(clientID) + "\t"+ clientCell +"\t\n";
  _currentCell.erase(clientID);

	_mtx.unlock();
	
	
	_broadcastMessage(endMessage);
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

//Handles IsTyping messages from the client
void Sheet::_handleIsTyping(std::string clientID, std::string cellName)
{
	return;

	_mtx.lock();
	
	int ID = std::stoi(clientID);
	
	std::pair<std::map<int,std::string>::iterator,bool> mapPair;

	mapPair = _currentCell.insert( std::pair<int,std::string>(ID,cellName));

	if( mapPair.second == false)
	{
		mapPair.first->second = cellName;
	}
	
	_mtx.unlock();
}

//Handles an Undo message from a client
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
//Also sends current selected cells
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
	
	_mtx.lock();
	
	//Send information about who is typing where
	for(std::map<int, std::string>::iterator it = _currentCell.begin(); it != _currentCell.end(); ++it)
	{
		int otherID = it->first;
		std::string cellName = it->second;
		
		_sessions[clientID]->DoWrite("IsTyping\t"+std::to_string(otherID)+'\t'+cellName+"\t\n");
	}
	
	_mtx.unlock();
}

//Parse cell and history data into this object
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

//Returns true if "name" matches a cellname
bool Sheet::_isValidCellname(std::string name)
{
	std::string validNamePattern = "^[A-Z]+[1-9][0-9]*$";
	
	boost::smatch dummy;
	
	//Just create a dummy smatch so we can call the RegexFind function
	return RegexUtils::RegexFind( name, validNamePattern,dummy);
}

void Sheet::_saveToFile()
{
	//Lock the datastructures and copy them for saving.
	_mtx.lock();
	std::map<std::string, std::string> cellCopy = _cells;
	std::stack<CellChange> histCopy = _history;
	_mtx.unlock();

	//Lock the filewriter mutex
	_fileMutex.lock();
	std::cout << "Beginning file write in " << _getFilename() << std::endl;
	//Open the file into a stream
	std::ofstream fs;
	fs.open(_getFilename(), std::fstream::out);

	//Write each cellchange into the file in the same format as a startup message.
	for(std::map<std::string, std::string>::iterator it = cellCopy.begin(); it != cellCopy.end(); ++it)
	{
		fs << it->first + "\t" + it->second + "\t";
	}

	//Write a newline to separate history from cells
	fs << "\n";

	//Write each change in order in the format "Celllname\tPrev_value\tNext_Value
	while(histCopy.size() > 0)
	{
		CellChange tmpChange = histCopy.top();
		histCopy.pop();

		fs << tmpChange.cell_name << "\t" << tmpChange.prev_value << "\t" << tmpChange.next_value << "\t";
	}

	//Close the filestream
	fs.close();
	
	std::cout << "Spreadsheet " << _getFilename() << " written to file" << std::endl;

	_fileMutex.unlock();
}

//Returns the filepath to this spreadsheet
// NOTE: This filepath requires that there is a "Sheets" directory in the server's root.
std::string Sheet::_getFilename() const
{
	return "Sheets/" + _name + ".txt";
}
