#include "inputfile.h"

InputFile::InputFile(std::string name)
{
  this->file.open(name);
}

InputFile::~InputFile()
{
  this->file.close();
}

std::string InputFile::getData()
{
  std::stringstream buf;
  buf << this->file.rdbuf();
  return buf.str();
}

bool InputFile::isGood()
{
  return this->file.good();
}
