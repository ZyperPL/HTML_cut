#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "parser.h"
#include "inputfile.h"

#define GOOD_EXIT       0
#define ARGUMENTS_EXIT  1
#define FILE_EXIT       2

void help(int argc, char *argv[])
{
  (void)argc; //prevent warning

  std::cerr << argv[0] << " [OPTIONS]..." << std::endl;
  std::cerr << "Reads from stdin or file and cuts HTML document based on tags and attributes";
  std::cerr << std::endl;
  std::cerr << "\t-f [HTML FILE]\t--file=[HTML FILE]" << std::endl;
  std::cerr << "\t-t [TAG]\t--tag=[TAG]" << std::endl;

  std::cerr << "\t-a [ATTRIBUTE]=[VALUE]\t--attribute=[ATTRIBUTE]=[VALUE] (VALUE=true for empty attribute)" << std::endl;   
  std::cerr << "\t-a [ATTRIBUTE]\t --attribute=[ATTRIBUTE] prints value of attribute" << std::endl;
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
       silent = false,
       getValue = false;
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

        std::string parameterValue;

        if (eqPos == std::string::npos) {
          parameterValue = GET_PARAMETER_VALUE_STR;
          getValue = true;          
        } else
        {
          // looking for parameter's value
          parameterValue  = parameter.substr(eqPos+1);
        }

        std::string parameterName   = parameter.substr(0, eqPos);
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

  Parser parser;

  //left trim data from spaces and tabs
  if (trim) {
    data = parser.trim(data);
  }

  // get tags
  std::vector<std::string> tags;
  tags = parser.findTags(data, tag, silent);

  // filter by attributes
  std::vector<std::string> elements;
  elements = parser.filterByAttributes(tags, attributes, silent);

  //for (auto e : elements) printf("Element: '%s'\n", e.c_str());

  //print results
  for (size_t i = 0; i < elements.size(); i++)
  {
    if (keep)
    {
      std::cout << elements.at(i) << std::endl;
    } else
    {

      std::string e = parser.extractContent(elements.at(i), tag, getValue, silent);
      
      if (e != "") {
        std::cout << e << std::endl;
      }

    }
  }

  return GOOD_EXIT;
}
