//                                               -*- C++ -*-
/**
 *  @brief symbolic expression (base classes and functionality)
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

#include <iomanip>
#include <cmath>

#include "operand.h"

namespace Ev3
{

// constructors
Operand::Operand() :
  oplabel_(CONST),
  dependency_(0),
  constant_(0),
  depconstant_(NULL),
  varindex_(NOVARIABLE),
  varname_(),
  coefficient_(1),
  depcoefficient_(NULL),
  exponent_(1),
  depexponent_(NULL)
{ }

Operand::Operand(double t) :
  oplabel_(CONST),
  dependency_(0),
  constant_(t),
  depconstant_(NULL),
  varindex_(NOVARIABLE),
  varname_(),
  coefficient_(1),
  depcoefficient_(NULL),
  exponent_(1),
  depexponent_(NULL)
{ }

Operand::Operand(const Int t) :
  oplabel_(CONST),
  dependency_(0),
  constant_(t),
  depconstant_(NULL),
  varindex_(NOVARIABLE),
  varname_(),
  coefficient_(1),
  depcoefficient_(NULL),
  exponent_(1),
  depexponent_(NULL)
{ }

Operand::Operand(const Int t,
                 const bool isvar) :
  dependency_(0),
  depconstant_(NULL),
  coefficient_(1),
  depcoefficient_(NULL),
  exponent_(1),
  depexponent_(NULL)
{
  if (isvar)
  {
    // make it a variable
    oplabel_ = VAR;
    constant_ = 0;
    varindex_ = t;
  }
  else
  {
    // make it an operator label
    oplabel_ = static_cast< int > (t);
    constant_ = 0;
    varindex_ = NOVARIABLE;
  }
}

// create an (empty) operator or a variable leaf and set coefficient
Operand::Operand(const double c,
                 const Int t,
                 const std::string & vn) :
  coefficient_(c),
  exponent_(1)
{
  // make it a variable
  oplabel_ = VAR;
  constant_ = 0;
  varindex_ = t;
  varname_ = vn;
  dependency_ = 0;
  depconstant_ = NULL;
  depcoefficient_ = NULL;
  depexponent_ = NULL;
}

// Operand class methods:

std::ostream & operator<< (std::ostream & outbuf, const Operand & operand)
{
  std::string vn;
  if (operand.GetCoeff() == 0)
  {
    // coefficient is 0
    outbuf << 0;
  } // GetCoeff() == 0
  else if (operand.GetOpType() == CONST)
  {
    // constant
    outbuf << operand.GetValue();
  } // GetOpType() == CONST
  else if (operand.GetOpType() == VAR)
  {
    // variable
    if (operand.GetCoeff() == 1)
    {
      int vi = operand.GetVarIndex();
      if (vi == NOVARIABLE)
      {
        if (operand.GetExponent() == 1)
        {
          outbuf << NOTVARNAME;
        } // GetExponent() == 1
        else
        {
          outbuf << NOTVARNAME << "^" << operand.GetExponent();
        } // GetExponent() != 1
      } // vi == NOVARIABLE
      else
      {
        vn = operand.GetVarName();
        if (operand.GetExponent() == 1)
        {
          outbuf << vn;// << VNAMEIDXCHAR << vi;
        } // GetExponent() == 1
        else
        {
          outbuf << vn/* << VNAMEIDXCHAR << vi*/ << "^" << operand.GetExponent();
        } // GetExponent() != 1
      } // vi != NOVARIABLE
    } // GetCoeff() == 1
    else
    {
      int vi = operand.GetVarIndex();
      if (vi == NOVARIABLE)
      {
        if (operand.GetExponent() == 1)
        {
          outbuf  << operand.GetCoeff() << "*_" << NOTVARNAME;
        }
        else
        {
          outbuf  << operand.GetCoeff() << "*" << NOTVARNAME << "^"
                  << operand.GetExponent();
        }
      } // vi == NOVARIABLE
      else
      {
        vn = operand.GetVarName();
        if (operand.GetExponent() == 1)
        {
          outbuf << operand.GetCoeff() << "*" << vn;// << VNAMEIDXCHAR << vi;
        } // GetExponent() == 1
        else
        {
          outbuf << operand.GetCoeff() << "*" << vn/* << VNAMEIDXCHAR << vi*/ << "^"
                 << operand.GetExponent();
        } // GetExponent() != 1
      } // vi != NOVARIABLE
    } // GetCoeff() != 1
  } // GetOpType() == VAR
  else
  {
    // operand, don't print anything
  }
  return outbuf;
}

std::string Operand::ToString() const
{
  std::stringstream outbuf;
  outbuf << *this;
  return outbuf.str();
}

