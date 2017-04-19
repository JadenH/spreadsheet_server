/* Sheet.h
 *
 * Represents one spreadsheet on the server.
 *
 */

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <map>
#include <queue>

#include "CellChange.h"

using boost::asio::ip::tcp;

class Sheet
{
  public:
    void subscribe_socket(tcp::socket socket);


  private:
    std::map<char*, char*> _cells; 
    std::queue<CellChange> _history;
};
