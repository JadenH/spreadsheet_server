/* Spreadsheet.h
 *
 * A single spreadsheet on the server.
 */

#include <map>
#include <queue>

public:
  Spreadsheet();
  
private:
  // Dictionary of cells indexed by cell name, stored as strings
  std::map<char*, char*> _cells;
  // Queue of the changes that have happened since startup
  std::queue<SpreadsheetChange> _changes;
