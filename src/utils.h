#pragma once
#include <string>
#include <iostream>

#define DPRINT(x) (std::cout << x << std::endl);
#define DMACRO(x) (x)

bool readFile(std::string &fileData, const std::string &fileName);
