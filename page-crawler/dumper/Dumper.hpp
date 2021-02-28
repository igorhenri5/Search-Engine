#ifndef __DUMPR__
#define __DUMPR__

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

class Dumper{
private:
    // std::string filename;
    // std::ofstream dumpFile;
    std::string filePrefix;
    int counter;
public:
    Dumper(std::string);
    // void dump(std::string, std::string);
    std::string dumpPage(std::string, std::string);
    // void close();
};

#endif
