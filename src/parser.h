#pragma once

#include <vector>
#include <string>

#define GET_PARAMETER_VALUE_STR "\1*\1"

struct attribute
{
  std::string name;
  std::string value;
};

/*
 * Parses HTML data
 */
class Parser
{
  public:
    std::string trim(std::string data);

    std::vector<std::string> filterByAttributes(std::vector<std::string> tags, 
                                                std::vector<struct attribute> attributes,
                                                bool silent = false);


    std::vector<std::string> findTags(std::string data, 
                                      std::string tag,
                                      bool silent = false);

    std::string extractContent(std::string data, 
                              std::string tag, 
                              bool getValue, 
                              bool silent = false);
};
