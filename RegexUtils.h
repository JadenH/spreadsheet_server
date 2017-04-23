#ifndef REGEXUTILS_H
#define REGEXUTILS_H
/* RegexUtils.h
 *
 * Represents one spreadsheet on the server.
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <boost/regex.hpp>

class RegexUtils
{
  public:
    static bool RegexFind(const std::string val, const std::string pattern, boost::smatch & out);
		static bool IsValidFilename(std::string val);
		static std::vector<std::string> Split(std::string inp, char delimiter);
};

#endif
