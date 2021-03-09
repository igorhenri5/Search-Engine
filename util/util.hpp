#ifndef __UTIL__
#define __UTIL__

#include <string>

namespace Util{
    void to_lower(std::string& str);
    void regex_special_token_clear(std::string& str);
    bool is_supported_char(unsigned short c);
    void clear_unicode(std::string &str);
    void search_and_destroy(std::string &str, std::string begin, std::string end);
    std::string html_pre_processing(std::string url);
}

#endif