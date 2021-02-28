#include "Dumper.hpp"

Dumper::Dumper(std::string filePrefix){
    this->filePrefix = filePrefix;
    counter = 0;
}

std::string Dumper::dumpPage(std::string url, std::string html){
    std::string filename = (filePrefix + std::to_string(counter))+".html";

    std::ofstream dumpFile;
    dumpFile.open("dump/pages/"+filename);
    dumpFile << html;
    dumpFile.close();
    counter++;

    return filename;
}
