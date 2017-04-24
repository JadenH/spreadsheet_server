#include "SpreadsheetManager.h"
#include <string>
#include <map>
#include "Sheet.h"
#include <iostream>
#include <dirent.h>
#include <stdio.h>

typedef std::map<std::string, Sheet*> SHEETS;

SpreadsheetManager* SpreadsheetManager::_instance = 0;

//Constructor: enforces singleton by not instantiating if instance already exists
SpreadsheetManager::SpreadsheetManager()
{
  if(_instance == 0)
  {
    std::cout << "Initializing SpreadsheetManager" << std::endl;
    _spreadsheets = std::map<std::string, Sheet*>();
		
		mkdir("Sheets", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		DIR *dir;
		struct dirent *dp;
		char* filename;
		dir = opendir("Sheets");

		while ((dp=readdir(dir)) != NULL)
		{
			std::string filename = dp->d_name;
			std::cout << "debug: " << dp->d_name << std::endl;
			filename = filename.substr(0, filename.size()-4);
			if(RegexUtils::IsValidFilename(filename))
			{
				GetSpreadsheet(filename);
			}
		}

  }
}

Sheet* SpreadsheetManager::GetSpreadsheet(std::string name)
{
  std::cout << "GetSpreadsheet: " << name << std::endl;
  SHEETS::iterator it = _spreadsheets.find(name);
  //If there is no spreadsheet with the given name, create it
  if(it == _spreadsheets.end())
  {
    _spreadsheets.insert(std::pair<std::string, Sheet*>(name, new Sheet(name)));
    //_spreadsheets[name] = new Sheet(name);
    std::cout <<"Returning: " << name << std::endl;
    return _spreadsheets[name];
  }
  
  return it->second;
} 

void SpreadsheetManager::Close()
{
	SHEETS::iterator it = _spreadsheets.begin();

	while(it != _spreadsheets.end())
	{
		it->second->Save();
		it++;
	}
}

//Returns the instance if it exists, creates one otherwise
SpreadsheetManager* SpreadsheetManager::GetInstance()
{
  if(_instance == 0)
  {
    _instance = new SpreadsheetManager();
  }
}

//Destructor
SpreadsheetManager::~SpreadsheetManager()
{
  if(_instance != 0)
  {
    free(_instance);
    _instance = 0;
  }
}
