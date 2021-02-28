#include "LongTermScheduler.hpp"
#include <iostream>

std::priority_queue<Site> LongTermScheduler::queue;

LongTermScheduler::LongTermScheduler(){
    
}

void LongTermScheduler::pushSite(Site site){
    if(LongTermScheduler::queue.size()<5000)
        LongTermScheduler::queue.push(site);
}

Site LongTermScheduler::getFirstSite(){
    if(LongTermScheduler::isEmpty())
        return Site("",0);
    return LongTermScheduler::queue.top();
}

void LongTermScheduler::popSite(){
    if(!LongTermScheduler::isEmpty())
        LongTermScheduler::queue.pop();
}

bool LongTermScheduler::isEmpty(){
    return LongTermScheduler::queue.empty();
}

void LongTermScheduler::printQueueStatus(){
    std::cout << "Queue size : " << LongTermScheduler::queue.size() << std::endl;
}

int LongTermScheduler::queueSize(){
    return LongTermScheduler::queue.size();
}