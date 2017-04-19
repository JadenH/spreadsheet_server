/* RegexUtils.h
 *
 * Represents one spreadsheet on the server.
 *
 */

 #include <iostream>
 #include <string>
 #include <boost/regex.hpp>

namespace cs3505
{
  class RegexUtils
  {
    public:
      static void RegexFind(const std::string val, const std::string pattern, boost::smatch & out);
  };
}
