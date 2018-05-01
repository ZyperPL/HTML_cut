#include <sstream>
#include <iostream>

#include "parser.h"

std::string Parser::trim(std::string data)
{
  std::istringstream iss(data);
  data = "";
  std::string line;
  while (getline(iss, line) )
  {
    size_t i = 0;
    for (i = 0; i < line.size(); i++)
    {
      if (line[i] != ' ' && line[i] != '\t') break;
    }
    line = line.substr(i)+"\n";
    data += line;
  }

  return data;
}


std::vector<std::string> Parser::filterByAttributes(std::vector<std::string> tags, 
                                            std::vector<struct attribute> attributes,
                                            bool silent)
{
  if (attributes.size() <= 0)
  {
    if (!silent) std::cerr << "No attributes specified!\n";
    return tags;
  }

  std::vector<std::string> elements;

  for (size_t i = 0; i < tags.size(); i++)
  {
    //element
    std::string e = tags.at(i);
    if (e == "") continue;

    //element definition begin
    size_t defBeg = e.find(" ", e.find("<"));
    if (defBeg == std::string::npos) continue;

    //element definition end
    size_t defEnd = e.find(">");
    if (defEnd == std::string::npos) continue;

    if (defBeg > defEnd) continue;

    //get element's attributes
    std::vector<struct attribute> eAttribs;    

    std::string definition = e.substr(defBeg+1, defEnd-defBeg);
    //std::cout << "|" <<definition << "|" << std::endl;
    
    for (size_t j = 0; j < definition.size(); j++)
    {
      if (definition.at(j) < ' ') {
        // replace any special characters      
        definition.at(j) = ' '; 
      }
    }

    for (size_t j = 0; j < definition.size(); j++)
    {
      if (definition.at(j) > ' ')
      {
        size_t spacePos = definition.find(" ", j);
        if (spacePos == std::string::npos) spacePos = definition.size()-1;
        char qChar = '\"'; //quote char        
        size_t quotePos = definition.find(qChar, j);

        size_t paramEnd = spacePos;
        size_t eqPos = definition.find("=", j);

        size_t extra = 0;

        if (quotePos == std::string::npos
        ||  quotePos > spacePos)
        {
          // no double quotes   
          // check if single quote
          qChar = '\'';    
          quotePos = definition.find(qChar, j);
        }

        if (quotePos != std::string::npos
        &&  quotePos < spacePos)
        {
          // single or double quotes
          paramEnd = definition.find(qChar, quotePos+1);
          extra = 1;
        } 

        std::string p, v;
        if (eqPos > spacePos) {
          // theres no equal sign
          p = definition.substr(j, spacePos-j);
          v = "true";
        } else {
          // there is equal sign
          p = definition.substr(j, eqPos-j);
          v = definition.substr(eqPos+1+extra, paramEnd-eqPos-1-extra);
        }

        //std::cout << p << "="  << v << std::endl; //debug

        struct attribute a = { p, v };
        eAttribs.push_back(a);

        j = paramEnd;
        if (j >= definition.size()) break;
      }
    }

    //check if attributes match
    bool correct = true;
    for (size_t j = 0; j < attributes.size(); j++)
    {
      correct = false;
      for (size_t k = 0; k < eAttribs.size(); k++)
      {
        if (eAttribs.at(k).name == attributes.at(j).name)
        {
          if (eAttribs.at(k).value == attributes.at(j).value)
          {
            correct = true;
          } else
          if (attributes.at(j).value == GET_PARAMETER_VALUE_STR)
          {
            correct = false;
            elements.push_back("<"+eAttribs.at(k).name+">"+eAttribs.at(k).value+"</"+eAttribs.at(k).name+">");
          }
        }
      }
    }
    if (correct) {
      elements.push_back(e);
    }

  }

  return elements;
}

std::vector<std::string> Parser::findTags(std::string data, 
                                  std::string tag,
                                  bool silent)
{
  std::vector<std::string> tags;

  // if no tag specified
  if (tag == "")
  {
    if (!silent) std::cerr << "No tag specified!\n";
    tags.push_back(data);
    return tags;
  }

  //beginning of tag
  std::string t = "<"+tag;

  //end of tag
  std::string c = "</"+tag;

  //first tag position
  size_t tagPos = data.find(t);

  //while still found
  while (tagPos != std::string::npos)
  {
    //try to find the beginning of end of tag
    size_t tagEndB = data.find(c, tagPos);

    //safety check if other tag is inside current tag
    size_t nextTagPos = data.find(t, tagPos+1);

    //other tag is inside current tag
    while (nextTagPos < tagEndB) {

      //find actual beginning of the end of the tag
      tagEndB = data.find(c, tagEndB+1);

      //check for other tag again
      nextTagPos = data.find(t, nextTagPos+1);

      //if last tag
      if (nextTagPos == std::string::npos) break;
    }
    // position of the end of the end of the tag ('>')
    size_t tagEndE = data.find(">", tagEndB)+1;

    // substr data from inside of tag
    //keep tag definition
    std::string dataFromTag;
    dataFromTag = data.substr(tagPos, tagEndE-tagPos);

    //add data to tags vector
    tags.push_back(dataFromTag);

    // check for other tags
    tagPos = data.find(t, tagPos+1);
  }

  return tags;
}


std::string Parser::extractContent(std::string data, std::string tag, bool getValue, bool silent)
{
  size_t beg = 0;
  if (tag == "") {
    tag = "html";
    beg = data.find("<"+tag);
  }
  if (getValue) { tag = ""; beg = 0; }

  size_t tagOpenEnd  = data.find(">", beg);
  size_t tagCloseBeg = data.find("</"+tag, tagOpenEnd);

  size_t lastClose = tagCloseBeg;

  while (lastClose != std::string::npos)
  {
    tagCloseBeg = lastClose;
    lastClose = data.find("</"+tag, lastClose+1); 
  }

  if (tagOpenEnd  == std::string::npos
  ||  tagCloseBeg == std::string::npos) {
    if (!silent) std::cerr << "Wrong element tag syntax!" << std::endl;
    std::cout << data << std::endl;
    return "";
  }

  std::string e = data.substr(tagOpenEnd+1, tagCloseBeg-tagOpenEnd+1-2);

  return e;
}
