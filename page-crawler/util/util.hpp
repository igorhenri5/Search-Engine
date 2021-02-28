#ifndef __UTIL__
#define __UTIL__

#define GATHER_LIMIT 1
#define THREAD_NUM  1

class Site;
#include "../site/Site.hpp"
#include <regex>

bool operator<(const Site&, const Site&);

namespace Util{
    int calculateUrlSize(std::string);
    std::string getUrlDomain(std::string url);
    std::string cleanUrlPrefix(std::string url);
}

#endif