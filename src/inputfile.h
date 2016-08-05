#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class InputFile
{
  private:
    std::ifstream file;
  public:
    InputFile(std::string name);
    ~InputFile();
    std::string getData();
    bool isGood();
};
