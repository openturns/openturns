//                                               -*- C++ -*-
/**
 *  @brief C++ class that builds an Expression v3 n-ary tree from a string containing a mathematical expression in n variables
 *
 *  Copyright (C) 2008-2010 Leo Liberti
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __EV3PARSERH__
#define __EV3PARSERH__

#include <iostream>
#include <map>
#include <cmath>
#include <limits>

#include "expression.h"

namespace Ev3
{
// global declarations

enum Token_value
{
  PEV3NAME, PEV3NUMBER, PEV3END, PEV3PLUS = '+', PEV3NLPLUS = '|',
  PEV3MINUS = '-', PEV3MUL = '*', PEV3DIV = '/', PEV3POWER = '^',
  PEV3PRINT = ';', PEV3ASSIGN = '=', PEV3LP = '(', PEV3RP = ')',
  PEV3UNARYMINUS = '#'
};

// constants
//const double PEV3infinity = std::numeric_limits<double>::infinity();
#define PEV3PI M_PI
#define PEV3E M_E
#define PEV3NOVARIABLE -1
#define PEV3UNKNOWNVAR -2

// the Eval class
class ExpressionParser
{

private:
  // user data
  std::map<std::string, int> variable_;
  std::map<int, std::string> varname_;

  // internal parser/lexer data
  Token_value curr_tok_;
  // contains the map varnames -> varindices
  std::map<std::string, double> table_;
  double number_value_;
  char string_value_[1024];
  int no_of_errors_;
  int no_of_functions_;
  std::istream *input_;
  // if parser hasn't been initialized with SetVariableIDs, false
  bool isinitialized_;

public:

  // constructors & destructors (public methods)
  ExpressionParser();

  // set variable ID
  void SetVariableID(const std::string & vname,
                     const int vid);

  // get variable ID
  int GetVariableID(const std::string & vname);

  // get variable name
  std::string GetVariableName(const int vid);

  // driver evaluating routine (public method)
  Expression Parse(const char* buf,
                   int& nerrors);

private:
  // parser: report error (private method)
  double error(const std::string& s);
  double error(const std::string& s,
               const Token_value tk);

  // parser: primary expressions (private method)
  Expression prim(const bool get);

  // parser: power
  Expression power(const bool get);

  // parser: unary minus
  Expression uminus(const bool get);

  // parser: products and fractions (private method)
  Expression term(const bool get);

  // parser: sums and subtractions (private method)
  Expression expr(const bool get);

  // lexical analyser (private method)
  Token_value get_token();

  // set variable ID (for internal use - doesn't set isinitialized)
  void InternalSetVariableID(const std::string & vname,
                             const int vid);

  // check whether a string is a function name or an unknown variable
  bool IsVariableName(const std::string & vname);

  // progressive vid
  int currentvid_;

}; // end class ExpressionParser

} /* namespace Ev3 */

#endif /* __EV3PARSERH__ */

