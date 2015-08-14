//                                               -*- C++ -*-
/**
 *  @brief Implementation of the class AnalyticalFunctionCompiler, in charge of the
 *         production of an external code compatible with Open TURNS from
 *         a valid C++ expression of an analytical function from R^n to R. Only
 *         the basic methods are implemented here.
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
#include <sstream>
#include <list>

#include "AnalyticalFunctionCompiler.hxx"

namespace AnalyticalFunctionCompiler
{
/** Default constructor */
AnalyticalFunctionCompiler::AnalyticalFunctionCompiler(std::string name, std::string formula, std::string path, std::list<std::string> variates):
  name_(name),
  formula_(formula),
  path_(path),
  variates_(variates)
{
}

/** Function that factorize copy constrution and assignment */
void AnalyticalFunctionCompiler::assignToMe(const AnalyticalFunctionCompiler & other)
{
  name_     = other.getName();
  formula_  = other.getFormula();
  path_     = other.getPath();
  variates_ = other.getVariates();
}

/** Copy constructor */
AnalyticalFunctionCompiler::AnalyticalFunctionCompiler(const AnalyticalFunctionCompiler & other)
{
  assignToMe(other);
}

/** Destructor */
AnalyticalFunctionCompiler::~AnalyticalFunctionCompiler()
{
}

/** Assignment */
AnalyticalFunctionCompiler & AnalyticalFunctionCompiler::operator = (const AnalyticalFunctionCompiler & other)
{
  if (this != &other)   // Other is NOT me, so I can assign it to me
  {
    assignToMe( other );
  }
  return *this;
}

/**
 * String converter
 * This method shows human readable information on the
 * internal state of an object. It is used when streaming
 * the object or for user information.
 */
std::string AnalyticalFunctionCompiler::str()
{
  std::ostringstream oss;
  oss << " path=" << getPath()
      << " name=" << getName()
      << " formula=" << getFormula()
      << " variates=";
  for (std::list<std::string>::iterator iteratorVariates = variates_.begin();
       iteratorVariates != variates_.end();
       iteratorVariates++)
  {
    oss << *iteratorVariates << " ";
  }
  return oss.str();
}
/**
 * Function name accessor
 */
std::string AnalyticalFunctionCompiler::getName() const
{
  return name_;
}

/**
 * Function formula accessor
 */
std::string AnalyticalFunctionCompiler::getFormula() const
{
  return formula_;
}

/**
 * Function path accessor
 */
std::string AnalyticalFunctionCompiler::getPath() const
{
  return path_;
}

/**
 * Function variates accessor
 */
std::list<std::string> AnalyticalFunctionCompiler::getVariates() const
{
  return variates_;
}
} // namespace AnalyticalFunctionCompiler
