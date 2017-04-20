#include "SpreadsheetManager.h"
#include <string>
#include <map>
#include "Sheet.h"
#include <iostream>

typedef std::map<std::string, Sheet*> SHEETS;

SpreadsheetManager* SpreadsheetManager::_instance = 0;

//Constructor: enforces singleton by not instantiating if instance already exists
SpreadsheetManager::SpreadsheetManager()
{
  if(_instance == 0)
  {
    std::cout << "Initializing SpreadsheetManager" << std::endl;
    _spreadsheets = std::map<std::string, Sheet*>();
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
