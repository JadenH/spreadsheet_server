#include <iostream>
#include <string>
#include <boost/regex.hpp>

using namespace std;

bool RegexFind(const std::string val, const std::string pattern, boost::smatch & out)
{
  boost::regex expr(pattern);
  boost::smatch what;
  if (boost::regex_search(val, what, expr))
  {
    out = what;
    return true;
  }
  return false;
}

int main()
{
    std::string input = "Change:{cellName, cellContents}\n";
    std::string pattern = "^[^:{]*";
    boost::smatch something;
    if (RegexFind(input, pattern, something))
    {
      cout << something[0] << endl;
    }

    return 0;
}