// get operator type
int Operand::GetOpType() const
{
  return oplabel_;
}

// get constant value - in CONSTs, multiply by coeff. and raise
// to exponent, first
double Operand::GetValue() const
{
  if ((oplabel_ == CONST) && (dependency_ == 0))
  {
    if (exponent_ == 1) return coefficient_ * constant_;
    else if (exponent_ == 2) return coefficient_ * constant_ * constant_;
    else return coefficient_ * pow(constant_, exponent_);
  }
  else if ((oplabel_ == CONST) && (dependency_ == 1) && depconstant_) return *depconstant_;
  else return constant_;
}

// just get the value in any case
double Operand::GetSimpleValue() const
{
  if ((dependency_ == 1) && depconstant_) return *depconstant_;
  return constant_;
}

// get variable index
Int Operand::GetVarIndex() const
{
  return varindex_;
}

// get variable name
std::string Operand::GetVarName() const
{
  return varname_;
}

// set operator type
void Operand::SetOpType(const int t)
{
  oplabel_ = t;
}

// set constant value
void Operand::SetValue(const double t)
{
  oplabel_ = CONST;
  constant_ = t;
}

// set variable index
void Operand::SetVarIndex(const Int t)
{
  oplabel_ = VAR;
  varindex_ = t;
}

// set variable name
void Operand::SetVarName(const std::string & vn)
{
  oplabel_ = VAR;
  varname_ = vn;
}

// is operand a constant?
bool Operand::IsConstant() const
{
  return (GetOpType() == CONST);
}

// is operand a variable?
bool Operand::IsVariable() const
{
  return (GetOpType() == VAR);
}

// is operand a leaf node?
bool Operand::IsLeaf() const
{
  return (IsConstant() || IsVariable());
}

void Operand::SetCoeff(const double coeff)
{
  coefficient_ = coeff;
}

double Operand::GetCoeff() const
{
  if ((dependency_ == 2) && depcoefficient_) return *depcoefficient_;
  else return coefficient_;
}

void Operand::SetExponent(const double expon)
{
  exponent_ = expon;
}

double Operand::GetExponent() const
{
  if ((dependency_ == 3) && depexponent_) return *depexponent_;
  else return exponent_;
}

void Operand::SetDependencyOnOperand(const int whichconstant,
                                     double** depvalue)
{
  dependency_ = whichconstant + 1;
  switch(dependency_)
  {
    case 1:
      depconstant_ = *depvalue;
      break;
    case 2:
      depcoefficient_ = *depvalue;
      break;
    case 3:
      depexponent_ = *depvalue;
      break;
  }
}

void Operand::EnforceDependencyOnOperand()
{
  switch(dependency_)
  {
    case 1:
      constant_ = *depconstant_;
      break;
    case 2:
      coefficient_ = *depcoefficient_;
      break;
    case 3:
      exponent_ = *depexponent_;
      break;
  }
}

void Operand::ConsolidateValue()
{
  SetValue(GetValue());
  SetCoeff(1.0);
  SetExponent(1.0);
}

// is operand a zero constant?
bool Operand::IsZero() const
{
  if (GetOpType() == CONST) return (std::abs(GetValue()) < Ev3NearZero());
  return false;
}

// is operand a constant having value v?
bool Operand::HasValue(const double v) const
{
  if (GetOpType() == CONST) return (std::abs(v - GetValue()) < Ev3NearZero());
  return false;
}

// is operand a negative constant?
bool Operand::IsLessThan(const double v) const
{
  if (GetOpType() == CONST) return (GetValue() < v + Ev3NearZero());
  return false;
}

// is operand a negative constant?
bool Operand::IsGreaterThan(const double v) const
{
  if (GetOpType() == CONST) return (GetValue() > v - Ev3NearZero());
  return false;
}


// is operand this == operand t?
bool Operand::operator == (const Operand & t)
{
  if (this == &t) return true;
  // not the same operand - check data fields
  return (GetOpType() == t.GetOpType() &&
          GetValue() == t.GetValue() &&
          GetVarIndex() == t.GetVarIndex());
}

// substitute a variable with a constant
void Operand::SubstituteVariableWithConstant(const int varindex,
    const double c)
{
  if ((GetOpType() == VAR) && (GetVarIndex() == varindex))
  {
    SetOpType(CONST);
    SetVarIndex(NOVARIABLE);
    double t;
    t = GetCoeff() * pow(c, GetExponent());
    SetCoeff(1.0);
    SetExponent(1.0);
    SetValue(t);
  }
}

} /* namespace Ev3 */
