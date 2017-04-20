#ifndef CELLCHANGE_H
#define CELLCHANGE_H
/* CellChange.h
 *
 * Represents a single change within the spreadsheet server.
 * Contains a cell name, a previous value, and a next value.
 *
 * Author: Adam Waggoner
 */

#include <string>

class CellChange
{
  public:
    CellChange(std::string cell, std::string prev, std::string next);
    std::string cell_name;
    std::string prev_value, next_value;
};

#endif
