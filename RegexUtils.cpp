/*	RegexUtils.cpp
*		Contains useful functions for dealing with string parsing.
*/

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

//Validates that a file name is legal
bool RegexUtils::IsValidFilename(const std::string name)
{
	if(name.size() > 255)
		return false;
	
	boost::regex expr("^[\\w\\- ]+$");
	boost::smatch dummy;
	
	return boost::regex_search(name, dummy, expr);
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
