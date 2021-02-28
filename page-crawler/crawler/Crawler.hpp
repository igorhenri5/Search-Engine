#ifndef __CRAWL__
#define __CRAWL__

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>
#include <chrono>
#include <list>
#include <map>
#include <utility>
#include <fstream>

#include <CkSpider.h>
#include <CkString.h>

#include "../util/util.hpp"
#include "../site/Site.hpp"
#include "../sched/LongTermScheduler.hpp"
#include "../dumper/Dumper.hpp"

class Crawler{
private:
    int numThreads;
    
    static bool done;
    static int numGathered;
    static int numThreadsGathering;    
    static std::mutex longTermScheduler_mutex;    
    static std::mutex numGathered_mutex;
    static std::mutex numThreadsGathering_mutex;
    static std::mutex visitedPages_mutex;

    static std::map<std::string,std::string> visitedPages;
    static bool checkVisitedPage(std::string);
    static bool insertVisitedPage(std::string, std::string);
    static bool editVisitedPage(std::string, std::string);
    static void saveVisitedPagesStatus();

public:
    Crawler(int);
    void start();
    void crawl();
};

#endif
