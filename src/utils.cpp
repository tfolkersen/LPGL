#include "utils.h"
#include <iostream>

using namespace std;


bool readFile(std::string &dest, const std::string &fileName) {
    dest.clear();

    FILE *f = fopen(fileName.c_str(), "r");

    if (f == NULL) {
        cerr << "readFile() failed to open file: " << fileName << endl;
        return false;
    }

    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    dest.resize(fileSize);

    fread(dest.data(), 1, fileSize, f);

    fclose(f);

    return true;
}

