/* CellChange.cpp
 * 
 * Represents a single change within the spreadsheet server.
 * Contains a cell name, a previous value, and a new value.
 *
 * Author: Adam Waggoner
 */


#include "CellChange.h"
#include <string>

 
CellChange::CellChange(char* cell, char* prev, char* next)
{
	cell_name = cell;
	prev_value = prev;
	next_value = next;
}
