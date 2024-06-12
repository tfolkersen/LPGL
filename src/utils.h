#pragma once
#include <string>
#include <iostream>
#include <vector>

#define DPRINT(x) (std::cout << x << std::endl);
#define DMACRO(x) (x)

bool readFileText(std::string &dest, const std::string &fileName);
// TODO: readFileRaw

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    size_t size = vec.size();

    os << "[";
    for (size_t i = 0; i < size; i++) {
        os << vec[i];
        
        if (i + 1 < size) {
            os << ", ";
        }
    }
    os << "]";

    return os;
}

template <class T>
std::ostream &operator<<(std::ostream &os, std::vector<T> &vec) {
    size_t size = vec.size();

    os << "[";
    for (size_t i = 0; i < size; i++) {
        os << vec[i];
        
        if (i + 1 < size) {
            os << ", ";
        }
    }
    os << "]";

    return os;
}
