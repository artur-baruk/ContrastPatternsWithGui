#ifndef STRINGTOOLS_H_INCLUDED
#define STRINGTOOLS_H_INCLUDED

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

int strToInt(const std::string& str) {
    int i;
    std::istringstream iss(str);
    iss >> i;
    return i;
}

float strToFloat(const std::string& str) {
    float i;
    std::istringstream iss(str);
    iss >> i;
    return i;
}

#endif // STRINGTOOLS_H_INCLUDED
