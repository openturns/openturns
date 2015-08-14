//                                               -*- C++ -*-
/**
 *  @brief Simple code for generating the C++ source code of an analytical
 *         function whose formula is given by a valid C++ expression, then
 *         compiling the source code and generating the OpenTURNS xml wrapper
 *         following the OpenTURNs dtd.
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <cstdlib>
#include <cstring>


#include "AnalyticalFunctionCompiler.hxx"

int main(int argc, char** argv)
{
  if(argc == 1)
  {
    std::cout << "Usage: AnalyticalFunctionCompiler <path> <name> <formula> <variable 1> ... <variable n>" << std::endl;
    std::cout << "  set path=""NULL"" if you want an empty string for path" << std::endl;
    exit(AnalyticalFunctionCompiler::BAD_ARGUMENTS);
  }

  std::string path("");
  if (strcmp(argv[1], "NULL"))
  {
    path = argv[1];
  }
  std::string name(argv[2]);
  std::string formula(argv[3]);

  std::list<std::string> variates;
  for(int argIndex = 4; argIndex < argc; argIndex++)
  {
    variates.push_back(std::string(argv[argIndex]));
  }
  AnalyticalFunctionCompiler::AnalyticalFunctionCompiler myFunction(name, formula, path, variates);
  myFunction.createSourceCode();
  myFunction.createWrapper();
  myFunction.createLibrary();
  exit(AnalyticalFunctionCompiler::EXIT_OK);
}
