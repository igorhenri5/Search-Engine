#include "util.hpp"

bool operator<(const Site& s1, const Site& s2){
    return s1.size > s2.size;
}

int Util::calculateUrlSize(std::string url){
    int size = 1;
    std::string s = url;
    char delimiters[] = {'.', '/'};

    for(int i=0; i<2; i++){
        size_t pos = 0;
        std::string token;
        while((pos = s.find(delimiters[i])) != std::string::npos){
            size++;
            s.erase(0, pos + 1);
        }
    }
    return size;
}

std::string Util::getUrlDomain(std::string url){
  unsigned i, j=0;
  const std::string http = "https";
  
  // Ignoring the http in front of the url
  // If the url doesnt start with http:// it does nothing
  for(i=0; i < url.size() && j < http.size(); i++)
  {
    if(url[i]==http[j]) j++;
    else break;
  }
  if(j+1 >= http.size()) i+=3;
  
  for(;i < url.size(); i++)
    if(url[i]=='/') break;
  
  return url.substr(0, i);
}

std::string Util::cleanUrlPrefix(std::string url){
    std::regex vowel_re("http(s)*://(www.)*");
    std::string result;
    std::regex_replace (std::back_inserter(result), url.begin(), url.end(), vowel_re, "");

    return result;
}

//std::regex_match(str, partRegex)