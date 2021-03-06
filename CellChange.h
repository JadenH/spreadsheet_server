#ifndef CELLCHANGE_H
#define CELLCHANGE_H
/* CellChange.h
 *
 * Represents a single change within the spreadsheet server.
 * Contains a cell name, a previous value, and a next value.
 *
 * Author: Adam Waggoner, Jaden Holladay, Logan Ropelato, Gray Marchese
 */

#include <string>

class CellChange
{
  public:
    CellChange(std::string cell, std::string prev, std::string next);
    CellChange(const CellChange& other);
    std::string cell_name;
    std::string prev_value, next_value;
};

#endif
