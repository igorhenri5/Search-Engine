#include <iostream>
#include <string>
#include <regex>

using namespace std;

std::string cleanUrlPrefix(std::string url){

    std::regex vowel_re("http(s)*://(www.)*");
    std::string result;
    std::regex_replace (std::back_inserter(result), url.begin(), url.end(), vowel_re, "");

    return result;
}

int main(int argc, char* argv[]){
    std::string text = "https://www.tudogostoso.uol.com.br";
    string result = cleanUrlPrefix(text);
    cout << result << endl;
    
    return 0;
}