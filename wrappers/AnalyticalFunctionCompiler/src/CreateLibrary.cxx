//                                               -*- C++ -*-
/**
 *  @brief Implementation of the method AnalyticalFunctionCompiler::CreateLibrary(),
 *         which attempts to compile the library associated to the external code.
 *         the needed external code.
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
#include <fstream>
#include <iomanip>
//#include "config.h"
#include "AnalyticalFunctionCompiler.hxx"

namespace AnalyticalFunctionCompiler
{
/** Generate the binary version of the plugin */
/** Need a cleaner implementation! It should produce an autotooled package for the external code, then use it to produce the external code */
void AnalyticalFunctionCompiler::createLibrary()
{
  std::ostringstream instructionStream;

  instructionStream << "echo \"Compilation step:\" > /tmp/msg1 && echo >> /tmp/msg1 && ";

  std::ostringstream commandStream;
  commandStream << "g++ -I" << OT_INCLUDE_PATH << " " << path_ << name_ << ".cxx -shared -o " << path_ << name_ << ".so -L" << OT_LIB_PATH << "/" << OT_LIB_NAME << " -lOT ";
  std::string command(commandStream.str());
  instructionStream << command << " &> /tmp/msg2";
  std::string compileInstruction(instructionStream.str());
  if ( system(compileInstruction.c_str()) )
  {
    std::ostringstream errorStream;
    errorStream << "echo > /tmp/msg3 && echo \"Formula:\" >> /tmp/msg3 && echo >> /tmp/msg3 && echo \"" << formula_ << "\" >> /tmp/msg3 && echo > /tmp/msg4 && echo \"Source code:\" >> /tmp/msg4 && echo >> /tmp/msg4 && cat /tmp/msg1 /tmp/msg2 /tmp/msg3 /tmp/msg4 " << path_ << name_ << ".cxx > /tmp/msg && rm /tmp/msg1 /tmp/msg2 /tmp/msg3 /tmp/msg4";
    std::string errorMessage(errorStream.str());
    system(errorMessage.c_str());
    system("cat /tmp/msg");
    std::cout << std::endl << "Compilation command was: " << command << std::endl;
    exit(BAD_COMPILATION);
  }
  else
  {
    system("rm /tmp/msg1 /tmp/msg2");
  }
} // AnalyticalFunctionCompiler::createLibrary
} // namespace AnalyticalFunctionCompiler
