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

#include "parser.h"
#include "common.h"

namespace Ev3
{

ExpressionParser::ExpressionParser()
  : input_(0)
{
  isinitialized_ = false;
  currentvid_ = 1;
}

// set variable ID
void ExpressionParser::SetVariableID(const std::string & vname,
                                     const int vid)
{
  isinitialized_ = true;
  variable_[vname] = vid;
  varname_[vid] = vname;
}

// set variable ID for internal use
void ExpressionParser::InternalSetVariableID(const std::string & vname,
    const int vid)
{
  variable_[vname] = vid;
  varname_[vid] = vname;
}

// get variable ID
int ExpressionParser::GetVariableID(const std::string & vname)
{
  if (variable_.find(vname) != variable_.end()) return variable_[vname];
  else return (IsVariableName(vname) ? PEV3UNKNOWNVAR : PEV3NOVARIABLE);
}

// get variable name
std::string ExpressionParser::GetVariableName(const int vid)
{
  if (varname_.find(vid) != varname_.end()) return varname_[vid];
  return NOTVARNAME;
}

bool ExpressionParser::IsVariableName(const std::string & vname)
{
  return  !((vname == "sin")      ||
            (vname == "cos")      ||
            (vname == "tan")      ||
            (vname == "asin")     ||
            (vname == "acos")     ||
            (vname == "atan")     ||
            (vname == "sinh")     ||
            (vname == "cosh")     ||
            (vname == "tanh")     ||
            (vname == "asinh")    ||
            (vname == "acosh")    ||
            (vname == "atanh")    ||
            (vname == "log2")     ||
            (vname == "log10")    ||
            (vname == "log")      ||
            (vname == "ln")       ||
            (vname == "lngamma")  ||
            (vname == "gamma")    ||
            (vname == "exp")      ||
            (vname == "erf")      ||
            (vname == "erfc")     ||
            (vname == "sqrt")     ||
            (vname == "cbrt")     ||
            (vname == "besselJ0") ||
            (vname == "besselJ1") ||
            (vname == "besselY0") ||
            (vname == "besselY1") ||
            (vname == "sign")     ||
            (vname == "rint")     ||
            (vname == "abs"));
}

// driver evaluating routine (public method)
Expression ExpressionParser::Parse(const char* buf,
                                   int & nerrors)
{
  curr_tok_ = PEV3PRINT;
  std::string buf_str(buf);

  // explicit unary minuses at beginning or after a closing parenthesis
  for (size_t i = 0; i < buf_str.size(); ++ i)
    if ((buf_str[i] == PEV3MINUS) && ((i == 0) || ((i > 0) && (buf_str[i - 1] == '('))))
      buf_str[i] = PEV3UNARYMINUS;

  Expression ret;
  input_ = new std::stringstream(buf_str);
  no_of_functions_ = 0;
  no_of_errors_ = 0;

  table_["_pi"] = PEV3PI;
  table_["_e"] = PEV3E;
  table_["pi_"] = PEV3PI;
  table_["e_"] = PEV3E;

  while (*input_)
  {
    get_token();
    switch(curr_tok_)
    {
      case PEV3END:
        break;
      case PEV3PRINT:
        continue;
      case PEV3RP:
        if (no_of_functions_ == 0) error("primary expected, found", curr_tok_);
        else --no_of_functions_;
        continue;
      default:
        ret = expr(false);
    } // switch(curr_tok_)
  } // while (*input)
  delete input_;
  input_ = 0;
  nerrors = no_of_errors_;
  return ret;
}

// parser: report error (private method)
double ExpressionParser::error(const std::string & )
{
  ++no_of_errors_;
  return 0;
}
double ExpressionParser::error(const std::string &,
                               const Token_value )
{
  ++no_of_errors_;
  return 0;
}

// parser: primary expressions (private method)
Expression ExpressionParser::prim(const bool get)
{
  Expression ret;
  if (get) get_token();

  switch (curr_tok_)
  {
    case PEV3NUMBER:
    {
      ret = Expression(number_value_);
      get_token();
    }
    break;
    case PEV3NAME:
    {
      int vid = GetVariableID(string_value_);
      std::string vn = GetVariableName(vid);
      double v = table_[string_value_];
      if (v != 0)
      {
        ret = Expression(v);
        get_token();
      }
      else if (vid == PEV3UNKNOWNVAR && !isinitialized_)
      {
        InternalSetVariableID(string_value_, currentvid_);
        ret->SetOpType(VAR);
        ret->SetVarIndex(currentvid_);
        ret->SetVarName(string_value_);
        ++currentvid_;
        get_token();
      }
      else if (vid != PEV3NOVARIABLE)
      {
        ret->SetOpType(VAR);
        ret->SetVarIndex(vid);
        ret->SetVarName(vn);
        get_token();
      }
      else
      {
        Token_value tk = get_token();
        if (tk == PEV3LP)
        {
          std::string s(string_value_);
          ++no_of_functions_;
          ret = expr(true);
          if (s == "sin") ret = SinLink(ret);
          else if (s == "cos") ret = CosLink(ret);
          else if (s == "tan") ret = TanLink(ret);
          else if (s == "acos") ret = AcosLink(ret);
          else if (s == "asin") ret = AsinLink(ret);
          else if (s == "atan") ret = AtanLink(ret);
          else if (s == "sinh") ret = SinhLink(ret);
          else if (s == "cosh") ret = CoshLink(ret);
          else if (s == "tanh") ret = TanhLink(ret);
          else if (s == "asinh") ret = AsinhLink(ret);
          else if (s == "acosh") ret = AcoshLink(ret);
          else if (s == "atanh") ret = AtanhLink(ret);
          else if (s == "log2") ret = Log2Link(ret);
          else if (s == "log10") ret = Log10Link(ret);
          else if (s == "log") ret = LogLink(ret);
          else if (s == "ln") ret = LogLink(ret);
          else if (s == "lngamma") ret = LngammaLink(ret);
          else if (s == "gamma") ret = GammaLink(ret);
          else if (s == "exp") ret = ExpLink(ret);
          else if (s == "erf") ret = ErfLink(ret);
          else if (s == "erfc") ret = ErfcLink(ret);
          else if (s == "sqrt") ret = SqrtLink(ret);
          else if (s == "cbrt") ret = CbrtLink(ret);
          else if (s == "besselJ0") ret = BesselJ0Link(ret);
          else if (s == "besselJ1") ret = BesselJ1Link(ret);
          else if (s == "besselY0") ret = BesselY0Link(ret);
          else if (s == "besselY1") ret = BesselY1Link(ret);
          else if (s == "sign") ret = SignLink(ret);
          else if (s == "rint") ret = RintLink(ret);
          else if (s == "abs") ret = AbsLink(ret);
          else error("unknown function");
          if (curr_tok_ != PEV3RP) error("bracket ) expected for end-of-function");
          else
          {
            --no_of_functions_;
            get_token();
          }
        } // if (tk == PEV3LP)
      }
    } // case PEV3NAME:
    break;
    case PEV3MINUS:
      ret = MinusLink(prim(true));
      break;
    case PEV3UNARYMINUS:
      ret = MinusLink(ret);
      break;
    case PEV3LP:
    {
      ret = expr(true);
      if (curr_tok_ != PEV3RP) error("bracket ) expected");
      else get_token();
    }
    break;
    default:
      error("primary expected, found", curr_tok_);
  }

  return ret;
}

// parser: power
Expression ExpressionParser::power(const bool get)
{
  Expression ret = prim(get);
  for (;;)
  {
    switch (curr_tok_)
    {
      case PEV3POWER:
        ret = PowerLink(ret, prim(true));
        break;
      default:
        return ret;
    }
  }
}

Expression ExpressionParser::uminus(const bool get)
{
  Expression ret = power(get);
  for (;;)
  {
    switch (curr_tok_)
    {
      case PEV3UNARYMINUS:
        ret = DifferenceLink(ret, power(true));
        break;
      default:
        return ret;
    }
  }
}

// parser: products and fractions (private method)
Expression ExpressionParser::term(const bool get)
{
  Expression ret = uminus(get);

  for (;;)
  {
    switch (curr_tok_)
    {
      case PEV3MUL:
        ret = ProductLink(ret, uminus(true));
        break;
      case PEV3DIV:
        ret = FractionLink(ret, uminus(true));
        break;
      default:
        return ret;
    }
  }
}

// parser: sums and subtractions (private method)
Expression ExpressionParser::expr(const bool get)
{
  Expression ret = term(get);

  for (;;)
  {
    switch (curr_tok_)
    {
      case PEV3PLUS:
      case PEV3NLPLUS:
        ret = SumLink(ret, term(true));
        break;
      case PEV3MINUS:
        ret = DifferenceLink(ret, term(true));
        break;
      default:
        return ret;
    }
  }
}

// lexical analyser (private method)
Token_value ExpressionParser::get_token()
{
  char ch;
  do
  {
    // skip whitespace except '\n'
    if (!input_->get(ch))
      return curr_tok_ = PEV3END;
  }
  while (ch != '\n' && isspace(ch));
  switch(ch)
  {
    case ';':
    case '\n':
      return curr_tok_ = PEV3PRINT;
    case 0:
      return curr_tok_ = PEV3END;
    case '*':
    case '/':
    case '+':
    case '-':
    case '|':
    case '(':
    case ')':
    case '=':
    case '^':
    case '#':
      return curr_tok_ = Token_value(ch);
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
      input_->putback(ch);
      *input_ >> number_value_;
      return curr_tok_ = PEV3NUMBER;
    default:  // PEV3NAME, PEV3NAME=, or error
      if (isalpha(ch) || (ch == '_'))
      {
        char* sv = string_value_;
        *sv = ch;
        while (input_->get(ch) && (isalnum(ch) || (ch == '_')))
        {
          ++sv;
          *sv = ch;
        }
        ++sv;
        *sv = '\0';
        input_->putback(ch);
        return curr_tok_ = PEV3NAME;
      }
      error("bad token");
      return curr_tok_ = PEV3PRINT;
  }
}

} /* namespace Ev3 */
