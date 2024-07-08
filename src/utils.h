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

template <class T>
T constrain(const T &low, const T &val, const T &high) {
    if (val < low) {
        return low;
    }

    if (val > high) {
        return high;
    }

    return val;
}

template <class T>
void printBits(T x) {
    int bits = sizeof(T) * 8;

    for (int shift = bits - 1; shift >= 0; shift--) {
        std::cout << ((x >> shift) & 1);
    }

}

