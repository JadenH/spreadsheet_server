/* CellChange.cpp
 *
 * Represents a single change within the spreadsheet server.
 * Contains a cell name, a previous value, and a new value.
 *
 * Author : Adam Waggoner, Jaden Holladay, Logan Ropelato, Gray Marchese
 */


#include "CellChange.h"
#include <string>


CellChange::CellChange(std::string cell, std::string prev, std::string next)
{
  cell_name = cell;
  prev_value = prev;
  next_value = next;
}

CellChange::CellChange(const CellChange& other)
{
  cell_name = other.cell_name;
  prev_value = other.prev_value;
  next_value = other.next_value;
}
