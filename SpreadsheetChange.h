/* SpreadsheetChange.h
 * 
 * Represents a single change within the spreadsheet server.
 * Contains a cell name, a previous value, and a next value.
 *
 * Author: Adam Waggoner
 */

#include <string>

class SpreadsheetChange
{
  public:
    SpreadsheetChange(char* cell, char* prev, char* next);
    char* cell_name; 
    char* prev_value,* next_value;
};
