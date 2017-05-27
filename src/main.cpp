#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "inputfile.h"

#define GOOD_EXIT 0
#define ARGUMENTS_EXIT 1
#define FILE_EXIT 2

struct attribute
{
  std::string name;
  std::string value;
};

std::vector<std::string> filterByAttributes(std::vector<std::string> tags, 
                                            std::vector<struct attribute> attributes,
                                            bool silent = false)
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

    std::string definition = e.substr(defBeg+1, defEnd-defBeg-1);
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

std::vector<std::string> findTags(std::string data, 
                                  std::string tag,
                                  bool silent = false)
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

void help(int argc, char *argv[])
{
  argc = argc; //ignore warning

  std::cerr << argv[0] << " [OPTIONS]..." << std::endl;
  std::cerr << "Reads from stdin or file and cuts HTML document based on tags and attributes";
  std::cerr << std::endl;
  std::cerr << "\t-f [HTML FILE]\t--file=[HTML FILE]" << std::endl;
  std::cerr << "\t-t [TAG]\t--tag=[TAG]" << std::endl;

  std::cerr << "\t-a [ATTRIBUTE]=[VALUE]\t--attribute=[ATTRIBUTE]=[VALUE] (VALUE=true for empty attribute)" << std::endl;   
  std::cerr << "\t--trim\t - left trim spaces and tabs" << std::endl;
  std::cerr << "\t--keep\t - keep tags, not just the content" << std::endl;
  std::cerr << "\t--silent\t - disable all error messages being printed to stderr" << std::endl;
  std::cerr << std::endl;
}

int main(int argc, char *argv[])
{
  // change argv to strings
  std::vector<std::string> args;
  for (uint16_t i = 1; i < argc; i++)
  {
    args.push_back(argv[i]);
  }

  // parse arguments
  std::string name = "", tag = "";

  std::vector<struct attribute> attributes;
  bool wantHelp = false, 
       trim = false, 
       keep = false,
       silent = false;
  for (uint16_t i = 0; i < args.size(); i++)
  {
    std::string arg = args.at(i); // current argument

    std::string option = "", parameter = "";

    if (arg[0] == '-')
    {
      const int DASH_N = 2; // number of dashes

      // if long option
      if (arg.substr(0,DASH_N) == "--") 
      {
        size_t eqPos = arg.find("=", DASH_N);
        if (arg == "--help" 
        || arg == "--trim" 
        || arg == "--keep"
        || arg == "--silent") eqPos = 0; // to prevent errors when no '='
        if (eqPos == std::string::npos) {
          if (!silent) std::cerr << "Equal sign missing at argument " << i << std::endl;
          return ARGUMENTS_EXIT;
        }
        option = arg.substr(DASH_N, eqPos-DASH_N);
        parameter = arg.substr(eqPos+1);
      } 
      else // if short option
      {
        option = arg[1];
        if (i < args.size()-1) {
          std::string narg = args.at(i+1); // next argument
          parameter = narg;
        } 
      }
    } else {
      continue; // argument is not an option
    }

    //std::cout << i << ": " <<  option << "=" << parameter << std::endl; //debug

    if (option == "f" || option == "file")
    {
      name = parameter;
    } else
    if (option == "h" || option == "help")
    {
      wantHelp = true;
    } else
    if (option == "t" || option == "tag")
    {
      if (parameter != "") {
        tag = parameter;
      }
    } else
    if (option == "a" || option == "attribute")
    {
      if (parameter != "") {
        size_t eqPos = parameter.find("=");
        if (eqPos == std::string::npos) {
          if (!silent) std::cerr << "Wrong attribute parameters at argument " << i << std::endl;
          return ARGUMENTS_EXIT;
        }
        std::string parameterName   = parameter.substr(0, eqPos);
        std::string parameterValue  = parameter.substr(eqPos+1);
        struct attribute a = { parameterName, parameterValue };
        attributes.push_back(a);
      }
    } else
    if (option == "trim")
    {
      trim = true;
    } else
    if (option == "keep")
    {
      keep = true;
    } else
    if (option == "silent")
    {
      silent = true;
    }
  }

  std::string data = "";
  if (name == "" && !wantHelp) {
    //read from stdin
    std::string line;
    while (std::getline(std::cin, line) ) { 
      data += line + "\n";
    }
  }

  // 'help' in arguments
  if ( (data == "" && name == "") || wantHelp)
  {
    help(argc, argv);
    return ARGUMENTS_EXIT;
  }

  //if file name provided
  if (name != "")
  {
    // open file
    InputFile *file = new InputFile(name);
    if (!file->isGood())
    {
      if (!silent) {
        std::cerr << "Cannot open file " << name;
        std::cerr << std::endl;
      }
      delete file;
      return FILE_EXIT;
    }

    // get data
    data = file->getData();

    // close file
    delete file;
  }

  //left trim data from spaces and tabs
  if (trim) {
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
  }

  // get tags
  std::vector<std::string> tags;
  tags = findTags(data, tag, silent);

  // filter by attributes
  std::vector<std::string> elements;
  elements = filterByAttributes(tags, attributes, silent);

  //print results
  for (size_t i = 0; i < elements.size(); i++)
  {
    if (keep)
    {
      std::cout << elements.at(i) << std::endl;
    } else
    {
      size_t beg = 0;
      if (tag == "") {
        tag = "html";
        beg = elements.at(i).find("<"+tag);
      }

      size_t tagOpenEnd  = elements.at(i).find(">", beg);
      size_t tagCloseBeg = elements.at(i).find("</"+tag, tagOpenEnd);

      size_t lastClose = tagCloseBeg;

      while (lastClose != std::string::npos)
      {
        tagCloseBeg = lastClose;
        lastClose = elements.at(i).find("</"+tag, lastClose+1); 
      }

      if (tagOpenEnd  == std::string::npos
      ||  tagCloseBeg == std::string::npos) {
        if (!silent) std::cerr << "Wrong element tag syntax!" << std::endl;
        std::cout << elements.at(i) << std::endl;
        continue;
      }

      std::string e = elements.at(i).substr(tagOpenEnd+1, tagCloseBeg-tagOpenEnd+1-2);
      std::cout << e << std::endl;
    }
  }

  return GOOD_EXIT;
}
