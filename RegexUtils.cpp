#include "RegexUtils.h"
#include <string>
#include <boost/regex.hpp>
#include <sstream>

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

//Citation: Code from http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> RegexUtils::Split(std::string inp, char delimiter)
{
	stringstream ss(inp);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delimiter)) {
		  tokens.push_back(item);
	}
	return tokens;
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
