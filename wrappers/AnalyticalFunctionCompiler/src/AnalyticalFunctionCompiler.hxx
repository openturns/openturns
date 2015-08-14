//                                               -*- C++ -*-
/**
 *  @brief Header for the class AnalyticalFunctionCompiler, in charge of the
 *         production of an external code compatible with Open TURNS from
 *         a valid C++ expression of an analytical function from R^n to R.
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
 *  Id      AnalyticalFunctionCompiler.hxx 2392 2012-02-17 18:35:43Z schueller
 */
#ifndef ANALYTICAL_FUNCTION_COMPILER
#define ANALYTICAL_FUNCTION_COMPILER

#include <string>
#include <list>
#include <cstdlib>

namespace AnalyticalFunctionCompiler
{
enum EXIT_CODE {EXIT_OK, BAD_ARGUMENTS, BAD_COMPILATION, BAD_SOURCE_CREATION, BAD_WRAPPER_CREATION};

class AnalyticalFunctionCompiler
{
public:

  /** Default constructor */
  AnalyticalFunctionCompiler(std::string name, std::string formula, std::string path, std::list<std::string> variates);

  /** Copy constructor */
  AnalyticalFunctionCompiler(const AnalyticalFunctionCompiler & other);

  /** Destructor */
  virtual ~AnalyticalFunctionCompiler();

  /** Assignment */
  AnalyticalFunctionCompiler & operator =(const AnalyticalFunctionCompiler & other);

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an object. It is used when streaming
   * the object or for user information.
   */
  std::string str();

  /**
   * Function name accessor
   */
  std::string getName() const;

  /**
   * Function formula accessor
   */
  std::string getFormula() const;

  /**
   * Function path accessor
   */
  std::string getPath() const;

  /**
   * Function variates accessor
   */
  std::list<std::string> getVariates() const;

  /**
   * Generate a C++ source code corresponding to the given formula and compatible with OpenTURNS wrappers
   */
  void createSourceCode();//throw (AnalyticalFunctionCompiler::CreateSourceCode);

  /**
   * Compile the library corresponding to the function
   */
  void createLibrary();// throw (AnalyticalFunctionCompiler::CompileLibrary);

  /**
   * Generate an xml wrapper compatible with OpenTURNS wrappers
   */
  void createWrapper();// throw (AnalyticalFunctionWrapper::CreateWrapper);
private:
  /** Virtual constructor */
  void assignToMe(const AnalyticalFunctionCompiler & other);

  std::string name_;
  std::string formula_;
  std::string path_;
  std::list<std::string> variates_;
}; // class AnalyticalFunctionCompiler
} // namespace AnalyticalFunctionCompiler

#endif
