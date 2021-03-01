#ifndef __UTIL__
#define __UTIL__

#include <regex>

namespace Util{
    std::string htmlPreProcessing(std::string);
    void searchAndDestroy(std::string &str, std::string begin, std::string end);
    void replaceToken(std::string &str, std::string token, std::string newToken);
    bool isSupportedChar(unsigned short);
    void clearUnicode(std::string& str);

}

#endif