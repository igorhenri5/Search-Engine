#include "Site.hpp"
// #include <iostream>

Site::Site(std::string url){
    this->url = url;
    this->size = Util::calculateUrlSize(url);

} 

Site::Site(std::string url, int size){
    this->url = url;
    this->size = size;
}

