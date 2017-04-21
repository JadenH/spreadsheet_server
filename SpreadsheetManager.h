#ifndef SPREADSHEETMANAGER_H
#define SPREADSHEETMANAGER_H
/* SpreadsheetManager.h
 *
 * A manager for all spreadsheets on the server.
 */

#include <map>
#include <queue>
#include <vector>
#include <boost/asio.hpp>
#include "Sheet.h"

class SpreadsheetManager {
  public:
    static SpreadsheetManager* GetInstance();
		void Close();
    SpreadsheetManager();
    ~SpreadsheetManager();
    Sheet* GetSpreadsheet(std::string name);

  private:
    std::map<std::string, Sheet*> _spreadsheets;
    static SpreadsheetManager *_instance;
};

#endif
