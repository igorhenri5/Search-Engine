#ifndef __LTERM__
#define __LTERM__

#include "../site/Site.hpp"
#include "../util/util.hpp"

#include <queue>

class LongTermScheduler{

private:
    static std::priority_queue<Site> queue;

public:
    LongTermScheduler();

    static void pushSite(Site);
    static Site getFirstSite();
    static void popSite();
    static bool isEmpty();
    static void printQueueStatus();
    static int queueSize();

};

#endif