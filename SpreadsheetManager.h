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
    SpreadsheetManager();
    Sheet GetSpreadsheet(std::string name);

  private:
    // Dictionary of cells indexed by cell name, stored as strings
    // std::vector<Session> _connectedSessions;
    std::map<std::string, Sheet>* _spreadsheets;
};

#endif
