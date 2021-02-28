#ifndef __WSITE__
#define __WSITE__

#include <string>
#include "../util/util.hpp"

class Site{

public:
    std::string url;
    int size;
    Site(std::string);
    Site(std::string, int);
};

#endif