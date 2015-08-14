//                                               -*- C++ -*-
/**
 *  @brief Implementation of the method AnalyticalFunctionCompiler::CreateWrapper(),
 *         which produces an XML file conform to the Open TURNS dtd associated to
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
#include "AnalyticalFunctionCompiler.hxx"

namespace AnalyticalFunctionCompiler
{
/** Generate the source code of the plugin */
void AnalyticalFunctionCompiler::createWrapper()
{
  std::ostringstream oss;
  oss << path_ << name_ << ".xml";
  std::string fileName(oss.str());
  std::ofstream wrapperFile(fileName.c_str(), std::ios::out);

  if(!wrapperFile)
  {
    std::cout << "Error creating " << fileName << std::endl;
    exit(BAD_WRAPPER_CREATION);
  }
  wrapperFile << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << std::endl
              << "<!DOCTYPE wrapper SYSTEM \"wrapper.dtd\">"                   << std::endl
              << "<wrapper>"                                                   << std::endl
              << "  <library>"                                                 << std::endl
              << "    <path>" << path_ << name_ << ".so</path>"                 << std::endl
              << "    <description>"                                           << std::endl
              << "      <data>"                                                << std::endl
              << "      </data>"                                               << std::endl
              << "      <variable-list>"                                       << std::endl;
  for (std::list<std::string>::iterator iteratorVariates = variates_.begin();
       iteratorVariates != variates_.end();
       iteratorVariates++)
  {
    wrapperFile << "        <variable id=\"" << *iteratorVariates << "\" type=\"in\">" << std::endl
                << "          <comment>Automatically generated</comment>"                          << std::endl
                << "          <unit>None</unit>"                                                   << std::endl
                << "          <regexp>None</regexp>"                                               << std::endl
                << "          <format>None</format>"                                               << std::endl
                << "        </variable>"                                                           << std::endl;
  }
  wrapperFile << "        <variable id=\"" << name_ << "\" type=\"out\">" << std::endl
              << "          <comment>Automatically generated</comment>"                          << std::endl
              << "          <unit>None</unit>"                                                   << std::endl
              << "          <regexp>None</regexp>"                                               << std::endl
              << "          <format>None</format>"                                               << std::endl
              << "        </variable>"                                                           << std::endl;
  wrapperFile << "      </variable-list>"                                                << std::endl
              << "      <function>"                                                                  << std::endl
              << "        <symbol>" << name_ << "</symbol>"                                          << std::endl
              << "      </function>"                                                                 << std::endl
              << "    </description>"                                                                << std::endl
              << "  </library>"                                                                      << std::endl
              << " </wrapper>"                                                                       << std::endl;
  wrapperFile.close();
} // AnalyticalFunctionCompiler::createWrapper
} // namespace AnalyticalFunctionCompiler
