#include "SpreadsheetManager.h"
#include <string>
#include "Sheet.h"

using namespace std;

SpreadsheetManager::SpreadsheetManager()
{
  _spreadsheets = new std::map<std::string, Sheet>();
}
