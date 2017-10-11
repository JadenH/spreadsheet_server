#include "SpreadsheetManager.h"
#include "Sheet.h"
#include <string>
#include <map>
#include <iostream>
#include <dirent.h>
#include <stdio.h>

typedef std::map<std::string, Sheet*> SHEETS;

SpreadsheetManager* SpreadsheetManager::_instance = 0;

// Constructor: enforces singleton by not instantiating if instance already exists
SpreadsheetManager::SpreadsheetManager()
{
  // Only create an instance of this class if one does not yet exist
  if(_instance == 0)
  {
    // Initialize the internal storage for the sheets
    _spreadsheets = std::map<std::string, Sheet*>();

	// Set up reference to the directory for saving spreadsheets, create one if not already created
    mkdir("Sheets", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    DIR *dir;
    struct dirent *dp;
    char* filename;
    dir = opendir("Sheets");

    while ((dp=readdir(dir)) != NULL)
    {
      std::string filename = dp->d_name;
      filename = filename.substr(0, filename.size()-4);

	  // Validate the filename to ensure that it contains alphanumeric characters, and wont attempt
	  // to save files outside of the server.
      if(RegexUtils::IsValidFilename(filename))
      {
        GetSpreadsheet(filename);
      }
    }

  }
}

// Returns a spreadsheet on the server if it already exists, creates one if not
Sheet* SpreadsheetManager::GetSpreadsheet(std::string name)
{
  SHEETS::iterator it = _spreadsheets.find(name);

  // If there is no spreadsheet with the given name, create it
  if(it == _spreadsheets.end())
  {
    _spreadsheets.insert(std::pair<std::string, Sheet*>(name, new Sheet(name)));
    return _spreadsheets[name];
  }

  return it->second;
}

// Gracefully close the server and save all of the sheets
void SpreadsheetManager::Close()
{
  SHEETS::iterator it = _spreadsheets.begin();

  while(it != _spreadsheets.end())
  {
    it->second->Save();
    it++;
  }
}

// Returns the instance if it exists, creates one otherwise
SpreadsheetManager* SpreadsheetManager::GetInstance()
{
  if(_instance == 0)
  {
    _instance = new SpreadsheetManager();
  }
}

// Destructor
SpreadsheetManager::~SpreadsheetManager()
{
  if(_instance != 0)
  {
    free(_instance);
    _instance = 0;
  }
}
