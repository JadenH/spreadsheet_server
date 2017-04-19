#include "RegexUtils.h"
#include <string>
#include <boost/regex.hpp>

using namespace std;

bool RegexUtils::RegexFind(const std::string val, const std::string pattern, boost::smatch & out)
{
  boost::regex expr(pattern);
  boost::smatch matches;
  if (boost::regex_search(val, matches, expr))
  {
    out = matches;
    return true;
  }
  return false;
}

//
// int main()
// {
//     std::string input = "Change:{cellName, cellContents}\n";
//     std::string pattern = "^[^:{]*";
//     boost::smatch something;
//     if (RegexFind(input, pattern, something))
//     {
//       cout << something[0] << endl;
//     }
//
//     return 0;
// }
