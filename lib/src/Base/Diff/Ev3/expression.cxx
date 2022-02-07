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

#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <iomanip>

#include "expression.h"

namespace Ev3
{

////////////// auxiliary functions ///////////////

//#define VNAMEIDXCHAR "_"
#define VNAMEIDXCHAR ""

#define ISINTTOLERANCE 1e-8

static bool is_integer(double a)
{
  double b = std::abs(a);
  int bi = (int) round(b);
  return (std::abs(b - bi) < ISINTTOLERANCE);
}

static bool is_even(double a)
{
  if (is_integer(a))
  {
    int ai = (int) round(a);
    return (ai % 2 == 0);
  }
  return false;
}

static bool is_odd(double a)
{
  if (is_integer(a))
  {
    int ai = (int) round(a);
    return (ai % 2 == 1);
  }
  return false;
}

double Ev3NearZero()
{
  // returns a very small positive value
  return 1 / LARGE;
}

double Ev3Infinity()
{
  // returns a very large positive value
  return LARGE;
}

// create empty
BasicExpression::BasicExpression() { }

// create a constant leaf
BasicExpression::BasicExpression(const double t) : Operand(t) { }

// create a constant (integer-valued) leaf
BasicExpression::BasicExpression(const Int t) : Operand(t) { }

// create an (empty) operator or a variable leaf
BasicExpression::BasicExpression(const Int t,
                                 const bool isvar) :
  Operand(t, isvar) { }

// create a variable leaf and set coefficient
BasicExpression::BasicExpression(const double c,
                                 const Int t,
                                 const std::string & vn) :
  Operand(c, t, vn) { }

// user-defined copy constructor with two options
BasicExpression::BasicExpression(const Expression & t,
                                 const bool iscopy) :
  Operand(t.GetPointee())
{
  const Int s = t->GetSize();
  // create a _copy_ of t, subnode by subnode
  if (iscopy) for (int i = 0; i < s; i++) nodes.push_back(t->GetCopyOfNode(i));
  // create a copy of the pointers in t
  else for (int i = 0; i < s; i++) nodes.push_back(t->GetNode(i));
}

// copy constructor
BasicExpression::BasicExpression(const BasicExpression & t) : Operand(t)
{
  const Int s = t.GetSize();
  // necessary for constructs "(BasicExpression) e =" where e already
  // existed prior to assignment
  // -- segvs (?)
  for(int i = 0; i < GetSize(); i++) RecursiveDestroy(GetNodePtr(i));
  DeleteAllNodes();
  // create a copy of the subnode pointers in t
  for (int i = 0; i < s; i++) nodes.push_back(t.GetNode(i));
}

// destructor
BasicExpression::~BasicExpression() { }

// BasicExpression class methods:


void BasicExpression::Debug () const
{
  Int s = GetSize();
  std::cerr << "BasicExpression: Debug:\n";
  std::cerr << "\tthis   = " << this << std::endl;
  std::cerr << "\toptype = " << GetOpType() << std::endl;
  std::cerr << "\tnodes  = " << s << std::endl;
  for (Int i = 0; i < s; i++) std::cerr << "\tnode " << i << ": " << GetNode(i)->GetOpType() << std::endl;
}

void BasicExpression::Zero()
{
  // -- segvs (?)
  for(int i = 0; i < GetSize(); i++) RecursiveDestroy(GetNodePtr(i));
  DeleteAllNodes();
  SetCoeff(1.0);
  SetExponent(1.0);
  SetValue(0.0);
  SetOpType(CONST);
}

void BasicExpression::One()
{
  // -- segvs (?)
  for(int i = 0; i < GetSize(); i++) RecursiveDestroy(GetNodePtr(i));
  DeleteAllNodes();
  SetCoeff(1.0);
  SetExponent(1.0);
  SetValue(1.0);
  SetOpType(CONST);
}

std::string BasicExpression::PrintTree(const int blanks,
                                       const int tabs) const
{
  std::stringstream outbuf;
  std::string b(blanks, ' ');
  if (IsLeaf()) outbuf << b << Operand::ToString();
  else
  {
    outbuf << b << "OP[" << GetOpType() << "](" << std::endl;
    for(int i = 0; i < GetSize(); i++)
    {
      outbuf << GetNode(i)->PrintTree(blanks + tabs, tabs);
      if (i < GetSize() - 1)
      {
        outbuf << ",";
      }
      outbuf << std::endl;
    }
    outbuf << b << ")";
  }
  return outbuf.str();
}


std::ostream & operator<< (std::ostream & outbuf, const BasicExpression & expr)
{
  Int i, s;
  if (expr.IsLeaf())
  {
    // leaf node, use Operand::ToString()
    outbuf << Operand(expr);

  }
  else
  {
    double tc( expr.GetCoeff());
    // operator node
    if (tc != 1)
    {
      if (tc != -1) outbuf << "(" << tc << "*(";
      else outbuf << "(-(";
    }
    s =  expr.GetSize();
    if (s > 1)
    {
      for (i = 0; i < s; i++)
      {
        outbuf << "(" << expr.GetNode(i) << ")";
        if (i < s - 1)
        {
          switch( expr.GetOpType())
          {
            case SUM:
              outbuf << "+";
              break;
            case DIFFERENCE:
              outbuf << "-";
              break;
            case PRODUCT:
              outbuf << "*";
              break;
            case FRACTION:
              outbuf << "/";
              break;
            case POWER:
              outbuf << "^";
              break;
            default:
              outbuf << "UNKNOWNOP";
              break;
          }
        }
      }
    }
    else
    {
      switch( expr.GetOpType())
      {
        case PLUS:
          break;
        case MINUS:
          outbuf << "-";
          break;
        case SIN:
          outbuf << "sin";
          break;
        case COS:
          outbuf << "cos";
          break;
        case TAN:
          outbuf << "tan";
          break;
        case ASIN:
          outbuf << "asin";
          break;
        case ACOS:
          outbuf << "acos";
          break;
        case ATAN:
          outbuf << "atan";
          break;
        case SINH:
          outbuf << "sinh";
          break;
        case COSH:
          outbuf << "cosh";
          break;
        case TANH:
          outbuf << "tanh";
          break;
        case ASINH:
          outbuf << "asinh";
          break;
        case ACOSH:
          outbuf << "acosh";
          break;
        case ATANH:
          outbuf << "atanh";
          break;
        case LOG2:
          outbuf << "log2";
          break;
        case LOG10:
          outbuf << "log10";
          break;
        case LOG:
          outbuf << "log";
          break;
        case LN:
          outbuf << "ln";
          break;
        case LNGAMMA:
          outbuf << "lngamma";
          break;
        case GAMMA:
          outbuf << "gamma";
          break;
        case EXP:
          outbuf << "exp";
          break;
        case ERF:
          outbuf << "erf";
          break;
        case ERFC:
          outbuf << "erfc";
          break;
        case SQRT:
          outbuf << "sqrt";
          break;
        case CBRT:
          outbuf << "cbrt";
          break;
        case BESSELJ0:
          outbuf << "besselJ0";
          break;
        case BESSELJ1:
          outbuf << "besselJ1";
          break;
        case BESSELY0:
          outbuf << "besselY0";
          break;
        case BESSELY1:
          outbuf << "besselY1";
          break;
        case SIGN:
          outbuf << "sign";
          break;
        case RINT:
          outbuf << "rint";
          break;
        case ABS:
          outbuf << "abs";
          break;
        case COT:
          outbuf << "cot";
          break;
        case COTH:
          outbuf << "coth";
          break;
        default:
          outbuf << "UNKNOWNOP";
      }
      if (s == 1)
      {
        outbuf << "(" << expr.GetNode(0) << ")";
      }
      else
      {
        // no arguments - error
        outbuf << "(NOARG)";
      }
    }
    if ( expr.GetCoeff() != 1)
    {
      outbuf << "))";
    }
  }
  return outbuf;
}


std::string BasicExpression::ToString() const
{
  std::stringstream outbuf;
  outbuf << *this;
  return outbuf.str();
}

std::ostream & operator<< (std::ostream & outbuf, const Expression & expr)
{
  outbuf << expr.GetPointee();
  return outbuf;
}

// is expression this == expression t?
bool BasicExpression::IsEqualTo(const Expression& t) const
{
  //Just in case constant value not consolidated in coeff
  if (GetOpType() == CONST && t->GetOpType() == CONST)
  {
    if (GetValue() == t->GetValue())
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  if (IsEqualToNoCoeff(t))
  {
    if (GetCoeff() == t->GetCoeff())
      return true;
    else
      return false;
  }
  else
    return false;
}

bool BasicExpression::operator == (const BasicExpression& t) const
{
  // fast checks
  if (IsLeaf() && t.IsLeaf())
  {
    if (GetOpType() == t.GetOpType())
    {
      if (GetOpType() == CONST)
      {
        // if both are constants, they're always equal up to coefficient
        return true;
      }
      else if (GetOpType() == VAR && GetVarIndex() == t.GetVarIndex() &&
               GetExponent() == t.GetExponent())
        return true;
      else
        return false;
    }
    else
      return false;
  }
  else if ((!IsLeaf()) && (!t.IsLeaf()))
  {
    // both BasicExpressions are not leaves, recurse using PRECISE
    // (i.e. not up to coefficient) form
    if (GetSize() != t.GetSize())
      return false;
    if (GetOpType() != t.GetOpType())
      return false;
    Int i;
    bool ret = true;
    for (i = 0; i < GetSize(); i++)
    {
      if (!(GetNode(i)->IsEqualTo(t.GetNode(i))))
      {
        ret = false;
        break;
      }
    }
    if (ret)
    {
      return true;
    }
    return false;
  }
  else
    return false;
}

bool BasicExpression::IsEqualTo(const double t) const
{
  return (IsLeaf() && GetOpType() == CONST && GetValue() == t);
}

int BasicExpression::NumberOfVariables() const
{
  int maxvi = 0;
  return NumberOfVariables(maxvi);
}

int BasicExpression::NumberOfVariables(int& maxvi) const
{
  int newvi = 0;
  if (IsVariable())
  {
    newvi = GetVarIndex();
    if (newvi > maxvi)
    {
      maxvi = newvi;
    }
    return maxvi;
  }
  else if (!IsLeaf())
  {
    for(int i = 0; i < GetSize(); i++)
    {
      newvi = GetNode(i)->NumberOfVariables(maxvi);
      if (newvi > maxvi)
      {
        maxvi = newvi;
      }
    }
    return maxvi;
  }
  else
  {
    return 0;
  }
}


// taken and adapted from operator ==
bool BasicExpression::IsEqualToNoCoeff(const Expression & t) const
{
  // fast checks
  if (IsLeaf() && t->IsLeaf())
  {
    if (GetOpType() == t->GetOpType())
    {
      if (GetOpType() == CONST)
      {
        // if both are constants, they're always equal up to coefficient
        return true;
      }
      else
      {
        if (GetOpType() == VAR && GetVarIndex() == t->GetVarIndex() &&
            GetExponent() == t->GetExponent())
          return true;
        else
          return false;
      }
    }
    else return false;
  }
  else if ((!IsLeaf()) && (!t->IsLeaf()))
  {
    // both BasicExpressions are not leaves, recurse using PRECISE
    // (i.e. not up to coefficient) form
    if (GetSize() != t->GetSize() || GetOpType() != t->GetOpType() ||
        GetExponent() != t->GetExponent())
    {
      return false;
    }
    Int i;
    bool ret = true;
    for (i = 0; i < GetSize(); i++)
    {
      if (!(GetNode(i)->IsEqualTo(t->GetNode(i))))
      {
        ret = false;
        break;
      }
    }
    if (ret)
    {
      return true;
    }
    return false;
  }
  else
    return false;
}

bool BasicExpression::IsEqualBySchema(const Expression& t) const
{
  if (IsLeaf() && t->IsLeaf())
  {
    if (GetOpType() == t->GetOpType() &&
        IsLinear() == t->IsLinear())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if ((!IsLeaf()) && (!t->IsLeaf()))
  {
    // both BasicExpressions are not leaves, recurse
    if (GetSize() != t->GetSize())
    {
      return false;
    }
    if (GetOpType() != t->GetOpType())
    {
      return false;
    }
    Int i;
    bool ret = true;
    for (i = 0; i < GetSize(); i++)
    {
      if (!(GetNode(i)->IsEqualBySchema(t->GetNode(i))))
      {
        ret = false;
        break;
      }
    }
    if (ret)
    {
      return true;
    }
    return false;
  }
  else
  {
    return false;
  }
}

bool BasicExpression::IsEqualByOperator(const int theoplabel) const
{
  if (GetOpType() == theoplabel)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool BasicExpression::DependsOnVariable(Int vi) const
{
  if (IsLeaf())
  {
    if (GetOpType() == VAR)
    {
      if (GetVarIndex() == vi)
        return true;
      else
        return false;
    }
    else
      return false;
  }
  else
  {
    Int i;
    bool ret = false;
    for (i = 0; i < GetSize(); i++)
    {
      ret = GetNode(i)->DependsOnVariable(vi);
      if (ret)
        return true;
    }
    return false;
  }
}

int BasicExpression::DependsLinearlyOnVariable(Int vi) const
{
  if (IsVariable())
  {
    if (GetVarIndex() == vi)
    {
      if (GetExponent() == 1)
      {
        return 1; // depends linearly
      }
      else
      {
        return 0; // depends nonlinearly
      }
    }
    else
    {
      return 2; // doesn't depend on vi at all
    }
  }
  else
  {
    int i;
    int d;
    bool dependsatall = false;
    // if node is linear:
    if (GetOpType() == SUM || GetOpType() == DIFFERENCE ||
        GetOpType() == PLUS || GetOpType() == MINUS)
    {
      for(i = 0; i < GetSize(); i++)
      {
        d = GetNode(i)->DependsLinearlyOnVariable(vi);
        if (d == 0)
        {
          // depends nonlinearly, return 0
          return 0;
        }
        if (d == 1)
        {
          // depends linearly, record
          dependsatall = true;
        }
      }
      if (dependsatall)
      {
        return 1;
      }
      else
      {
        return 2;
      }
    }
    else if (GetOpType() == PRODUCT)
    {
      int nbBranchesDependingOnVariable = 0;
      for(i = 0; i < GetSize(); i++)
      {
        d = GetNode(i)->DependsLinearlyOnVariable(vi);
        if (d == 0)
          return 0;

        if (d == 1)
          ++nbBranchesDependingOnVariable;
      }

      if (nbBranchesDependingOnVariable == 0)
        return 2;
      else if (nbBranchesDependingOnVariable == 1)
        return 1;
      else
        return 0;
    }
    else
    {
      if (DependsOnVariable(vi))
      {
        return 0;  // depends nonlinearly
      }
      else
      {
        return 2;  // doesn't depend on vi at all
      }
    }
  }
}

void BasicExpression::ConsolidateProductCoeffs()
{
  if (GetOpType() == PRODUCT)
  {
    Int i;
    double tc = GetCoeff();
    for (i = 0; i < GetSize(); i++)
    {
      tc *= GetNode(i)->GetCoeff();
      GetNode(i)->SetCoeff(1.0);
    }
    if (std::abs(tc) < Ev3NearZero())
    {
      Zero();
    }
    else
    {
      SetCoeff(tc);
    }
  }
}

void BasicExpression::DistributeCoeffOverSum()
{
  if (GetOpType() == SUM)
  {
    double tc = GetCoeff();
    if (tc != 1)
    {
      SetCoeff(1.0);
      Int i;
      for(i = 0; i < GetSize(); i++)
      {
        GetNode(i)->SetCoeff(tc * GetNode(i)->GetCoeff());
        GetNode(i)->DistributeCoeffOverSum();
      }
    }
  }
}

void BasicExpression::DistributeCoeffOverProduct()
{
  if (GetOpType() == PRODUCT)
  {
    double tc = GetCoeff();
    if (tc != 1 && GetSize() > 0)
    {
      SetCoeff(1.0);
      GetNode(0)->SetCoeff(tc * GetNode(0)->GetCoeff());
    }
  }
}

// enforce constant dependencies (added for MORON - see ../PROGNOTES)
// this only acts on the proper leaf nodes
void BasicExpression::EnforceDependency()
{
  if (IsLeaf())
  {
    // nonrecursive
    EnforceDependencyOnOperand();
  }
  else
  {
    // recursive
    int i;
    for (i = 0; i < GetSize(); i++)
    {
      GetNode(i)->EnforceDependency();
    }
  }
}

// substitute a variable with a constant
void BasicExpression::VariableToConstant(int varindex,
    double c)
{
  if (IsLeaf())
  {
    // nonrecursive
    SubstituteVariableWithConstant(varindex, c);
  }
  else
  {
    // recursive
    int i;
    for (i = 0; i < GetSize(); i++)
    {
      GetNode(i)->VariableToConstant(varindex, c);
    }
  }
}

// replace variable indexed v1 with variable indexed v2
void BasicExpression::ReplaceVariable(int v1,
                                      int v2,
                                      const std::string & vn)
{
  if (DependsOnVariable(v1))
  {
    if (IsVariable() && GetVarIndex() == v1)
    {
      SetVarIndex(v2);
      SetVarName(vn);
    }
    else
    {
      int i;
      for(i = 0; i < GetSize(); i++)
      {
        GetNode(i)->ReplaceVariable(v1, v2, vn);
      }
    }
  }
}
void BasicExpression::ReplaceVariable(int v1,
                                      int v2,
                                      const std::string & vn,
                                      double c2)
{
  if (DependsOnVariable(v1))
  {
    if (IsVariable() && GetVarIndex() == v1)
    {
      SetVarIndex(v2);
      SetVarName(vn);
      SetCoeff(GetCoeff() * c2);
    }
    else
    {
      int i;
      for(i = 0; i < GetSize(); i++)
      {
        GetNode(i)->ReplaceVariable(v1, v2, vn, c2);
      }
    }
  }
}

// replace with a variable the deepest node conforming to schema and
// return replaced term or zero expression if no replacement occurs
Expression BasicExpression::ReplaceBySchema(int vi,
    const std::string & vn,
    const Expression & schema)
{
  Expression ret(0.0);
  ret = ReplaceBySchemaRecursive(vi, vn, schema);
  if (ret->IsZero())
  {
    // no subnodes with schema found, wor on this one
    if (IsEqualBySchema(schema))
    {
      // this node is according to schema
      // save (recursively) this into ret
      ret.SetToCopyOf(*this);
      // replace with w_vi
      // -- segvs (?)
      for(int i = 0; i < GetSize(); i++)
      {
        RecursiveDestroy(GetNodePtr(i));
      }
      DeleteAllNodes();
      SetOpType(VAR);
      SetVarIndex(vi);
      SetVarName(vn);
      SetCoeff(1.0);
      SetExponent(1.0);
    }
  }
  return ret;
}

// recursive version - works on subnodes, not on current node
Expression BasicExpression::ReplaceBySchemaRecursive(int vi,
    const std::string & vn,
    const Expression & schema)
{
  bool done = false;
  Expression ret(0.0);
  for(int i = 0; i < GetSize(); i++)
  {
    if (!GetNode(i)->IsLeaf())
    {
      ret = GetNode(i)->ReplaceBySchemaRecursive(vi, vn, schema);
      if (!ret->IsZero())
      {
        done = true;
        break;
      }
    }
    if (!done)
    {
      if (GetNode(i)->IsEqualBySchema(schema))
      {
        ret = GetNode(i);
        Expression w(1, vi, vn);
        GetNodePtr(i)->SetTo(w);
        done = true;
        break;
      }
    }
  }
  return ret;
}

// replace with a variable the deepest node with given operator label
// return replaced term or zero expression if no replacement occurs
Expression BasicExpression::ReplaceByOperator(int vi,
    const std::string & vn,
    int theoplabel)
{
  Expression ret(0.0);
  ret = ReplaceByOperatorRecursive(vi, vn, theoplabel);
  if (ret->IsZero())
  {
    // no subnodes with schema found, wor on this one
    if (IsEqualByOperator(theoplabel))
    {
      // this node is according to schema
      // save (recursively) this into ret
      ret.SetToCopyOf(*this);
      // replace with w_vi
      // -- segvs
      for(int i = 0; i < GetSize(); i++)
      {
        RecursiveDestroy(GetNodePtr(i));
      }
      DeleteAllNodes();
      SetOpType(VAR);
      SetVarIndex(vi);
      SetVarName(vn);
      SetCoeff(1.0);
      SetExponent(1.0);
    }
  }
  return ret;
}

// recursive version - works on subnodes, not on current node
Expression BasicExpression::ReplaceByOperatorRecursive(int vi,
    const std::string & vn,
    int theoplabel)
{
  bool done = false;
  Expression ret(0.0);
  for(int i = 0; i < GetSize(); i++)
  {
    if (!GetNode(i)->IsLeaf())
    {
      ret = GetNode(i)->ReplaceByOperatorRecursive(vi, vn, theoplabel);
      if (!ret->IsZero())
      {
        done = true;
        break;
      }
    }
    if (!done)
    {
      if (GetNode(i)->IsEqualByOperator(theoplabel))
      {
        ret = GetNode(i);
        Expression w(1, vi, vn);
        GetNodePtr(i)->SetTo(w);
        done = true;
        break;
      }
    }
  }

  return ret;
}

void BasicExpression::ReplaceWithExpression(const Expression & replace)
{
  /* // -- segvs (?)
     for(int i = 0; i < GetSize(); i++) {
     RecursiveDestroy(GetNodePtr(i));
     }
  */
  DeleteAllNodes();
  if (replace->GetOpType() == VAR)
  {
    SetVarIndex(replace->GetVarIndex());
    SetVarName(replace->GetVarName());
    SetCoeff(replace->GetCoeff());
    SetExponent(replace->GetExponent());
  }
  else if (replace->GetOpType() == CONST)
  {
    SetValue(replace->GetValue());
  }
  else
  {
    SetCoeff(replace->GetCoeff());
    SetExponent(replace->GetExponent());
    SetOpType(replace->GetOpType());
  }
  for (int i = 0; i < replace->GetSize(); i++)
  {
    nodes.push_back(replace->GetNode(i));
  }
}


int BasicExpression::ReplaceSubexpression(const Expression & needle,
    const Expression & replace)
{
  int ret = 0;
  if (!IsLeaf())
  {
    // recurse
    for(int i = 0; i < GetSize(); i++)
    {
      ret += GetNode(i)->ReplaceSubexpression(needle, replace);
    }
  }
  // act on this node
  if (IsEqualTo(needle))
  {
    ret++;
    ReplaceWithExpression(replace);
  }
  return ret;
}

void BasicExpression::ResetVarNames(const std::string & vn,
                                    int lid,
                                    int uid)
{
  // set all variable names in the expression to vn
  if (!IsLeaf())
  {
    for(int i = 0; i < GetSize(); i++)
    {
      GetNode(i)->ResetVarNames(vn, lid, uid);
    }
  }
  else
  {
    if (GetOpType() == VAR)
    {
      int vi = GetVarIndex();
      if (vi >= lid && vi <= uid)
      {
        SetVarName(vn);
      }
    }
  }
}


bool BasicExpression::DistributeProductsOverSums()
{
  // recursive part
  bool ret = false;
  if (!IsLeaf())
  {
    for(int i = 0; i < GetSize(); i++)
    {
      bool haschanged = GetNode(i)->DistributeProductsOverSums();
      if (haschanged)
      {
        ret = true;
      }
    }
  }
  // deal with this node
  Expression e(0.0);
  if (GetOpType() == PRODUCT)
  {
    for(int i = 0; i < GetSize(); i++)
    {
      if (GetNode(i)->GetOpType() == SUM)
      {
        // found occurrence of *(+), distribute
        ret = true;
        Expression f = (*this) / GetNode(i);
        Simplify(&f);
        for(int j = 0; j < GetNode(i)->GetSize(); j++)
        {
          e = e + f * GetNode(i)->GetNode(j);
        }
        // now replace this with e
        ReplaceWithExpression(e);
      }
    }
  }
  return ret;
}

void BasicExpression::GetVarIndices(std::vector<int> & vidx)
{
  if (!IsLeaf())
  {
    for(int i = 0; i < GetSize(); i++)
    {
      GetNode(i)->GetVarIndices(vidx);
    }
  }
  else if (IsVariable())
  {
    int vi = GetVarIndex();
    if (find(vidx.begin(), vidx.end(), vi) == vidx.end())
    {
      vidx.push_back(vi);
    }
  }
}

void BasicExpression::GetVarIndicesInSchema(std::vector<int> & vidx,
    const Expression & schema)
{
  // recurse
  if (!IsLeaf())
  {
    for(int i = 0; i < GetSize(); i++)
    {
      GetNode(i)->GetVarIndicesInSchema(vidx, schema);
    }
  }
  // deal with this node
  if (IsEqualBySchema(schema))
  {
    GetVarIndices(vidx);
  }
}

// find the variable name corresponding to variable index vi
std::string BasicExpression::FindVariableName(int vi)
{
  std::string vn;
  if (IsVariable())
  {
    if (GetVarIndex() == vi)
    {
      return GetVarName();
    }
    else
    {
      return "";
    }
  }
  else
  {
    int i;
    for(i = 0; i < GetSize(); i++)
    {
      vn = GetNode(i)->FindVariableName(vi);
      if (vn.length() > 0)
      {
        return vn;
      }
    }
  }
  return "";
}

// is this expression linear?
bool BasicExpression::IsLinear() const
{
  if (IsVariable())
  {
    if (GetExponent() != 0 && GetExponent() != 1)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  if (IsConstant())
  {
    return true;
  }
  if(GetOpType() == SUM || GetOpType() == DIFFERENCE)
  {
    int i;
    for(i = 0; i < GetSize(); i++)
    {
      if (!GetNode(i)->IsLinear())
      {
        return false;
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}

// is this expression a quadratic product?
bool BasicExpression::IsQuadratic(int& prodtype) const
{
  bool ret = false;
  if ((GetOpType() == PRODUCT &&
       GetNode(0)->GetOpType() == VAR && GetNode(1)->GetOpType() == VAR) ||
      (GetOpType() == POWER &&
       GetNode(0)->GetOpType() == VAR && GetNode(1)->GetValue() == 2) ||
      (GetOpType() == VAR && GetExponent() == 2))
  {
    prodtype = GetOpType();
    ret = true;
  }
  return ret;
}
bool BasicExpression::IsQuadratic() const
{
  int ret = 0;
  return IsQuadratic(ret);
}

// return info about the linear part (assumes Simplify() has already been
// called on this) - return false if expression has no linear part
// by "linear part" we mean lin(x) in expr(x,y) = lin(x) + nonlin(y)
bool BasicExpression::GetLinearInfo(std::vector<double> & lincoeff,
                                    std::vector<int> & linvi,
                                    std::vector<std::string> & linvn,
                                    double& c)
{
  c = 0;
  bool ret = false;
  Expression nl(GetPureNonlinearPart());
  if (IsLinear())
  {
    nl->Zero();
  }
  if (lincoeff.size() > 0)
  {
    lincoeff.erase(lincoeff.begin(), lincoeff.end());
    linvi.erase(linvi.begin(), linvi.end());
    linvn.erase(linvn.begin(), linvn.end());
  }
  if (IsLeaf())
  {
    if (IsConstant())
    {
      // just a constant
      c = GetValue();
      ret = true;
    }
    else if (IsVariable() && GetExponent() == 1)
    {
      // just a variable
      linvi.push_back(GetVarIndex());
      lincoeff.push_back(GetCoeff());
      linvn.push_back(GetVarName());
      ret = true;
    }
  }
  else
  {
    if (GetOpType() == SUM)
    {
      int i, vi;
      c = 0;
      for(i = 0; i < GetSize(); i++)
      {
        if (GetNode(i)->IsConstant())
        {
          if (i > 0)
          {
            //std::cerr << "BasicExpression::GetLinearInfo: WARNING: "
            //          << "run Simplify() first\n";
          }
          c += GetNode(i)->GetValue();
        }
        else if (GetNode(i)->IsVariable() &&
                 GetNode(i)->GetExponent() == 1)
        {
          vi = GetNode(i)->GetVarIndex();
          if (!nl->DependsOnVariable(vi))
          {
            // vi depends only linearly on expression
            linvi.push_back(vi);
            lincoeff.push_back(GetNode(i)->GetCoeff());
            linvn.push_back(GetNode(i)->GetVarName());
            ret = true;
          }
        }
      }
    }
  }
  return ret;
}

// return info about the purely linear part
// (assumes Simplify() has already been
// called on this) - return false if expression has no linear part
// by "pure linear part" we mean e.g. x+y in x+y+y^2
bool BasicExpression::GetPureLinearInfo(std::vector<double> & lincoeff,
                                        std::vector<int> & linvi,
                                        std::vector<std::string> & linvn,
                                        double& c)
{
  c = 0;
  bool ret = false;
  int i;
  Expression nl(GetPureNonlinearPart());
  if (IsLinear())
  {
    nl->Zero();
  }
  if (lincoeff.size() > 0)
  {
    lincoeff.erase(lincoeff.begin(), lincoeff.end());
    linvi.erase(linvi.begin(), linvi.end());
    linvn.erase(linvn.begin(), linvn.end());
  }
  if (IsLeaf())
  {
    if (IsConstant())
    {
      // just a constant
      c = GetValue();
      ret = true;
    }
    else if (IsVariable() && GetExponent() == 1)
    {
      // just a variable
      linvi.push_back(GetVarIndex());
      lincoeff.push_back(GetCoeff());
      linvn.push_back(GetVarName());
      ret = true;
    }
  }
  else
  {
    if (GetOpType() == SUM)
    {
      int vi;
      c = 0;
      for(i = 0; i < GetSize(); i++)
      {
        if (GetNode(i)->IsConstant())
        {
          if (i > 0)
          {
            //std::cerr << "BasicExpression::GetLinearInfo: WARNING: "
            //          << "run Simplify() first\n";
          }
          c += GetNode(i)->GetValue();
        }
        else if (GetNode(i)->IsVariable() &&
                 GetNode(i)->GetExponent() == 1)
        {
          vi = GetNode(i)->GetVarIndex();
          linvi.push_back(vi);
          lincoeff.push_back(GetNode(i)->GetCoeff());
          linvn.push_back(GetNode(i)->GetVarName());
          ret = true;
        }
      }

    }
  }
  return ret;
}

// get the linear part - x in x+y+y^2
Expression BasicExpression::GetLinearPart()
{
  std::vector<double> lincoeff;
  std::vector<int> linvi;
  std::vector<std::string> linvn;
  double c;
  GetLinearInfo(lincoeff, linvi, linvn, c);
  int i;
  Expression ret;
  if (lincoeff.size() > 0)
  {
    ret->SetOpType(VAR);
    ret->SetVarIndex(linvi[0]);
    ret->SetCoeff(lincoeff[0]);
    ret->SetVarName(linvn[0]);
    ret->SetExponent(1.0);
    if (lincoeff.size() > 1)
    {
      Expression addend(1.0, -1, NOTVARNAME);
      for(i = 1; i < (int) lincoeff.size(); i++)
      {
        addend->SetVarIndex(linvi[i]);
        addend->SetCoeff(lincoeff[i]);
        addend->SetVarName(linvn[i]);
        ret = ret + addend;
      }
    }
  }
  return ret;
}

// get the pure linar part - x+y in x+y+y^2
Expression BasicExpression::GetPureLinearPart()
{
  std::vector<double> lincoeff;
  std::vector<int> linvi;
  std::vector<std::string> linvn;
  double c;
  GetPureLinearInfo(lincoeff, linvi, linvn, c);
  int i;
  Expression ret(0.0);
  if (lincoeff.size() > 0)
  {
    ret->SetOpType(VAR);
    ret->SetVarIndex(linvi[0]);
    ret->SetCoeff(lincoeff[0]);
    ret->SetVarName(linvn[0]);
    ret->SetExponent(1.0);
    if (lincoeff.size() > 1)
    {
      for(i = 1; i < (int) lincoeff.size(); i++)
      {
        Expression addend(lincoeff[i], linvi[i], linvn[i]);
        ret = SumLink(ret, addend);
      }
    }
  }
  return ret;
}

// get the nonlinear part - nonlin(y) in expr(x,y) = lin(x) + nonlin(y)
Expression BasicExpression::GetNonlinearPart()
{
  Expression ret(GetPureNonlinearPart());
  std::vector<double> linval;
  std::vector<int> linidx;
  std::vector<std::string> linvn;
  double c = 0;
  GetPureLinearInfo(linval, linidx, linvn, c);
  int i;
  Expression addend(1.0, -1, NOTVARNAME);
  // we cycle backwards to keep the order of addends in ret
  for(i = linidx.size() - 1; i >= 0 ; i--)
  {
    if (ret->DependsOnVariable(linidx[i]))
    {
      // pure nonlinear part depends on varindex i, add it to ret
      addend->SetCoeff(linval[i]);
      addend->SetVarIndex(linidx[i]);
      addend->SetVarName(linvn[i]);
      ret = addend + ret;
    }
  }
  return ret;
}


// get the purely nonlinear part - e.g. only y^2 in x+y+y^2
Expression BasicExpression::GetPureNonlinearPart()
{
  Expression ret(0.0);
  if (!IsLeaf())
  {
    if (GetOpType() == SUM)
    {
      int i;
      for(i = 0; i < GetSize(); i++)
      {
        if (!GetNode(i)->IsLinear())
        {
          ret = SumLink(ret, GetNode(i));
        }
      }
    }
    else if (GetOpType() == DIFFERENCE)
    {
      int i;
      for(i = 0; i < GetSize(); i++)
      {
        if (!GetNode(i)->IsLinear())
        {
          ret = ret - GetNode(i);
        }
      }
    }
    else if (GetOpType() == PLUS)
    {
      ret = GetNode(0);
    }
    else if (GetOpType() == MINUS)
    {
      ret = GetNode(0);
      ret->SetCoeff(-ret->GetCoeff());
    }
    else
    {
      ret = *this;
    }
  }
  else
  {
    // leaf but can be a power
    if (GetExponent() != 0 && GetExponent() != 1)
    {
      ret = *this;
    }
  }
  return ret;
}

// get value of additive constant
double BasicExpression::GetConstantPart()
{
  double ret = 0;
  if (IsConstant())
  {
    ret = GetValue();
  }
  else if (!IsLeaf())
  {
    int op = GetOpType();
    if (op == SUM || op == DIFFERENCE)
    {
      int i = 0;
      int sz = GetSize();
      while(i < sz)
      {
        if (GetNode(i)->IsConstant())
        {
          if (op == SUM || (op == DIFFERENCE && i == 0))
          {
            ret += GetNode(i)->GetValue();
          }
          else
          {
            ret -= GetNode(i)->GetValue();
          }
        }
        i++;
      }
    }
  }
  return ret;
}

// doesn't deal with additive constants in PLUS/MINUS operands
double BasicExpression::RemoveAdditiveConstant()
{
  double ret = 0;
  if (IsConstant())
  {
    ret = GetValue();
    SetValue(0);
  }
  else if (!IsLeaf())
  {
    int op = GetOpType();
    if (op == SUM || op == DIFFERENCE)
    {
      int i = 0;
      int sz = GetSize();
      while(i < sz)
      {
        if (GetNode(i)->IsConstant())
        {
          if (op == SUM || (op == DIFFERENCE && i == 0))
          {
            ret += GetNode(i)->GetValue();
          }
          else
          {
            ret -= GetNode(i)->GetValue();
          }
          DeleteNode(i);
          sz--;
        }
        else
        {
          i++;
        }
      }
    }
  }
  return ret;
}

/************** expression creation (no change to args) ***************/

// BIG FAT WARNING: when you change these operators, also please
// change their "-Link" counterparts!

// sums:
Expression operator + (Expression a,
                       Expression b)
{
  Expression ret;
  // make a preliminary check
  if (a->GetCoeff() == 0 || a->HasValue(0))
  {
    ret.SetToCopyOf(b);
    return ret;
  }
  if (b->GetCoeff() == 0 || b->HasValue(0))
  {
    ret.SetToCopyOf(a);
    return ret;
  }
  if (!(a->IsConstant() && b->IsConstant()) && a->IsEqualToNoCoeff(b))
  {
    a->SetCoeff(a->GetCoeff() + b->GetCoeff());
    if (std::abs(a->GetCoeff()) < Ev3NearZero())
    {
      // simplify to zero - for differences
      Expression zero(0.0);
      return zero;
    }
    else
    {
      ret.SetToCopyOf(a);
      return ret;
    }
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      b->IsLeaf() && b->GetOpType() == CONST)
  {
    // a, b are numbers - add them
    ret.SetToCopyOf(a);
    ret->SetValue(a->GetValue() + b->GetValue());
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    return ret;
  }
  else if (a->IsLeaf() && a->GetOpType() == VAR &&
           b->IsLeaf() && b->GetOpType() == VAR &&
           a->GetVarIndex() == b->GetVarIndex() &&
           a->GetExponent() == b->GetExponent())
  {
    // a, b are the same variable - add coefficients
    ret.SetToCopyOf(a);
    ret->SetCoeff(a->GetCoeff() + b->GetCoeff());
    return ret;
  }
  else if (a->GetOpType() == SUM && b->GetOpType() != SUM)
  {
    // a is a sum and b isn't - just add the term b
    ret.SetToCopyOf(a);
    ret->DistributeCoeffOverSum();
    Int i = 0;
    bool couldsimplify = false;
    Expression tmp;
    // if t is a leaf and there is a like leaf in this,
    // just add it to the value/coefficient
    if (b->IsLeaf() && b->GetOpType() == CONST)
    {
      // b is a constant
      for (i = 0; i < ret->GetSize(); i++)
      {
        tmp = ret->GetNode(i);
        if (tmp->IsLeaf() && tmp->GetOpType() == CONST)
        {
          tmp->SetValue(tmp->GetValue() + b->GetValue() / ret->GetCoeff());
          tmp->SetCoeff(1.0);
          tmp->SetExponent(1.0); // NB: changing tmp should also change a
          couldsimplify = true;
          break;
        }
      }
    }
    else if (b->IsLeaf() && b->GetOpType() == VAR)
    {
      // b is a variable
      for (i = 0; i < ret->GetSize(); i++)
      {
        if (ret->GetNode(i)->IsLeaf() && ret->GetNode(i)->GetOpType() == VAR &&
            b->GetVarIndex() == ret->GetNode(i)->GetVarIndex() &&
            b->GetExponent() == ret->GetNode(i)->GetExponent())
        {
          double tc = ret->GetNode(i)->GetCoeff() +
                      b->GetCoeff() / ret->GetCoeff();
          // warning: tc could be zero, but it would be cumbersome
          // to simplify it here - do it in SimplifyConstant
          ret->GetNode(i)->SetCoeff(tc);
          couldsimplify = true;
          break;
        }
      }
    }
    else if (!b->IsLeaf())
    {
      // a is a sum, b is a nonleaf, look for a subnode of a similar to b
      for (i = 0; i < ret->GetSize(); i++)
      {
        if (ret->GetNode(i)->IsEqualTo(b))
        {
          // found one, add coefficients - notice, as above, coeff could
          // be zero, but deal with that case in SimplifyConstant
          ret->GetNode(i)->SetCoeff(ret->GetNode(i)->GetCoeff()
                                    + b->GetCoeff());
          couldsimplify = true;
          break;
        }
      }
    }
    if (!couldsimplify)
    {
      // either could not simplify in steps above, or b is an operator
      ret->AddCopyOfNode(b);
    }
    return ret;
  }
  else if (a->GetOpType() == SUM && b->GetOpType() == SUM)
  {
    // a, b are sums - add terms of b to a
    b->DistributeCoeffOverSum();
    ret.SetToCopyOf(a);
    Int i = 0;
    Int s = b->GetSize();
    for (i = 0; i < s; i++)
    {
      ret = ret + b->GetNode(i);
    }
    return ret;
  }
  else if (a->GetOpType() != SUM && b->GetOpType() == SUM)
  {
    // a is not a sum but b is - transform this into a sum
    ret.SetToCopyOf(b);
    ret = ret + a;
    return ret;
  }
  else
  {
    // all other cases - make new node on top of the addends
    ret->SetOpType(SUM);
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->AddCopyOfNode(a);
    ret->AddCopyOfNode(b);
    return ret;
  }
}


// product
Expression operator * (Expression a,
                       Expression t)
{
  Expression ret;
  // make a preliminary check
  if (a->GetCoeff() == 0 || t->GetCoeff() == 0 ||
      a->HasValue(0) || t->HasValue(0))
  {
    Expression zero(0.0);
    return zero;
  }
  if (a->HasValue(1))
  {
    return t;
  }
  if (t->HasValue(1))
  {
    return a;
  }
  if (!(a->IsConstant() && t->IsConstant()) && a->IsEqualToNoCoeff(t))
  {
    Expression two(2.0);
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret = ret ^ two;
    ret->SetCoeff(a->GetCoeff() * t->GetCoeff());
    return ret;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      t->IsLeaf() && t->GetOpType() == CONST)
  {
    // a, t are numbers - multiply them
    ret.SetToCopyOf(a);
    ret->SetValue(a->GetValue() * t->GetValue());
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    return ret;
  }
  else if (a->IsLeaf() && a->GetOpType() == VAR &&
           t->IsLeaf() && t->GetOpType() == VAR &&
           a->GetVarIndex() == t->GetVarIndex())
  {
    // a, t are the same variable - multiply coefficients
    // and add exponents
    ret.SetToCopyOf(a);
    ret->SetCoeff(a->GetCoeff() * t->GetCoeff());
    ret->SetExponent(a->GetExponent() + t->GetExponent());
    return ret;
  }
  else if (t->IsConstant())
  {
    // t is constant, set coeff of a
    ret.SetToCopyOf(a);
    ret->SetCoeff(a->GetCoeff() * t->GetValue());
    ret->DistributeCoeffOverSum();
    return ret;
  }
  else if (a->IsConstant())
  {
    // a is constant, set coeff of t
    ret.SetToCopyOf(t);
    ret->SetCoeff(t->GetCoeff() * a->GetValue());
    ret->DistributeCoeffOverSum();
    return ret;
  }
  else if (a->GetOpType() == PRODUCT && t->GetOpType() != PRODUCT)
  {
    // a is a product and t isn't - just multiply the term t
    ret.SetToCopyOf(a);
    Int i = 0;
    bool couldsimplify = false;
    if (t->IsLeaf() && t->GetOpType() == VAR)
    {
      // t is a variable
      Expression tmp;
      for (i = 0; i < ret->GetSize(); i++)
      {
        tmp = ret->GetNode(i);
        if (tmp->IsLeaf() && tmp->GetOpType() == VAR &&
            t->GetVarIndex() == tmp->GetVarIndex())
        {
          // found same variable in a, multiply coeffs and add exponents
          tmp->SetCoeff(tmp->GetCoeff() * t->GetCoeff());
          tmp->SetExponent(tmp->GetExponent() + t->GetExponent());
          couldsimplify = true;
          break;
        }
      }
    }
    // here we shan't try to simplify f*f <-- f^2 (f nonleaf)
    // because a product of nonleaves is easier to manipulate than
    // a power (as it adds a depth level)
    if (!couldsimplify)
    {
      // either could not simplify in steps above, or t is an operator
      ret->AddCopyOfNode(t);
    }
    return ret;
  }
  else if (a->GetOpType() == PRODUCT && t->GetOpType() == PRODUCT)
  {
    // a, t are products - multiply terms of t to a
    t->DistributeCoeffOverProduct();
    ret.SetToCopyOf(a);
    Int s = t->GetSize();
    for (Int i = 0; i < s; i++)
    {
      ret = ret * t->GetNode(i);
    }
    return ret;
  }
  else if (a->GetOpType() != PRODUCT && t->GetOpType() == PRODUCT)
  {
    // a is not a products but t is - transform this into a product
    ret.SetToCopyOf(t);
    ret = ret * a;
    return ret;
  }
  else
  {
    // all other cases - make new node on top of the addends
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(PRODUCT);
    ret->AddCopyOfNode(a);
    ret->AddCopyOfNode(t);
    return ret;
  }
}

// fractions:
Expression operator / (Expression a,
                       Expression t)
{
  Expression ret;
  // make a preliminary check
  if (t->GetCoeff() == 0)
  {
    // divide by zero
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("operator/");
    std::string myop("t.GetCoeff()==0");
    std::string mydesc("Divisor cannot be zero");
    std::string myinfo(HELPURL);
    std::string mydiv(NONE);
    throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
  }
  if (a->GetCoeff() == 0 || a->HasValue(0))
  {
    // dividend has zero coeff, return zero
    Expression zero(0.0);
    return zero;
  }
  if (t->HasValue(1))
  {
    ret.SetToCopyOf(a);
    return ret;
  }
  if (!(a->IsConstant() && t->IsConstant()) && a->IsEqualToNoCoeff(t))
  {
    // dividend = divisor, return ratio of coefficients
    Expression one(1.0);
    one->SetCoeff(a->GetCoeff() / t->GetCoeff());
    return one;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      t->IsLeaf() && t->GetOpType() == CONST)
  {
    // a, t are numbers - divide them
    if (t->GetValue() == 0)
    {
      unsigned long mycode(0);
      std::string myif("Expression Building");
      std::string myscope("operator/");
      std::string myop("t.GetValue()==0");
      std::string mydesc("Divisor cannot be zero");
      std::string myinfo(HELPURL);
      std::string mydiv(NONE);
      throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
    }
    else
    {
      ret.SetToCopyOf(a);
      ret->SetValue(a->GetValue() / t->GetValue());
      ret->SetCoeff(1.0);
      ret->SetExponent(1.0);
      return ret;
    }
  }
  else if (t->HasValue(1))
  {
    // divide by constant 1, don't do anything
    ret.SetToCopyOf(a);
    return ret;
  }
  else if (t->IsConstant())
  {
    // t is constant, set coeff of a
    ret.SetToCopyOf(a);
    ret->SetCoeff(a->GetCoeff() / t->GetValue());
    ret->DistributeCoeffOverSum();
    return ret;
  }
  else if (a->IsVariable() && t->IsVariable() &&
           a->GetVarIndex() == t->GetVarIndex())
  {
    // cx^e / dx^f = (c/d)x^(e-f)
    ret.SetToCopyOf(a);
    double te = a->GetExponent() - t->GetExponent();
    double tc = a->GetCoeff() / t->GetCoeff();
    if (std::abs(te) < Ev3NearZero())
    {
      Expression c(tc);
      return c;
    }
    ret->SetCoeff(tc);
    ret->SetExponent(te);
    return ret;
  }
  else if (a->IsVariable() && t->GetOpType() == PRODUCT)
  {
    // a is a variable, t is a product - see if a appears in t
    // and cancel common term
    // first simplify coeffs of divisor
    Expression at;
    at.SetToCopyOf(a);
    ret.SetToCopyOf(t);
    ret->ConsolidateProductCoeffs();
    // denominator
    if (std::abs(ret->GetCoeff()) < Ev3NearZero())
    {
      // divide by zero
      unsigned long mycode(22);
      std::string myif("Expression Building");
      std::string myscope("operator/");
      std::string myop("t->GetCoeff()");
      std::string mydesc("Divisor cannot be zero");
      std::string myinfo(HELPURL);
      std::string mydiv(NONE);
      throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
    }
    if (std::abs(at->GetCoeff()) < Ev3NearZero())
    {
      Expression zero(0.0);
      return zero;
    }
    double accumulatedcoeff = at->GetCoeff() / ret->GetCoeff();
    at->SetCoeff(1.0);
    ret->SetCoeff(1.0);
    // now try simplification
    Int i;
    for (i = 0; i < ret->GetSize(); i++)
    {
      if (ret->GetNode(i)->GetOpType() == VAR &&
          at->GetVarIndex() == ret->GetNode(i)->GetVarIndex())
      {
        double te = at->GetExponent() - ret->GetNode(i)->GetExponent();
        if (std::abs(te) < Ev3NearZero())
        {
          // exponents are the same, just cancel
          at->One();
          ret->DeleteNode(i);
        }
        else if (te > 0)
        {
          // numerator remains, cancel denominator
          at->SetExponent(te);
          ret->DeleteNode(i);
        }
        else if (te < 0)
        {
          // numerator goes to one, denominator remains
          at->One();
          ret->GetNode(i)->SetExponent(-te);
        }
        // exit loop
        break;
      }
    }
    // check that denominator (t) has more than one operand;
    // if not, bring up a rank level
    if (ret->GetSize() == 1)
    {
      ret = ret->GetNode(0);
    }
    // build ratio
    Expression ret2;
    ret2->SetOpType(FRACTION);
    ret2->SetCoeff(accumulatedcoeff);
    ret2->SetExponent(1.0);
    ret2->AddCopyOfNode(at);
    ret2->AddCopyOfNode(ret);
    return ret2;
  }
  else if (t->IsVariable() && a->GetOpType() == PRODUCT)
  {
    // t is a variable, a is a product - see if t appears in a
    // and cancel common term
    // first simplify coeffs of divisor
    Expression bt;
    bt.SetToCopyOf(t);
    ret.SetToCopyOf(a);
    ret->ConsolidateProductCoeffs();
    // denominator - already checked
    if (std::abs(ret->GetCoeff()) < Ev3NearZero())
    {
      Expression zero(0.0);
      return zero;
    }
    double accumulatedcoeff = ret->GetCoeff() / bt->GetCoeff();
    ret->SetCoeff(1.0);
    bt->SetCoeff(1.0);
    // now try simplification
    Int i;
    for (i = 0; i < ret->GetSize(); i++)
    {
      if (ret->GetNode(i)->GetOpType() == VAR &&
          bt->GetVarIndex() == ret->GetNode(i)->GetVarIndex())
      {
        double te = ret->GetNode(i)->GetExponent() - bt->GetExponent();
        if (std::abs(te) < Ev3NearZero())
        {
          // exponents are the same, just cancel
          bt->One();
          ret->DeleteNode(i);
        }
        else if (te > 0)
        {
          // numerator remains, cancel denominator
          bt->One();
          ret->GetNode(i)->SetExponent(te);
        }
        else if (te < 0)
        {
          // numerator goes to one, denominator remains
          bt->SetExponent(-te);
          ret->DeleteNode(i);
        }
        // exit loop
        break;
      }
    }
    // check that numerator (a) has more than one operands;
    // if not, bring up a rank level
    if (ret->GetSize() == 1)
    {
      ret = ret->GetNode(0);
    }
    // build ratio
    Expression ret2;
    ret2->SetOpType(FRACTION);
    ret2->SetCoeff(accumulatedcoeff);
    ret2->SetExponent(1.0);
    ret2->AddCopyOfNode(ret);
    ret2->AddCopyOfNode(bt);
    return ret2;
  }
  else if (a->GetOpType() == PRODUCT && t->GetOpType() == PRODUCT)
  {
    // a, t are products, try to cancel common terms
    Expression at;
    Expression bt;
    at.SetToCopyOf(a);
    bt.SetToCopyOf(t);
    Int i = 0, j = 0;
    double accumulatedcoeff;
    // first simplify coefficients of operands
    at->ConsolidateProductCoeffs();
    bt->ConsolidateProductCoeffs();
    // denominator
    if (std::abs(bt->GetCoeff()) < Ev3NearZero())
    {
      // divide by zero
      unsigned long mycode(21);
      std::string myif("Expression Building");
      std::string myscope("operator/");
      std::string myop("t->GetCoeff()");
      std::string mydesc("Divisor cannot be zero");
      std::string myinfo(HELPURL);
      std::string mydiv(NONE);
      throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
    }
    if (std::abs(at->GetCoeff()) < Ev3NearZero())
    {
      Expression zero(0.0);
      return zero;
    }
    // save ratio of coeffs of products
    accumulatedcoeff = at->GetCoeff() / bt->GetCoeff();
    at->SetCoeff(1.0);
    bt->SetCoeff(1.0);
    // now try simplification
    i = 0;
    bool isnumeratorempty = false;
    bool isdenominatorempty = false;
    int szi = at->GetSize();
    int szj = bt->GetSize();
    while(!isnumeratorempty && !isdenominatorempty && i < szi)
    {
      j = 0;
      while(!isnumeratorempty && !isdenominatorempty && j < szj)
      {
        if (at->GetNode(i)->IsEqualTo(bt->GetNode(j)))
        {
          // found like terms i and j
          at->DeleteNode(i);
          szi--;
          if(szi == 0)
          {
            isnumeratorempty = true;
            at->One();
          }
          bt->DeleteNode(j);
          szj--;
          if (szj == 0)
          {
            isdenominatorempty = true;
            bt->One();
          }
          i--;   // cancel the effect of the later i++
          break; // go to outer cycle
        }
        else
        {
          j++;
        }
      }
      i++;
    }
    if (bt->HasValue(1))
    {
      // denominator is 1, return a
      at->SetCoeff(accumulatedcoeff);
      return at;
    }
    // now construct fraction
    // check that numerator, denominator have more than one operands;
    // if not, bring up a rank level
    if (at->GetSize() == 1)
    {
      at = at->GetNode(0);
    }
    if (bt->GetSize() == 1)
    {
      bt = bt->GetNode(0);
    }
    ret->SetCoeff(accumulatedcoeff); // already contains coeffs of a, t
    ret->SetExponent(1.0);
    ret->SetOpType(FRACTION);
    ret->AddCopyOfNode(at);
    ret->AddCopyOfNode(bt);
    return ret;
  }
  else
  {
    Expression at;
    Expression bt;
    at.SetToCopyOf(a);
    bt.SetToCopyOf(t);
    ret->SetCoeff(at->GetCoeff() / bt->GetCoeff());
    at->SetCoeff(1.0);
    bt->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(FRACTION);
    ret->AddCopyOfNode(at);
    ret->AddCopyOfNode(bt);
    return ret;
  }
}

// unary minus:
Expression operator - (Expression a)
{
  Expression ret;
  ret.SetToCopyOf(a);
  if (ret->IsLeaf() && ret->GetOpType() == CONST)
  {
    ret->SetValue(- ret->GetValue());
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
  }
  else
  {
    ret->SetCoeff(- ret->GetCoeff());
  }
  return ret;
}

// binary minus:
Expression operator - (Expression a,
                       Expression b)
{
  Expression ret;
  if (a->HasValue(0))
    return -b;
  if (b->HasValue(0))
  {
    ret.SetToCopyOf(a);
    return a;
  }
  ret = a + (-b);
  return ret;
}

// power:
Expression operator ^ (Expression a,
                       Expression t)
{
  // make a preliminary check
  Expression ret;
  if (a->GetCoeff() == 0)
  {
    // *this is zero, just return zero
    Expression zero(0.0);
    return zero;
  }
  if (t->HasValue(0.0))
  {
    // exponent is 0, just return 1
    Expression one(1.0);
    return one;
  }
  else if (t->HasValue(1.0))
  {
    // exponent is 1, just return a
    ret.SetToCopyOf(a);
    return ret;
  }
  if (a->HasValue(0.0))
  {
    // base is zero, return 0
    Expression zero(0.0);
    return zero;
  }
  else if (a->HasValue(1.0))
  {
    // base is one, return 1
    Expression one(1.0);
    return one;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      t->IsLeaf() && t->GetOpType() == CONST)
  {
    // constant to constant
    ret.SetToCopyOf(a);
    ret->SetValue(pow(ret->GetValue(), t->GetValue()));
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    return ret;
  }
  else if (a->IsLeaf() && a->GetOpType() == VAR &&
           t->IsLeaf() && t->GetOpType() == CONST)
  {
    // variable to constant
    ret.SetToCopyOf(a);
    ret->SetCoeff(pow(ret->GetCoeff(), t->GetValue()));
    ret->SetExponent(ret->GetExponent() * t->GetValue());
    return ret;
  }
  else
  {
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(POWER);
    ret->AddCopyOfNode(a);
    ret->AddCopyOfNode(t);
    return ret;
  }
}

Expression Sin(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(sin(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SIN);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Cos(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(cos(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COS);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Tan(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(tan(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(TAN);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Asin(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(asin(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ASIN);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Acos(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(acos(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ACOS);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Atan(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(atan(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ATAN);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Sinh(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(sinh(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SINH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Cosh(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(cosh(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COSH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Tanh(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(tanh(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(TANH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Asinh(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(asinh(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ASINH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Acosh(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(acosh(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ACOSH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Atanh(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(atanh(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ATANH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Log2(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
  {
    // *this is zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Log2");
    std::string myop("IsZero()");
    std::string mydesc("log2(0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  if (a->IsLessThan(0))
  {
    // argument is < zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Log2");
    std::string myop("value <= 0");
    std::string mydesc("log2(<=0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = ret->GetValue();
    assert(t >= 0);
    ret->SetCoeff(1.0);
    ret->SetValue(log2(t));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LOG2);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Log10(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
  {
    // *this is zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Log10");
    std::string myop("IsZero()");
    std::string mydesc("log10(0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  if (a->IsLessThan(0))
  {
    // argument is < zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Log10");
    std::string myop("value <= 0");
    std::string mydesc("log10(<=0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = ret->GetValue();
    assert(t >= 0);
    ret->SetCoeff(1.0);
    ret->SetValue(log10(t));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LOG10);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Log(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
  {
    // *this is zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Log");
    std::string myop("IsZero()");
    std::string mydesc("log(0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  if (a->IsLessThan(0))
  {
    // argument is < zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Log");
    std::string myop("value <= 0");
    std::string mydesc("log(<=0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = ret->GetValue();
    assert(t >= 0);
    ret->SetCoeff(1.0);
    ret->SetValue(log(t));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LOG);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Ln(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
  {
    // *this is zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Ln");
    std::string myop("IsZero()");
    std::string mydesc("ln(0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  if (a->IsLessThan(0))
  {
    // argument is < zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Ln");
    std::string myop("value <= 0");
    std::string mydesc("ln(<=0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = ret->GetValue();
    assert(t >= 0);
    ret->SetCoeff(1.0);
    ret->SetValue(log(t));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LN);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Lngamma(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(lgamma(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LNGAMMA);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Gamma(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(exp(lgamma(a->GetValue())));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(GAMMA);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Exp(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(exp(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(EXP);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Erf(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(erf(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ERF);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Erfc(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(erfc(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ERFC);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Sqrt(Expression a)
{
  // make a preliminary check
  if (a->IsLessThan(0) && !a->HasValue(0))
  {
    // argument is < zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Sqrt");
    std::string myop("value < 0");
    std::string mydesc("sqrt(<0) is complex, can't do");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = a->GetValue();
    assert(t >= 0);
    ret->SetCoeff(1.0);
    ret->SetValue(sqrt(t));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SQRT);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Cbrt(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(cbrt(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(CBRT);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression BesselJ0(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(j0(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELJ0);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression BesselJ1(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(j1(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELJ1);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression BesselY0(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(y0(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELY0);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression BesselY1(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(y1(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELY1);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Sign(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    double val = a->GetValue();
    ret->SetValue((val < 0.0) ? -1.0 : (val > 0.0) ? 1.0 : 0.0);
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SIGN);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Rint(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(floor(0.5 + a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(RINT);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Abs(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    ret->SetCoeff(1.0);
    ret->SetValue(std::abs(a->GetValue()));
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ABS);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Cot(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
  {
    // *this is zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Cot");
    std::string myop("IsZero()");
    std::string mydesc("cot(0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = tan(a->GetValue());
    assert(t != 0);
    ret->SetCoeff(1.0);
    ret->SetValue(1 / t);
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COT);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

Expression Coth(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
  {
    // *this is zero, can't do
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("Coth");
    std::string myop("IsZero()");
    std::string mydesc("coth(0) is undefined");
    std::string myinfo(HELPURL);
    throw ErrNotPermitted(mycode, myif, myscope, myop, mydesc, myinfo);
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    Expression ret;
    ret.SetToCopyOf(a);
    double t = tanh(a->GetValue());
    assert(t != 0);
    ret->SetCoeff(1.0);
    ret->SetValue(1 / t);
    ret->SetExponent(1.0);
    ret->SetOpType(CONST);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COTH);
    ret->AddCopyOfNode(a);
    return ret;
  }
}

/***************** expression creation (affects arguments) ***********/

// sums:
Expression SumLink(Expression a,
                   Expression b)
{
  // make a preliminary check
  if (a->GetCoeff() == 0 || a->HasValue(0))
    return b;
  if (b->GetCoeff() == 0 || b->HasValue(0))
    return a;
  if (!(a->IsConstant() && b->IsConstant()) && a->IsEqualToNoCoeff(b))
  {
    a->SetCoeff(a->GetCoeff() + b->GetCoeff());
    if (std::abs(a->GetCoeff()) < Ev3NearZero())
    {
      // simplify to zero - for differences
      Expression zero(0.0);
      return zero;
    }
    else
      return a;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      b->IsLeaf() && b->GetOpType() == CONST)
  {
    // a, b are numbers - add them
    a->SetValue(a->GetValue() + b->GetValue());
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    return a;
  }
  else if (a->IsLeaf() && a->GetOpType() == VAR &&
           b->IsLeaf() && b->GetOpType() == VAR &&
           a->GetVarIndex() == b->GetVarIndex() &&
           a->GetExponent() == b->GetExponent())
  {
    // a, b are the same variable - add coefficients
    a->SetCoeff(a->GetCoeff() + b->GetCoeff());
    return a;
  }
  else if (a->GetOpType() == SUM && b->GetOpType() != SUM)
  {
    // a is a sum and b isn't - just add the term b
    a->DistributeCoeffOverSum();
    Int i = 0;
    bool couldsimplify = false;
    Expression tmp;
    // if t is a leaf and there is a like leaf in this,
    // just add it to the value/coefficient
    if (b->IsLeaf() && b->GetOpType() == CONST)
    {
      // b is a constant
      for (i = 0; i < a->GetSize(); i++)
      {
        tmp = a->GetNode(i);
        if (tmp->IsLeaf() && tmp->GetOpType() == CONST)
        {
          tmp->SetValue(tmp->GetValue() + b->GetValue() / a->GetCoeff());
          tmp->SetCoeff(1.0);
          tmp->SetExponent(1.0); // NB: changing tmp should also change a
          couldsimplify = true;
          break;
        }
      }
    }
    else if (b->IsLeaf() && b->GetOpType() == VAR)
    {
      // b is a variable
      for (i = 0; i < a->GetSize(); i++)
      {
        if (a->GetNode(i)->IsLeaf() && a->GetNode(i)->GetOpType() == VAR &&
            b->GetVarIndex() == a->GetNode(i)->GetVarIndex() &&
            b->GetExponent() == a->GetNode(i)->GetExponent())
        {
          double tc = a->GetNode(i)->GetCoeff() + b->GetCoeff() / a->GetCoeff();
          // warning: tc could be zero, but it would be cumbersome
          // to simplify it here - do it in SimplifyConstant
          a->GetNode(i)->SetCoeff(tc);
          couldsimplify = true;
          break;
        }
      }
    }
    else if (!b->IsLeaf())
    {
      // a is a sum, b is a nonleaf, look for a subnode of a similar to b
      for (i = 0; i < a->GetSize(); i++)
      {
        if (a->GetNode(i)->IsEqualTo(b))
        {
          // found one, add coefficients - notice, as above, coeff could
          // be zero, but deal with that case in SimplifyConstant
          a->GetNode(i)->SetCoeff(a->GetNode(i)->GetCoeff() + b->GetCoeff());
          couldsimplify = true;
          break;
        }
      }
    }
    if (!couldsimplify)
    {
      // either could not simplify in steps above, or b is an operator
      a->AddNode(b);
    }
    return a;
  }
  else if (a->GetOpType() == SUM && b->GetOpType() == SUM)
  {
    // a, b are sums - add terms of b to a
    b->DistributeCoeffOverSum();
    Int i = 0;
    Int s = b->GetSize();
    for (i = 0; i < s; i++)
    {
      a = SumLink(a, b->GetNode(i));
    }
    return a;
  }
  else if (a->GetOpType() != SUM && b->GetOpType() == SUM)
  {
    // a is not a sum but b is - transform this into a sum
    b = SumLink(b, a);
    return b;
  }
  else
  {
    // all other cases - make new node on top of the addends
    Expression ret;


    ret->SetOpType(SUM);
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->AddNode(a);
    ret->AddNode(b);

    return ret;
  }
}


// product
Expression ProductLink(Expression a,
                       Expression t)
{
  // make a preliminary check
  if (a->GetCoeff() == 0 || t->GetCoeff() == 0 ||
      a->HasValue(0) || t->HasValue(0))
  {
    Expression zero(0.0);
    return zero;
  }
  if (a->HasValue(1))
    return t;
  if (t->HasValue(1))
    return a;
  if (!(a->IsConstant() && t->IsConstant()) && a->IsEqualToNoCoeff(t))
  {
    Expression two(2.0);
    double c = a->GetCoeff();
    a->SetCoeff(1.);
    Expression power2(a ^ two);
    power2->SetCoeff(c * t->GetCoeff());
    return power2;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      t->IsLeaf() && t->GetOpType() == CONST)
  {
    // a, t are numbers - multiply them
    a->SetValue(a->GetValue() * t->GetValue());
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    return a;
  }
  else if (a->IsLeaf() && a->GetOpType() == VAR &&
           t->IsLeaf() && t->GetOpType() == VAR &&
           a->GetVarIndex() == t->GetVarIndex())
  {
    // a, t are the same variable - multiply coefficients
    // and add exponents
    a->SetCoeff(a->GetCoeff() * t->GetCoeff());
    a->SetExponent(a->GetExponent() + t->GetExponent());
    return a;
  }
  else if (t->IsConstant())
  {
    // t is constant, set coeff of a
    a->SetCoeff(a->GetCoeff() * t->GetValue());
    a->DistributeCoeffOverSum();
    return a;
  }
  else if (a->IsConstant())
  {
    // a is constant, set coeff of t
    t->SetCoeff(t->GetCoeff() * a->GetValue());
    t->DistributeCoeffOverSum();
    return t;
  }
  else if (a->GetOpType() == PRODUCT && t->GetOpType() != PRODUCT)
  {
    // a is a product and t isn't - just multiply the term t
    Int i = 0;
    bool couldsimplify = false;
    if (t->IsLeaf() && t->GetOpType() == VAR)
    {
      // t is a variable
      Expression tmp;
      for (i = 0; i < a->GetSize(); i++)
      {
        tmp = a->GetNode(i);
        if (tmp->IsLeaf() && tmp->GetOpType() == VAR &&
            t->GetVarIndex() == tmp->GetVarIndex())
        {
          // found same variable in a, multiply coeffs and add exponents
          tmp->SetCoeff(tmp->GetCoeff() * t->GetCoeff());
          tmp->SetExponent(tmp->GetExponent() + t->GetExponent());
          couldsimplify = true;
          break;
        }
      }
    }
    // here we shan't try to simplify f*f <-- f^2 (f nonleaf)
    // because a product of nonleaves is easier to manipulate than
    // a power (as it adds a depth level)
    if (!couldsimplify)
    {
      // either could not simplify in steps above, or t is an operator
      a->AddNode(t);
    }
    return a;
  }
  else if (a->GetOpType() == PRODUCT && t->GetOpType() == PRODUCT)
  {
    // a, t are products - multiply terms of t to a
    t->DistributeCoeffOverProduct();
    Int i = 0;
    Int s = t->GetSize();
    for (i = 0; i < s; i++)
    {
      a = ProductLink(a, t->GetNode(i));
    }
    return a;
  }
  else if (a->GetOpType() != PRODUCT && t->GetOpType() == PRODUCT)
  {
    // a is not a products but t is - transform this into a product
    t = ProductLink(t, a);
    return t;
  }
  else
  {
    // all other cases - make new node on top of the addends
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(PRODUCT);
    ret->AddNode(a);
    ret->AddNode(t);
    return ret;
  }
}

// fractions:
Expression FractionLink(Expression a,
                        Expression t)
{
  // make a preliminary check
  if (t->GetCoeff() == 0)
  {
    // divide by zero
    unsigned long mycode(0);
    std::string myif("Expression Building");
    std::string myscope("FractionLink");
    std::string myop("t.GetCoeff()==0");
    std::string mydesc("Divisor cannot be zero");
    std::string myinfo(HELPURL);
    std::string mydiv(NONE);
    throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
  }
  if (a->GetCoeff() == 0 || a->HasValue(0))
  {
    // dividend has zero coeff, return zero
    Expression zero(0.0);
    return zero;
  }
  if (t->HasValue(1))
    return a;
  if (!(a->IsConstant() && t->IsConstant()) && a->IsEqualToNoCoeff(t))
  {
    // dividend = divisor, return ratio of coefficients
    Expression one(1.0);
    one->SetCoeff(a->GetCoeff() / t->GetCoeff());
    return one;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      t->IsLeaf() && t->GetOpType() == CONST)
  {
    // a, t are numbers - divide them
    if (t->GetValue() == 0)
    {
      unsigned long mycode(0);
      std::string myif("Expression Building");
      std::string myscope("FractionLink");
      std::string myop("t.GetValue()==0");
      std::string mydesc("Divisor cannot be zero");
      std::string myinfo(HELPURL);
      std::string mydiv(NONE);
      throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
    }
    else
    {
      a->SetValue(a->GetValue() / t->GetValue());
      a->SetCoeff(1.0);
      a->SetExponent(1.0);
      return a;
    }
  }
  else if (t->HasValue(1))
  {
    // divide by constant 1, don't do anything
    return a;
  }
  else if (t->IsConstant())
  {
    // t is constant, set coeff of a
    a->SetCoeff(a->GetCoeff() / t->GetValue());
    a->DistributeCoeffOverSum();
    return a;
  }
  else if (a->IsVariable() && t->IsVariable() &&
           a->GetVarIndex() == t->GetVarIndex())
  {
    // cx^e / dx^f = (c/d)x^(e-f)
    double te = a->GetExponent() - t->GetExponent();
    double tc = a->GetCoeff() / t->GetCoeff();
    if (std::abs(te) < Ev3NearZero())
    {
      Expression c(tc);
      return c;
    }
    a->SetCoeff(tc);
    a->SetExponent(te);
    return a;
  }
  else if (a->IsVariable() && t->GetOpType() == PRODUCT)
  {
    // a is a variable, t is a product - see if a appears in t
    // and cancel common term
    // first simplify coeffs of divisor
    t->ConsolidateProductCoeffs();
    // denominator
    if (std::abs(t->GetCoeff()) < Ev3NearZero())
    {
      // divide by zero
      unsigned long mycode(22);
      std::string myif("Expression Building");
      std::string myscope("FractionLink");
      std::string myop("t->GetCoeff()");
      std::string mydesc("Divisor cannot be zero");
      std::string myinfo(HELPURL);
      std::string mydiv(NONE);
      throw ErrDivideByZero(mycode, myif, myscope, myop, mydesc, myinfo, mydiv);
    }
    if (std::abs(a->GetCoeff()) < Ev3NearZero())
    {
      Expression zero(0.0);
      return zero;
    }
    double accumulatedcoeff = a->GetCoeff() / t->GetCoeff();
    a->SetCoeff(1.0);
    t->SetCoeff(1.0);
    // now try simplification
    Int i;
    for (i = 0; i < t->GetSize(); i++)
    {
      if (t->GetNode(i)->GetOpType() == VAR &&
          a->GetVarIndex() == t->GetNode(i)->GetVarIndex())
      {
        double te = a->GetExponent() - t->GetNode(i)->GetExponent();
        if (std::abs(te) < Ev3NearZero())
        {
          // exponents are the same, just cancel
          a->One();
          t->DeleteNode(i);
        }
        else if (te > 0)
        {
          // numerator remains, cancel denominator
          a->SetExponent(te);
          t->DeleteNode(i);
        }
        else if (te < 0)
        {
          // numerator goes to one, denominator remains
          a->One();
          t->GetNode(i)->SetExponent(-te);
        }
        // exit loop
        break;
      }
    }
    // check that denominator (t) has more than one operands;
    // if not, bring up a rank level
    if (t->GetSize() == 1)
    {
      t = t->GetNode(0);
    }
    // build ratio
    Expression ret;
    ret->SetOpType(FRACTION);
    ret->SetCoeff(accumulatedcoeff);
    ret->SetExponent(1.0);
    ret->AddNode(a);
    ret->AddNode(t);
    return ret;
  }
  else if (t->IsVariable() && a->GetOpType() == PRODUCT)
  {
    // t is a variable, a is a product - see if t appears in a
    // and cancel common term
    // first simplify coeffs of divisor
    a->ConsolidateProductCoeffs();
    // denominator - already checked
    if (std::abs(a->GetCoeff()) < Ev3NearZero())
    {
      Expression zero(0.0);
      return zero;
    }
    double accumulatedcoeff = a->GetCoeff() / t->GetCoeff();
    a->SetCoeff(1.0);
    t->SetCoeff(1.0);
    // now try simplification
    Int i;
    for (i = 0; i < a->GetSize(); i++)
    {
      if (a->GetNode(i)->GetOpType() == VAR &&
          t->GetVarIndex() == a->GetNode(i)->GetVarIndex())
      {
        double te = a->GetNode(i)->GetExponent() - t->GetExponent();
        if (std::abs(te) < Ev3NearZero())
        {
          // exponents are the same, just cancel
          t->One();
          a->DeleteNode(i);
        }
        else if (te > 0)
        {
          // numerator remains, cancel denominator
          t->One();
          a->GetNode(i)->SetExponent(te);
        }
        else if (te < 0)
        {
          // numerator goes to one, denominator remains
          t->SetExponent(-te);
          a->DeleteNode(i);
        }
        // exit loop
        break;
      }
    }
    // check that numerator (a) has more than one operands;
    // if not, bring up a rank level
    if (a->GetSize() == 1)
    {
      a = a->GetNode(0);
    }
    // build ratio
    Expression ret;
    ret->SetOpType(FRACTION);
    ret->SetCoeff(accumulatedcoeff);
    ret->SetExponent(1.0);
    ret->AddNode(a);
    ret->AddNode(t);
    return ret;
  }
  else if (a->GetOpType() == PRODUCT && t->GetOpType() == PRODUCT)
  {
    // a, t are products, try to cancel common terms
    Int i = 0, j = 0;
    double accumulatedcoeff;
    // first simplify coefficients of operands
    a->ConsolidateProductCoeffs();
    t->ConsolidateProductCoeffs();
    // denominator
    if (std::abs(t->GetCoeff()) < Ev3NearZero())
      throw ErrDivideByZero(21, "Expression Building", "FractionLink", "t->GetCoeff()", "Divisor cannot be zero", HELPURL, NONE);

    if (std::abs(a->GetCoeff()) < Ev3NearZero())
    {
      Expression zero(0.0);
      return zero;
    }
    // save ratio of coeffs of products
    accumulatedcoeff = a->GetCoeff() / t->GetCoeff();
    a->SetCoeff(1.0);
    t->SetCoeff(1.0);
    // now try simplification
    i = 0;
    bool isnumeratorempty = false;
    bool isdenominatorempty = false;
    int szi = a->GetSize();
    int szj = t->GetSize();
    while(!isnumeratorempty && !isdenominatorempty && i < szi)
    {
      j = 0;
      while(!isnumeratorempty && !isdenominatorempty && j < szj)
      {
        if (a->GetNode(i)->IsEqualTo(t->GetNode(j)))
        {
          // found like terms i and j
          a->DeleteNode(i);
          szi--;
          if(szi == 0)
          {
            isnumeratorempty = true;
            a->One();
          }
          t->DeleteNode(j);
          szj--;
          if (szj == 0)
          {
            isdenominatorempty = true;
            t->One();
          }
          i--;   // cancel the effect of the later i++
          break; // go to outer cycle
        }
        else
        {
          j++;
        }
      }
      i++;
    }
    if (t->HasValue(1))
    {
      // denominator is 1, return a
      a->SetCoeff(accumulatedcoeff);
      return a;
    }
    // now construct fraction
    // check that numerator, denominator have more than one operands;
    // if not, bring up a rank level
    if (a->GetSize() == 1)
    {
      a = a->GetNode(0);
    }
    if (t->GetSize() == 1)
    {
      t = t->GetNode(0);
    }
    Expression ret;
    ret->SetCoeff(accumulatedcoeff); // already contains coeffs of a, t
    ret->SetExponent(1.0);
    ret->SetOpType(FRACTION);
    ret->AddNode(a);
    ret->AddNode(t);
    return ret;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(a->GetCoeff() / t->GetCoeff());
    a->SetCoeff(1.0);
    t->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(FRACTION);
    ret->AddNode(a);
    ret->AddNode(t);
    return ret;
  }
}

// unary minus:
Expression MinusLink(Expression a)
{
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(- a->GetValue());
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
  }
  else
  {
    a->SetCoeff(- a->GetCoeff());
  }
  return a;
}

// binary minus:
Expression DifferenceLink(Expression a,
                          Expression b)
{
  if (a->HasValue(0))
    return MinusLink(b);
  if (b->HasValue(0))
    return a;
  return SumLink(a, MinusLink(b));
}

// power:
Expression PowerLink(Expression a,
                     Expression t)
{
  // make a preliminary check
  if (a->GetCoeff() == 0)
  {
    // *this is zero, just return zero
    Expression zero(0.0);
    return zero;
  }
  if (t->HasValue(0.0))
  {
    // exponent is 0, just return 1
    Expression one(1.0);
    return one;
  }
  else if (t->HasValue(1.0))
  {
    // exponent is 1, just return a
    return a;
  }
  if (a->HasValue(0.0))
  {
    // base is zero, return 0
    Expression zero(0.0);
    return zero;
  }
  else if (a->HasValue(1.0))
  {
    // base is one, return 1
    Expression one(1.0);
    return one;
  }
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST &&
      t->IsLeaf() && t->GetOpType() == CONST)
  {
    // constant to constant
    a->SetValue(pow(a->GetValue(), t->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    return a;
  }
  else if ((std::abs(a->GetCoeff()) == 1.0) && a->IsLeaf() && (a->GetOpType() == VAR) &&
           t->IsLeaf() && (t->GetOpType() == CONST))
  {
    // variable to constant
    a->SetExponent(a->GetExponent() * t->GetValue());
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(POWER);
    ret->AddNode(a);
    ret->AddNode(t);
    return ret;
  }
}

Expression SinLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(sin(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SIN);
    ret->AddNode(a);
    return ret;
  }
}

Expression CosLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(cos(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COS);
    ret->AddNode(a);
    return ret;
  }
}

Expression TanLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(tan(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(TAN);
    ret->AddNode(a);
    return ret;
  }
}

Expression AsinLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(asin(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ASIN);
    ret->AddNode(a);
    return ret;
  }
}

Expression AcosLink(Expression a)
{
  if (a->IsLessThan(-1.0) || a->IsGreaterThan(1.0))
    throw ErrNotPermitted(0, "Expression Building", "AcosLink", "value <-1|>1", "acos(<-1|>1) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(acos(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ACOS);
    ret->AddNode(a);
    return ret;
  }
}

Expression AtanLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(atan(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ATAN);
    ret->AddNode(a);
    return ret;
  }
}

Expression SinhLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(sinh(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SINH);
    ret->AddNode(a);
    return ret;
  }
}

Expression CoshLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(cosh(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COSH);
    ret->AddNode(a);
    return ret;
  }
}

Expression TanhLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(tanh(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(TANH);
    ret->AddNode(a);
    return ret;
  }
}

Expression AsinhLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(asinh(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ASINH);
    ret->AddNode(a);
    return ret;
  }
}

Expression AcoshLink(Expression a)
{
  if (a->IsLessThan(1.0))
    throw ErrNotPermitted(0, "Expression Building", "AcoshLink", "value < 1", "acosh(<1) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(acosh(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ACOSH);
    ret->AddNode(a);
    return ret;
  }
}

Expression AtanhLink(Expression a)
{
  if (a->IsLessThan(-1.0) || a->IsGreaterThan(1.0))
    throw ErrNotPermitted(0, "Expression Building", "AtanhLink", "value <-1|>1", "atanh(<-1|>1) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(atanh(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ATANH);
    ret->AddNode(a);
    return ret;
  }
}

Expression Log2Link(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
    throw ErrNotPermitted(0, "Expression Building", "Log2Link", "IsZero()", "log2(0) is undefined", HELPURL);

  if (a->IsLessThan(0))
    throw ErrNotPermitted(0, "Expression Building", "Log2Link", "value <= 0", "log2(<=0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = a->GetValue();
    assert(t >= 0);
    a->SetCoeff(1.0);
    a->SetValue(log2(t));
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LOG2);
    ret->AddNode(a);
    return ret;
  }
}

Expression Log10Link(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
    throw ErrNotPermitted(0, "Expression Building", "Log10Link", "IsZero()", "log10(0) is undefined", HELPURL);

  if (a->IsLessThan(0))
    throw ErrNotPermitted(0, "Expression Building", "Log10Link", "value <= 0", "log10(<=0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = a->GetValue();
    assert(t >= 0);
    a->SetCoeff(1.0);
    a->SetValue(log10(t));
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LOG10);
    ret->AddNode(a);
    return ret;
  }
}

Expression LogLink(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
    throw ErrNotPermitted(0, "Expression Building", "LogLink", "IsZero()", "log(0) is undefined", HELPURL);

  if (a->IsLessThan(0))
    throw ErrNotPermitted(0, "Expression Building", "LogLink", "value <= 0", "log(<=0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = a->GetValue();
    assert(t >= 0);
    a->SetCoeff(1.0);
    a->SetValue(log(t));
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LOG);
    ret->AddNode(a);
    return ret;
  }
}

Expression LnLink(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
    throw ErrNotPermitted(0, "Expression Building", "LnLink", "IsZero()", "ln(0) is undefined", HELPURL);

  if (a->IsLessThan(0))
    throw ErrNotPermitted(0, "Expression Building", "LnLink", "value <= 0", "ln(<=0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = a->GetValue();
    assert(t >= 0);
    a->SetCoeff(1.0);
    a->SetValue(log(t));
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LN);
    ret->AddNode(a);
    return ret;
  }
}

Expression LngammaLink(Expression a)
{
  if (a->IsLessThan(0.0))
    throw ErrNotPermitted(0, "Expression Building", "LngammaLink", "value < 0", "lngamma(<0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(lgamma(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(LNGAMMA);
    ret->AddNode(a);
    return ret;
  }
}

Expression GammaLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(exp(lgamma(a->GetValue())));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(GAMMA);
    ret->AddNode(a);
    return ret;
  }
}

Expression ExpLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(exp(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(EXP);
    ret->AddNode(a);
    return ret;
  }
}

Expression ErfLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(erf(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ERF);
    ret->AddNode(a);
    return ret;
  }
}

Expression ErfcLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(erfc(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ERFC);
    ret->AddNode(a);
    return ret;
  }
}

Expression SqrtLink(Expression a)
{
  // make a preliminary check
  if (a->IsLessThan(0) && !a->HasValue(0))
    throw ErrNotPermitted(0, "Expression Building", "SqrtLink", "value < 0", "sqrt(<0) is complex, can't do", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = a->GetValue();
    assert(t >= 0.);
    a->SetCoeff(1.0);
    a->SetValue(sqrt(t));
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SQRT);
    ret->AddNode(a);
    return ret;
  }
}

Expression CbrtLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(cbrt(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(CBRT);
    ret->AddNode(a);
    return ret;
  }
}

Expression BesselJ0Link(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(j0(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELJ0);
    ret->AddNode(a);
    return ret;
  }
}

Expression BesselJ1Link(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(j1(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELJ1);
    ret->AddNode(a);
    return ret;
  }
}

Expression BesselY0Link(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(y0(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELY0);
    ret->AddNode(a);
    return ret;
  }
}

Expression BesselY1Link(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(y1(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(BESSELY1);
    ret->AddNode(a);
    return ret;
  }
}

Expression SignLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double tmp(a->GetValue());
    a->SetValue((tmp < 0) ? -1.0 : (tmp > 0) ? 1.0 : 0.0);
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(SIGN);
    ret->AddNode(a);
    return ret;
  }
}

Expression RintLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(floor(0.5 + a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(RINT);
    ret->AddNode(a);
    return ret;
  }
}

Expression AbsLink(Expression a)
{
  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    a->SetValue(std::abs(a->GetValue()));
    a->SetCoeff(1.0);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(ABS);
    ret->AddNode(a);
    return ret;
  }
}

Expression CotLink(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
    throw ErrNotPermitted(0, "Expression Building", "CotLink", "IsZero()", "cot(0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = tan(a->GetValue());
    assert(t != 0);
    a->SetCoeff(1.0);
    a->SetValue(1 / t);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COT);
    ret->AddNode(a);
    return ret;
  }
}

Expression CothLink(Expression a)
{
  // make a preliminary check
  if (a->IsZero())
    throw ErrNotPermitted(0, "Expression Building", "CothLink", "IsZero()", "coth(0) is undefined", HELPURL);

  // go for it
  if (a->IsLeaf() && a->GetOpType() == CONST)
  {
    double t = tanh(a->GetValue());
    assert(t != 0);
    a->SetCoeff(1.0);
    a->SetValue(1 / t);
    a->SetExponent(1.0);
    a->SetOpType(CONST);
    return a;
  }
  else
  {
    Expression ret;
    ret->SetCoeff(1.0);
    ret->SetExponent(1.0);
    ret->SetOpType(COTH);
    ret->AddNode(a);
    return ret;
  }
}


/***************** differentiation ******************/

// differentiate w.r.t. variable varindex
Expression Diff(const Expression& ac, Int vi)
{
  Expression ret(DiffNoSimplify(ac, vi));
  Simplify(&ret);
  return ret;
}

Expression DiffNoSimplify(const Expression& ac, Int vi)
{
  Expression a;
  a.SetToCopyOf(ac);
  Expression zero(0.0);
  Expression c(1.0);
  if (a->DependsOnVariable(vi))
  {
    if (a->IsLeaf())
    {
      if (a->GetOpType() == CONST || a->GetVarIndex() != vi)
      {
        // safety check
        std::cerr << "Expression::Diff: warning: this node should "
                  << "not diff to zero\n";
        return zero;
      }
      else
      {
        // variable vi, check exponent
        if (a->GetExponent() == 0)
        {
          // exponent is zero, node is actually constant
          return zero;
        }
        else if (a->GetExponent() == 1)
        {
          // exponent is one, node is variable
          c->SetValue(a->GetCoeff());

          return c;
        }
        else
        {
          // for all other cases, apply rule x^c = c*x^(c-1)
          double expon = a->GetExponent();
          Expression ret(a.Copy());
          ret->SetExponent(expon - 1);
          ret->SetCoeff(ret->GetCoeff() * expon);
          return ret;
        }
      }
    }
    else
    {

      // non-leaf node. build derivative.
      int op = a->GetOpType();
      Int sz = a->GetSize();
      double opcoeff = a->GetCoeff();
      if (sz == 0)
      {
        throw ErrNotPermitted(10, "Expression", "Diff", "GetSize() == 0",
                              "non-leaf node can't have size 0", HELPURL);
      }
      Int i, j;
      Expression ret(0.0);
      Expression tmp(1.0);
      Expression tmp2(1.0);
      Expression two(2.0);
      switch(op)
      {
        case SUM:
          ret = Diff(a->GetNode(0), vi); // f_0'
          for(i = 1; i < sz; i++)
          {
            tmp = Diff(a->GetNode(i), vi);
            if (!tmp->IsZero())
            {
              ret = ret + tmp; // ... + g_i'
            }
          }
          break;
        case DIFFERENCE:
          ret = Diff(a->GetNode(0), vi);  // f_0'
          for(i = 1; i < sz; i++)
          {
            tmp = Diff(a->GetNode(i), vi);
            if (!tmp->IsZero())
            {
              ret = ret - tmp;  // ... - g_i'
            }
          }
          break;
        case PRODUCT:
          if (sz == 1)
          {
            // warn about product with one operand
            std::cerr << "Expression::Diff: warning: product with 1 operand "
                      << "should not occur\n";
          }
          ret = Diff(a->GetNode(0), vi);  // f_0'
          if (!ret->IsZero())
          {
            for(j = 1; j < sz; j++)
            {
              // get copies, not references
              ret = ret * a->GetCopyOfNode(j); // ... * f_i[i!=0]
            }
          }
          tmp->One(); // reset temporary to 1.0
          for(i = 1; i < sz; i++)
          {
            tmp = Diff(a->GetNode(i), vi); // tmp = f_i'
            if (!tmp->IsZero())
            {
              for(j = 0; j < sz; j++)
              {
                if (j != i)
                {
                  // get references, and copy later (in sum)
                  tmp = tmp * a->GetNode(j); // ... * f_j[i!=i]
                }
              }
              ret = ret + tmp.Copy();  // ... + tmp
              tmp->One(); // reset temporary to 1.0
            }
          }
          break;
        case FRACTION:
          if (sz != 2)
          {
            // check that there are exactly two operands
            throw ErrNotPermitted(11, "Expression", "Diff", "GetSize() != 2",
                                  "fraction must have exactly 2 operands",
                                  HELPURL);
          }
          if (a->GetNode(1)->IsZero())
          {
            // check denominator is not zero
            throw ErrDivideByZero(20, "Expression", "Diff",
                                  "GetNode(1)->IsZero()",
                                  "cannot divide by zero", HELPURL,
                                  a->GetNode(1)->ToString());
          }
          tmp->One();
          ret = Diff(a->GetNode(0), vi); // f'
          if (!ret->IsZero())
          {
            ret = ret / a->GetCopyOfNode(1);  // f'/g
          }
          // can dispense from using GetCopyOf because tmp gets copied anyway
          tmp = a->GetNode(0);           // tmp = f
          tmp2 = Diff(a->GetNode(1), vi);
          if (!tmp2->IsZero())
          {
            tmp = tmp * tmp2;  // tmp = fg'
            ret = ret - tmp.Copy() / (a->GetCopyOfNode(1) ^ two);  // f'/g - fg'/g^2
          }
          // can dispense from using copy here - tmp is not used thereafter
          // and when tmp is deleted, its subnodes are not automatically
          // deleted unless reference counter is zero - which won't be.
          break;
        case POWER:
          if (sz != 2)
          {
            // check that there are exactly two operands
            throw ErrNotPermitted(12, "Expression", "Diff", "GetSize() != 2",
                                  "power must have exactly 2 operands",
                                  HELPURL);
          }
          // check exponent
          if (a->GetNode(1)->IsZero())
          {
            // exponent is zero, whole node is one, diff is zero
            ret->Zero();
          }
          else if (a->GetNode(1)->HasValue(1.0))
          {
            // exponent is one, whole node is first operand, diff
            // is diff of first operand
            ret = Diff(a->GetNode(0), vi);
          }
          else if (a->GetNode(1)->HasValue(2.0))
          {
            // exponent is two, diff is 2 * first op * diff of first operand
            ret = Diff(a->GetNode(0), vi);  // f'
            ret = ret * a->GetCopyOfNode(0);   // f'f
            ret->SetCoeff(ret->GetCoeff() * 2.0);  // 2f'f
          }
          else
          {
            // all other cases
            if (a->GetNode(1)->IsConstant())
            {
              // numeric exponent != 0,1,2
              ret = Diff(a->GetNode(0), vi); // f'
              tmp = a->GetCopyOfNode(0);     // f
              const double coeff = tmp->GetCoeff();
              tmp = tmp ^ a->GetCopyOfNode(1);  // f^c
              if (tmp->GetOpType() == VAR)
              {
                tmp->SetCoeff(tmp->GetExponent() * std::pow(coeff, tmp->GetExponent() - 1.0)); // cf^c
                tmp->SetExponent(tmp->GetExponent() - 1); // cf^(c-1)
              }
              else
              {
                tmp->GetNode(1)->ConsolidateValue();
                tmp->SetCoeff(tmp->GetCoeff() * tmp->GetNode(1)->GetValue());//cf^c
                tmp->GetNode(1)->SetValue(tmp->GetNode(1)->GetValue() - 1); //cf^(c-1)
              }
              // can dispense from using copy here - Diff returns copies anyway.
              // when temporary is deleted, its subnodes are not automatically
              // deleted unless their reference counter is zero - which won't be.
              ret = ret * tmp; // f'(cf^(c-1))
            }
            else
            {
              // symbolic exponent f^g
              ret = a->GetCopyOfNode(0); // f
              ret = Log(ret); // log(f)
              // can dispense from using copy here - Diff returns copies anyway.
              // when temporary is deleted, its subnodes are not automatically
              // deleted unless their reference counter is zero - which won't be.
              ret = ret * Diff(a->GetNode(1), vi); // g' log(f)
              tmp = Diff(a->GetNode(0), vi);  // f'
              tmp = tmp * a->GetCopyOfNode(1);   // gf'
              tmp = tmp / a->GetCopyOfNode(0);    // gf'/f
              // can dispense from using copy here - tmp is not used thereafter
              // and when tmp is deleted, its subnodes are not automatically
              // deleted unless their reference counter is zero - which won't be.
              ret = ret + tmp;           // g'log(f) + gf'/f
              tmp = a.Copy();
              tmp->SetCoeff(1.0);
              ret = ret * tmp;        // (g'log(f) + gf'/f)(f^g)
            }
          }
          break;
        case MINUS:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(13, "Expression", "Diff", "GetSize() != 1",
                                  "unary minus must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi);
          ret->SetCoeff(- ret->GetCoeff());
          break;
        case SIN:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(17, "Expression", "Diff", "GetSize() != 1",
                                  "sin must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi) * Cos(a->GetCopyOfNode(0));  // f' cos(f)
          break;
        case COS:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(18, "Expression", "Diff", "GetSize() != 1",
                                  "cos must have exactly 1 operand",
                                  HELPURL);
          }
          ret = -Diff(a->GetNode(0), vi) * Sin(a->GetCopyOfNode(0)); // -f'sin(f)
          break;
        case TAN:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(19, "Expression", "Diff", "GetSize() != 1",
                                  "tan must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a.Copy();  // tan(f)
          ret->SetCoeff(1.0);
          ret = ret ^ two;      // tan(f)^2
          c->One();
          ret = ret + c;         // tan(f)^2 + 1
          ret = ret * Diff(a->GetNode(0), vi);    // f' * (tan(f)^2 + 1)
          break;
        case ASIN:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(17, "Expression", "Diff", "GetSize() != 1",
                                  "asin must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = ret ^ two;
          c->One();
          ret = c - ret;
          ret = Diff(a->GetNode(0), vi) / Sqrt(ret); // f' / sqrt(1 - f^2)
          break;
        case ACOS:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(18, "Expression", "Diff", "GetSize() != 1",
                                  "acos must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = ret ^ two;
          c->One();
          ret = c - ret;
          ret = -Diff(a->GetNode(0), vi) / Sqrt(ret); // -f' / sqrt(1 - f^2)
          break;
        case ATAN:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(19, "Expression", "Diff", "GetSize() != 1",
                                  "atan must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = ret ^ two;
          c->One();
          ret = ret + c;
          ret = Diff(a->GetNode(0), vi) / ret; // f' / (f^2 + 1)
          break;
        case SINH:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(17, "Expression", "Diff", "GetSize() != 1",
                                  "sinh must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi) * Cosh(a->GetCopyOfNode(0));  // f' cosh(f)
          break;
        case COSH:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(18, "Expression", "Diff", "GetSize() != 1",
                                  "cosh must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi) * Sinh(a->GetCopyOfNode(0)); // f'sinh(f)
          break;
        case TANH:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(19, "Expression", "Diff", "GetSize() != 1",
                                  "tanh must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a.Copy();
          ret->SetCoeff(1.0);
          ret = ret ^ two;
          c->One();
          ret = c - ret;
          ret = ret * Diff(a->GetNode(0), vi);    // f' * (1 - tan(f)^2)
          break;
        case ASINH:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(17, "Expression", "Diff", "GetSize() != 1",
                                  "asinh must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = ret ^ two;
          c->One();
          ret = c + ret;
          ret = Diff(a->GetNode(0), vi) / Sqrt(ret); // f' / sqrt(1 + f^2)
          break;
        case ACOSH:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(18, "Expression", "Diff", "GetSize() != 1",
                                  "acosh must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = ret ^ two;
          c->One();
          ret = ret - c;
          ret = Diff(a->GetNode(0), vi) / Sqrt(ret); // f' / sqrt(f^2 - 1)
          break;
        case ATANH:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(19, "Expression", "Diff", "GetSize() != 1",
                                  "atanh must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = ret ^ two;
          c->One();
          ret = c - ret;
          ret = Diff(a->GetNode(0), vi) / ret;    // f' / (1 - tan(f)^2)
          break;
        case LOG2:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(14, "Expression", "Diff", "GetSize() != 1",
                                  "log2 must have exactly 1 operand",
                                  HELPURL);
          }
          if (a->GetNode(0)->IsLessThan(0))
          {
            throw ErrNotPermitted(15, "Expression", "Diff", "arg <= 0",
                                  "log2 argument must be symbolic or positive",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi);
          ret = ret / (Expression(M_LN2) * a->GetCopyOfNode(0));  // f'/(log(2)f)
          break;
        case LOG10:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(14, "Expression", "Diff", "GetSize() != 1",
                                  "log10 must have exactly 1 operand",
                                  HELPURL);
          }
          if (a->GetNode(0)->IsLessThan(0))
          {
            throw ErrNotPermitted(15, "Expression", "Diff", "arg <= 0",
                                  "log10 argument must be symbolic or positive",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi);
          ret = ret / (Expression(M_LN10) * a->GetCopyOfNode(0));  // f'/(log(10)f)
          break;
        case LOG:
        case LN:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(14, "Expression", "Diff", "GetSize() != 1",
                                  "log (ln) must have exactly 1 operand",
                                  HELPURL);
          }
          if (a->GetNode(0)->IsLessThan(0))
          {
            throw ErrNotPermitted(15, "Expression", "Diff", "arg <= 0",
                                  "log (ln) argument must be symbolic or positive",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi);
          ret = ret / a->GetCopyOfNode(0);  // f'/f
          break;
        case EXP:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "exp must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi) * Exp(a->GetCopyOfNode(0));  // f'exp(f)
          break;
        case ERF:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "erf must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Expression(M_2_SQRTPI) * Diff(a->GetCopyOfNode(0), vi) * Exp(-(a->GetCopyOfNode(0) ^ two)); // (2/sqrt(pi)) f'e^(-f^2)
          break;
        case ERFC:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "erfc must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Expression(-M_2_SQRTPI) * Diff(a->GetCopyOfNode(0), vi) * Exp(-(a->GetCopyOfNode(0) ^ two)); // (2/sqrt(pi)) f'e^(-f^2)
          break;
        case SQRT:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(19, "Expression", "Diff", "GetSize() != 1",
                                  "sqrt must have exactly 1 operand",
                                  HELPURL);
          }
          if (a->GetNode(0)->IsLessThan(0))
          {
            throw ErrNotPermitted(15, "Expression", "Diff", "arg < 0",
                                  "sqrt argument must be symbolic or positive",
                                  HELPURL);
          }
          ret = Expression(0.5) * Diff(a->GetNode(0), vi) / Sqrt(a->GetCopyOfNode(0)); // 1/2 * f' / sqrt(f)
          break;
        case CBRT:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "cbrt must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Expression(1.0 / 3.0) * Diff(a->GetNode(0), vi) / (a->GetCopyOfNode(0) ^ Expression(2.0 / 3.0)); // (1/3) f' / f^(2/3)
          break;
        case BESSELJ0:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "besselJ0 must have exactly 1 operand",
                                  HELPURL);
          }
          ret = -Diff(a->GetNode(0), vi) * BesselJ1(a->GetCopyOfNode(0)); // -f'besselJ1(f)
          break;
        case BESSELJ1:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "besselJ1 must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = BesselJ0(ret) - BesselJ1(ret) / ret;
          ret = Diff(a->GetNode(0), vi) * ret; // f'(besselJ0(f) - besselJ1(f) / f)
          break;
        case BESSELY0:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "besselY0 must have exactly 1 operand",
                                  HELPURL);
          }
          ret = -Diff(a->GetNode(0), vi) * BesselY1(a->GetCopyOfNode(0)); // -f'besselY1(f)
          break;
        case BESSELY1:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "besselY1 must have exactly 1 operand",
                                  HELPURL);
          }
          ret = a->GetCopyOfNode(0);
          ret = BesselY0(ret) - BesselY1(ret) / ret;
          ret = Diff(a->GetNode(0), vi) * ret; // f'(besselY0(f) - besselY1(f) / f)
          break;
        case SIGN:
        case RINT:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "sign (rint) must have exactly 1 operand",
                                  HELPURL);
          }
          ret = zero; // 0
          break;
        case ABS:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(16, "Expression", "Diff", "GetSize() != 1",
                                  "abs must have exactly 1 operand",
                                  HELPURL);
          }
          ret = Diff(a->GetNode(0), vi) * Sign(a->GetCopyOfNode(0)); // f'sign(f)
          break;
        default:
          if (sz != 1)
          {
            // check that there is exactly one operand
            throw ErrNotPermitted(14, "Expression", "Diff", "GetSize() != 1",
                                  "log must have exactly 1 operand",
                                  HELPURL);
          }
          throw ErrNotPermitted(16, "Expression", "Diff", "not implemented",
                                "The derivative of the function is not implemented.",
                                HELPURL);
          break;
      }
      ret->SetCoeff(ret->GetCoeff() * opcoeff);
      return ret;
    }
  }
  else
  {
    return zero;
  }
  return zero;
}

/************************ simplifications **********************/

bool TrigSimp(Expression a)
{
  Int i = 0;
  Int ret = 0;
  bool bret = false;
  bool ischanged = false;
  // first, recurse over all subnodes of a
  for(i = 0; i < a->GetSize(); i++)
  {
    ischanged = TrigSimp(a->GetNode(i));
    if (!bret && ischanged)
    {
      bret = true;
    }
  }
  // now try simplification on a
  if (a->GetOpType() == SUM && a->GetSize() > 1)
  {
    // try to look for a sin^2 and a cos^2
    Int sinpos = -1;
    Int cospos = -1;
    Int sinpossimple = -1;
    Int cospossimple = -1;
    for (i = 0; i < a->GetSize(); i++)
    {
      // cycle over subnodes
      if (a->GetNode(i)->GetOpType() == POWER)
      {
        if (a->GetNode(i)->GetNode(0)->GetOpType() == SIN &&
            a->GetNode(i)->GetNode(1)->HasValue(2))
          sinpos = i;
      }
      if (a->GetNode(i)->GetOpType() == POWER)
      {
        if (a->GetNode(i)->GetNode(0)->GetOpType() == COS &&
            a->GetNode(i)->GetNode(1)->HasValue(2))
          cospos = i;
      }
      if (a->GetNode(i)->GetOpType() == SIN &&
          a->GetNode(i)->GetExponent() == 2)
      {
        sinpossimple = i;
      }
      if (a->GetNode(i)->GetOpType() == COS &&
          a->GetNode(i)->GetExponent() == 2)
      {
        cospossimple = i;
      }
    }

    if (sinpos != -1 && cospos != -1)
    {
      double coscoeff = a->GetNode(sinpos)->GetCoeff();
      double sincoeff = a->GetNode(cospos)->GetCoeff();

      // found both, check their arguments
      if ((coscoeff == sincoeff) &&
          a->GetNode(sinpos)->GetNode(0)->GetNode(0)->IsEqualTo
          (a->GetNode(cospos)->GetNode(0)->GetNode(0)))
      {
        ret++; // augment simplification counter
        bret = true;
        // arguments are equal, can simplify
        Int f = sinpos < cospos ? sinpos : cospos; // last to delete
        Int l = sinpos > cospos ? sinpos : cospos; // first to delete
        a->DeleteNode(l);
        a->DeleteNode(f);
        // verify that there are still some nodes left
        if (a->GetSize() == 0)
        {
          // there aren't any, set a to one
          a->One();
          a->SetCoeff(coscoeff);
        }
        else
        {
          // there are some, check whether there is a constant part
          // we can add the 1 to
          bool addflag = false;
          for (i = 0; i < a->GetSize(); i++)
          {
            if (a->GetNode(i)->IsConstant())
            {
              // yes there is
              a->GetNode(i)->SetValue(a->GetNode(i)->GetSimpleValue() + 1);
              addflag = true;
              break;
            }
          }
          if (!addflag)
          {
            // no there wasn't, add it as a symbolic node
            Expression coeffexpr(coscoeff);
            a->AddNode(coeffexpr);
          }
          // check that there is more than just one node
          if (a->GetSize() == 1)
          {
            // only one node, shift everything one rank level up
            a = a->GetNode(0);
          }
        }
      }
    }
    if (sinpossimple != -1 && cospossimple != -1)
    {
      double coscoeff = a->GetNode(sinpossimple)->GetCoeff();
      double sincoeff = a->GetNode(cospossimple)->GetCoeff();

      if ((coscoeff == sincoeff) &&
          a->GetNode(sinpossimple)->GetNode(0)->IsEqualTo
          (a->GetNode(cospossimple)->GetNode(0)))
      {
        ret++;
        bret = true;
        // arguments are equal, can simplify
        Int f = sinpossimple < cospossimple ? sinpossimple : cospossimple;
        Int l = sinpossimple > cospossimple ? sinpossimple : cospossimple;
        a->DeleteNode(l);
        a->DeleteNode(f);
        // verify that there are still some nodes left
        if (a->GetSize() == 0)
        {
          // there aren't any, set a to one
          a->One();
          a->SetCoeff(coscoeff);
        }
        else
        {
          // there are some, check whether there is a constant part
          // we can add the 1 to
          bool addflag = false;
          for (i = 0; i < a->GetSize(); i++)
          {
            if (a->GetNode(i)->IsConstant())
            {
              // yes there is
              a->GetNode(i)->SetValue(a->GetNode(i)->GetSimpleValue() + 1);
              addflag = true;
              break;
            }
          }
          if (!addflag)
          {
            // no there wasn't, add it as a symbolic node
            Expression coeffexpr(coscoeff);
            a->AddNode(coeffexpr);
          }
          // check that there is more than just one node
          if (a->GetSize() == 1)
          {
            // only one node, shift everything one rank level up
            a = a->GetNode(0);
          }
        }
      }
    }
  }
  if (ret > 0)
    bret = true;
  return bret;
}

// generic simplification with modification of the expression
bool Simplify(Expression* a)
{
  bool changedflag = false;
  bool ret = false;
  bool goonflag = true;
  while(goonflag)
  {
    goonflag = false;
    (*a)->ConsolidateProductCoeffs();
    (*a)->DistributeCoeffOverSum();
    ret = DifferenceToSum(a);
    if (ret)
    {
      changedflag = true;
      goonflag = true;
    }
    ret = SimplifyConstant(a);
    if (ret)
    {
      changedflag = true;
      goonflag = true;
    }
    ret = CompactProducts(a);
    if (ret)
    {
      changedflag = true;
      goonflag = true;
    }
    ret = CompactLinearPart(a);
    if (ret)
    {
      changedflag = true;
      goonflag = true;
    }
    ret = SimplifyRecursive(a);
    if (ret)
    {
      changedflag = true;
      goonflag = true;
    }
    ret = TrigSimp(*a);
    if (ret)
    {
      changedflag = true;
      goonflag = true;
    }
  }
  return changedflag;
}

// call after DifferenceToSum
bool SimplifyConstant(Expression* a)
{
  bool ret = false;
  Expression one(1.0);
  Expression zero(0.0);
  if ((*a)->GetExponent() == 0)
  {
    double c = (*a)->GetCoeff();
    RecursiveDestroy(a);
    a->SetTo(one);
    (*a)->SetCoeff(c);
    ret = true;
  }
  else if ((*a)->GetCoeff() == 0)
  {
    RecursiveDestroy(a);
    a->SetTo(zero);
    ret = true;
  }
  else
  {
    int i = 0;
    int op, ops;
    op = (*a)->GetOpType();
    bool ischanged = false;
    int sz = (*a)->GetSize();
    while (i < sz)
    {
      // simplify each of the terms
      ischanged = SimplifyConstant((*a)->GetNodePtr(i));
      if (!ret && ischanged)
      {
        ret = true;
      }
      i++;
    }
    i = 0;
    while (i < sz)
    {
      // simplify this operand as a whole
      ops = (*a)->GetNode(i)->GetOpType();
      switch(op)
      {
        case SUM:
          if (ops == CONST)
          {
            if ((*a)->GetNode(i)->GetValue() == 0)
            {
              (*a)->DeleteNode(i);
              ret = true;
              sz--;
              if (sz == 1)
              {
                a->SetTo((*a)->GetNode(1 - i));
                i = 0;
                sz = (*a)->GetSize();
              }
            }
            else
            {
              i++;
            }
          }
          else
          {
            i++;
          }
          break;
        case PRODUCT:
          if (ops == CONST)
          {
            if ((*a)->GetNode(i)->GetValue() == 1)
            {
              (*a)->DeleteNode(i);
              ret = true;
              sz--;
              if (sz == 1)
              {
                a->SetTo((*a)->GetNode(1 - i));
                i = 0;
                sz = (*a)->GetSize();
              }
            }
            else if ((*a)->GetNode(i)->GetValue() == 0)
            {
              RecursiveDestroy(a);
              ret = true;
              a->SetTo(zero);
              sz = 0;
            }
            else
            {
              i++;
            }
          }
          else
          {
            i++;
          }
          break;
        case FRACTION:
          if (ops == CONST && i == 1)
          {
            double c = (*a)->GetCoeff();
            a->SetTo((*a)->GetNode(0));
            (*a)->SetCoeff((*a)->GetCoeff() * c);
            ret = true;
            sz--;
          }
          else
          {
            i++;
          }
          if (sz >= 2 && (*a)->GetNode(0)->IsConstant() &&
              (*a)->GetNode(1)->IsConstant())
          {
            double d = (*a)->GetNode(1)->GetValue();
            if (d == 0)
            {
              throw ErrDivideByZero(23, "Expression", "SimplifyConstant",
                                    "d==0",
                                    "cannot divide by zero", HELPURL,
                                    (*a)->ToString());
            }
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue((*a)->GetValue() / d);
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          break;
        case POWER:
          if (sz >= 2 && (*a)->GetNode(0)->IsConstant() &&
              (*a)->GetNode(1)->IsConstant())
          {
            double d = (*a)->GetNode(1)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(pow((*a)->GetValue(), d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case LOG:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            if (d <= 0)
            {
              throw ErrNotPermitted(24, "Expression", "SimplifyConstant",
                                    "d<=0",
                                    "log of nonpositive not allowed", HELPURL);
            }
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(log(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case EXP:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(exp(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case SIN:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(sin(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case COS:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(cos(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case TAN:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(tan(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case COT:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(1 / tan(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case SINH:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(sinh(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case COSH:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(cosh(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case TANH:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(tanh(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case COTH:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(1 / tanh(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        case SQRT:
          if ((*a)->GetNode(0)->IsConstant())
          {
            double d = (*a)->GetNode(0)->GetValue();
            if (d <= 0)
            {
              throw ErrNotPermitted(25, "Expression", "SimplifyConstant",
                                    "d<=0",
                                    "sqrt of nonpositive not allowed", HELPURL);
            }
            ret = true;
            a->SetTo((*a)->GetNode(0));
            (*a)->SetValue(sqrt(d));
            (*a)->SetCoeff(1.0);
            (*a)->SetExponent(1.0);
            sz = 0;
          }
          else
          {
            i++;
          }
          break;
        default:
          i++;
          break;
      }
    }
  }
  return ret;
}

bool SimplifyRecursive(Expression* a)
{
  bool ret = false;
  bool ischanged = false;
  // signals whether we're dealing with
  // -1 : nothing
  // 0 : constants
  // 1 : linear variables
  // 2 : a variable raised to an exponent different from 1
  // 3 : any other more complex node
  if (!(*a)->IsLeaf())
  {
    int op = (*a)->GetOpType();
    Expression t1, t2;
    int i, j;
    for(i = 0; i < (*a)->GetSize(); i++)
    {
      ischanged = SimplifyRecursive((*a)->GetNodePtr(i));
      if (!ret && ischanged)
        ret = true;
    }
    int status = -1;
    int prestatus = -1;
    double consolidated[4] = {0, 0, 0, 0};
    double expon = 0;
    double preexpon = 0;
    double c = 0;
    int prevarindex = -1;
    int prevarpowindex = -1;
    int varindex = -1;
    int varpowindex = -1;
    int firstvarindex = -1;
    int firstvarpowindex = -1;
    int firstconstindex = -1;
    int sz = (*a)->GetSize();
    Expression one(1.0);

    switch(op)
    {
      case SUM:
        i = 0;
        while(i < sz)
        {
          // work out which status we're in
          if ((*a)->GetNode(i)->IsConstant())
          {
            if (status == -1 || firstconstindex == -1)
            {
              firstconstindex = i;
            }
            // constant
            status = 0;
          }
          else if ((*a)->GetNode(i)->IsVariable() &&
                   (*a)->GetNode(i)->GetExponent() == 1)
          {
            // variable
            status = 1;
          }
          else if ((*a)->GetNode(i)->IsVariable() &&
                   (*a)->GetNode(i)->GetExponent() != 1)
          {
            // variable raised to power
            status = 2;
          }
          else
          {
            // other term
            status = 3;
          }
          if (status == 0)
          {
            // constant
            consolidated[status] += (*a)->GetNode(i)->GetValue();
            (*a)->GetNode(firstconstindex)->SetValue(consolidated[status]);
            (*a)->GetNode(firstconstindex)->SetCoeff(1.0);
            (*a)->GetNode(firstconstindex)->SetExponent(1.0);
            if (prestatus == 0)
            {
              (*a)->DeleteNode(i);
              ret = true;
              sz--;
              if (sz == 1)
              {
                a->SetTo((*a)->GetNode(0));
                i = 0;
                sz = (*a)->GetSize();
              }
            }
            else
            {
              i++;
            }
          }
          else if (status == 1)
          {
            // variable
            varindex = (*a)->GetNode(i)->GetVarIndex();
            c = (*a)->GetNode(i)->GetCoeff();
            if (varindex != prevarindex)
            {
              firstvarindex = i;
              consolidated[status] = c;
              i++;
            }
            else
            {
              consolidated[status] += c;
              (*a)->GetNode(firstvarindex)->SetCoeff(consolidated[status]);
              ret = true;
              (*a)->DeleteNode(i);
              sz--;
              if (sz == 1)
              {
                a->SetTo((*a)->GetNode(0));
                i = 0;
                sz = (*a)->GetSize();
              }
            }
            prevarindex = varindex;
          }
          else if (status == 2)
          {
            // variable raised to power
            varpowindex = (*a)->GetNode(i)->GetVarIndex();
            expon = (*a)->GetNode(i)->GetExponent();
            c = (*a)->GetNode(i)->GetCoeff();
            if (expon != preexpon || varpowindex != prevarpowindex)
            {
              firstvarpowindex = i;
              consolidated[status] = c;
              i++;
            }
            else
            {
              consolidated[status] += c;
              (*a)->GetNode(firstvarpowindex)->SetCoeff(consolidated[status]);
              ret = true;
              (*a)->DeleteNode(i);
              sz--;
              if (sz == 1)
              {
                a->SetTo((*a)->GetNode(0));
                i = 0;
                sz = (*a)->GetSize();
              }
            }
            preexpon = expon;
            prevarpowindex = varpowindex;
          }
          else if (status == 3)
          {
            // other term
            c = (*a)->GetNode(i)->GetCoeff();
            firstvarindex = i;
            consolidated[status] = c;
            j = i + 1;
            while(j < sz)
            {
              if ((*a)->GetNode(i)->IsEqualToNoCoeff((*a)->GetNode(j)))
              {
                c = (*a)->GetNode(j)->GetCoeff();
                consolidated[status] += c;
                ret = true;
                (*a)->GetNode(firstvarindex)->SetCoeff(consolidated[status]);
                RecursiveDestroy((*a)->GetNodePtr(j));
                (*a)->DeleteNode(j);
                sz--;
                if (sz == 1)
                {
                  a->SetTo((*a)->GetNode(0));
                  j = i + 1;
                  sz = (*a)->GetSize();
                }
              }
              else
              {
                j++;
              }
            }
            i++;
          }
          else
          {
            // should never happen, but anyway...
            i++;
          }
          // update status of last iteration
          prestatus = status;
        }
        break;
      case PRODUCT:
        i = 0;
        prevarindex = -1;
        consolidated[0] = 1;
        expon = 0;
        while(i < sz)
        {
          if ((*a)->GetNode(i)->IsVariable())
          {
            varindex = (*a)->GetNode(i)->GetVarIndex();
            if (varindex != prevarindex)
            {
              firstvarindex = i;
              consolidated[0] = (*a)->GetNode(i)->GetCoeff();
              expon = (*a)->GetNode(i)->GetExponent();
              i++;
            }
            else
            {
              consolidated[0] *= (*a)->GetNode(i)->GetCoeff();
              expon += (*a)->GetNode(i)->GetExponent();
              (*a)->GetNode(firstvarindex)->SetCoeff(consolidated[0]);
              (*a)->GetNode(firstvarindex)->SetExponent(expon);
              (*a)->DeleteNode(i);
              ret = true;
              sz--;
              if (sz == 1)
              {
                a->SetTo((*a)->GetNode(0));
                i = 0;
                sz = (*a)->GetSize();
              }
            }
          }
          else if (!(*a)->GetNode(i)->IsLeaf())
          {
            // WARNING: work to be done
            // not going to do the same as in sum just yet, maybe future
            // work - transform expr * expr in expr^2 when expr not a variable
            i++;
          }
        }
        break;
      case FRACTION:
        if ((*a)->GetNode(0)->IsEqualTo((*a)->GetNode(1)))
        {
          // f(x)/f(x)
          c = (*a)->GetCoeff();
          RecursiveDestroy(a);
          a->SetTo(Expression(c));
          ret = true;
          sz = 0;
        }
        else
        {
          // try to simplify denominator by one of numerator factors
          if ((*a)->GetNode(0)->GetOpType() == PRODUCT)
          {
            for(j = 0; j < (*a)->GetNode(0)->GetSize(); j++)
            {
              if ((*a)->GetNode(1)->IsEqualTo((*a)->GetNode(0)->GetNode(j)))
              {
                c = (*a)->GetCoeff();
                a->SetTo((*a)->GetNode(0));
                (*a)->SetCoeff((*a)->GetCoeff() * c);
                (*a)->DeleteNode(j);
                ret = true;
                sz = 0;
                break;
              }
            }
          }
          // do the opposite
          if (sz > 0 && (*a)->GetNode(1)->GetOpType() == PRODUCT)
          {
            for(j = 0; j < (*a)->GetNode(1)->GetSize(); j++)
            {
              if ((*a)->GetNode(0)->IsEqualTo((*a)->GetNode(1)->GetNode(j)))
              {
                *((*a)->GetNodePtr(0)) = Pointer<BasicExpression>(one);
                (*a)->GetNode(1)->DeleteNode(j);
                ret = true;
                sz = 0;
                break;
              }
            }
          }
          if (sz > 0 && (*a)->GetNode(0)->GetOpType() == PRODUCT &&
              (*a)->GetNode(1)->GetOpType() == PRODUCT)
          {
            // both num and denom. are products, try and find common factors
            int k = 0;
            int sz1, sz2;
            j = 0;
            sz1 = (*a)->GetNode(0)->GetSize();
            sz2 = (*a)->GetNode(1)->GetSize();
            while (j < sz1)
            {
              k = 0;
              while (k < sz2)
              {
                if ((*a)->GetNode(0)->GetNode(j)->IsEqualTo
                    ((*a)->GetNode(1)->GetNode(k)))
                {
                  (*a)->GetNode(0)->DeleteNode(j);
                  (*a)->GetNode(1)->DeleteNode(k);
                  ret = true;
                  sz1--;
                  if (sz1 == 0)
                  {
                    // numerator empty, replace with 1
                    (*a)->GetNode(0)->One();
                  }
                  sz2--;
                  if (sz2 == 0)
                  {
                    // denominator empty, node becomes num.
                    a->SetTo((*a)->GetNode(0));
                  }
                  if (sz1 == 0 && sz2 == 0)
                  {
                    // 1/1, simplify
                    (*a)->One();
                  }
                  if (sz1 == 0 || sz2 == 0)
                  {
                    // either num. or den. 1, exit loop
                    sz1 = 0;
                    sz2 = 0;
                    break;
                  }
                  j--;
                }
                else
                {
                  k++;
                }
              }
              j++;
            }
          }
        }
        sz = 0;
        break;
      case POWER:
        if (sz == 2 &&
            (*a)->GetNode(0)->IsVariable() &&
            (*a)->GetNode(1)->IsConstant())
        {
          // case var^const, transform in variable with an exponent
          expon = (*a)->GetNode(1)->GetValue();
          c = (*a)->GetCoeff();
          double c0 = (*a)->GetNode(0)->GetCoeff();
          (*a)->GetNode(0)->SetExponent(expon);
          (*a)->DeleteNode(1);
          a->SetTo((*a)->GetNode(0));
          (*a)->SetCoeff(c * pow(c0, expon));
        }
        break;
      default:
        break;
    }
  }
  return ret;
}

bool DifferenceToSum(Expression* a)
{
  bool ret = false;
  double d = 0., e = 0.;
  if (!(*a)->IsLeaf())
  {
    if (((*a)->GetOpType() == SUM || (*a)->GetOpType() == DIFFERENCE) &&
        (*a)->GetSize() == 1)
    {
      DifferenceToSum((*a)->GetNodePtr(0));
      // replace a with its child
      a->SetTo((*a)->GetNode(0));
      ret = true;
    }
    if ((*a)->GetOpType() == DIFFERENCE)
    {
      int i;
      (*a)->SetOpType(SUM);
      for(i = 1; i < (*a)->GetSize(); i++)
      {
        // start from node 1 not 0 because a difference is +op0 -op1 -op2 ...
        (*a)->GetNode(i)->SetCoeff(- (*a)->GetNode(i)->GetCoeff());
      }
    }
    else if ((*a)->GetOpType() == MINUS)
    {
      d = (*a)->GetCoeff();
      e = (*a)->GetExponent();
      if (is_even(e))
      {
        // replace a with its child and adjust coeff
        a->SetTo((*a)->GetNode(0));
        (*a)->SetCoeff((*a)->GetCoeff() * d); // since exponent is even, +
        (*a)->SetExponent((*a)->GetExponent() * e);
        ret = true;
      }
      else if (is_odd(e))
      {
        // replace a with its child and adjust coeff
        a->SetTo((*a)->GetNode(0));
        (*a)->SetCoeff(- (*a)->GetCoeff() * d); // since exponent is odd, -
        (*a)->SetExponent((*a)->GetExponent() * e);
        ret = true;
      }
    }
    else if ((*a)->GetOpType() == PLUS)
    {
      // replace a with its child
      a->SetTo((*a)->GetNode(0));
      (*a)->SetCoeff((*a)->GetCoeff() * d); // since exponent is even, +
      (*a)->SetExponent((*a)->GetExponent() * e);
      ret = true;
    }
  }
  return ret;
}

// standard order for a set of subnodes of a sum is:
// constants + linear variables + vars^{rising powers} + complex operands
class NodeOrderSum
{
public:
  int operator() (const Expression& a,
                  const Expression& b)
  {
    if (a->IsConstant() && !b->IsConstant())
    {
      return true;
    }
    else if (a->IsVariable() && b->IsVariable())
    {
      if (a->GetExponent() == 1 && b->GetExponent() != 1)
      {
        return true;
      }
      else if (a->GetExponent() < b->GetExponent())
      {
        return true;
      }
      else if (a->GetExponent() > b->GetExponent())
      {
        return false;
      }
      else
      {
        if (a->GetVarIndex() < b->GetVarIndex())
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }
    else if (a->IsLeaf() && !b->IsLeaf())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
};

// standard order for a set of subnodes is:
// constants + vars^{rising powers} + complex operands
class NodeOrder
{
public:
  int operator() (const Expression& a,
                  const Expression& b)
  {
    if (a->IsConstant() && !b->IsConstant())
    {
      return true;
    }
    else if (a->IsVariable() && b->IsVariable())
    {
      if (a->GetExponent() < b->GetExponent())
      {
        return true;
      }
      else if (a->GetExponent() > b->GetExponent())
      {
        return false;
      }
      else
      {
        if (a->GetVarIndex() < b->GetVarIndex())
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }
    else if (a->IsLeaf() && !b->IsLeaf())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
};

bool ReorderNodes(Expression* a)
{
  bool ret = false;
  if (!(*a)->IsLeaf() && (*a)->GetSize() > 1 &&
      ((*a)->GetOpType() == SUM || (*a)->GetOpType() == PRODUCT))
  {
    int i;
    for(i = 0; i < (*a)->GetSize(); i++)
    {
      ReorderNodes((*a)->GetNodePtr(i));
    }
    // how do I get this sort to tell me whether it did anything or not?
    // at the moment this function returns false by definition, incorrect
    if ((*a)->GetOpType() == SUM)
    {
      sort(((*a)->GetNodeVectorPtr())->begin(),
           ((*a)->GetNodeVectorPtr())->end(), NodeOrderSum());
    }
    else
    {
      sort(((*a)->GetNodeVectorPtr())->begin(),
           ((*a)->GetNodeVectorPtr())->end(), NodeOrder());
    }
  }
  return ret;
}

bool CompactLinearPart(Expression* a)
{
  bool ret = false;
  bool ischanged = false;
  ischanged = SimplifyConstant(a);
  if (!ret && ischanged)
    ret = true;
  ischanged = DifferenceToSum(a);
  if (!ret && ischanged)
    ret = true;
  ischanged = CompactLinearPartRecursive(a);
  if (!ret && ischanged)
    ret = true;
  ischanged = ReorderNodes(a);
  return ret;
}

bool CompactLinearPartRecursive(Expression* a)
{
  bool ret = false;
  bool ischanged = false;
  int i, j;
  i = 0;
  int sz = (*a)->GetSize();
  if ((*a)->GetOpType() == SUM)
  {
    while(i < sz)
    {
      ischanged = CompactLinearPartRecursive((*a)->GetNodePtr(i));
      if (!ret && ischanged)
        ret = true;
      if ((*a)->GetNode(i)->GetOpType() == SUM)
      {
        ret = true;
        double ci = (*a)->GetNode(i)->GetCoeff();
        for(j = 0; j < (*a)->GetNode(i)->GetSize(); j++)
        {
          Expression nodej((*a)->GetNode(i)->GetNode(j));
          nodej->SetCoeff(nodej->GetCoeff() * ci);
          (*a)->AddNode(nodej);
          ++ sz;// we added a node
        }
        (*a)->DeleteNode(i);
        -- sz; // we have deleted a node
        // we don't need to increase i, since we have deleted the i-th node
        // the next node is still the i-th node
        if (sz == 1)
        {
          a->SetTo((*a)->GetNode(0));
          i = 0;
          sz = (*a)->GetSize();
        }
      }
      else
      {
        i++;
      }
    }
  }
  return ret;
}


// deals with cases like *(*(x,y), z) --> *(x,y,z)
bool CompactProducts(Expression* a)
{
  bool ret = false;
  bool ischanged = false;
  int i, j;
  if ((*a)->GetOpType() == PRODUCT)
  {
    for(i = 0; i < (*a)->GetSize(); i++)
    {
      ischanged = CompactProducts((*a)->GetNodePtr(i));
      if (!ret && ischanged)
        ret = true;
      if ((*a)->GetNode(i)->GetOpType() == PRODUCT)
      {
        ret = true;
        for(j = 0; j < (*a)->GetNode(i)->GetSize(); j++)
        {
          (*a)->AddNode((*a)->GetNode(i)->GetNode(j));
        }
        (*a)->DeleteNode(i);
      }
    }
    if ((*a)->GetSize() == 1)
    {
      // product with just one operand, up a level
      double c = (*a)->GetCoeff();
      a->SetTo((*a)->GetNode(0));
      (*a)->SetCoeff((*a)->GetCoeff() * c);
      ret = true;
    }
  }
  else
  {
    // not a product, recurse
    for(i = 0; i < (*a)->GetSize(); i++)
    {
      ischanged = CompactProducts((*a)->GetNodePtr(i));
      if (!ret && ischanged)
      {
        ret = true;
      }
    }
  }
  (*a)->ConsolidateProductCoeffs();
  return ret;
}

// generic simplification on a copy of the expression
Expression SimplifyCopy(Expression* a,
                        bool & ischanged)
{
  Expression b;
  b = (*a).Copy();
  ischanged = Simplify(&b);
  return b;
}

// recursive destroy - deleted all the tree and nodes in a tree - use
// with caution
void RecursiveDestroy(Expression* a)
{
  int i;
  for(i = 0; i < (*a)->GetSize(); i++)
  {
    RecursiveDestroy((*a)->GetNodePtr(i));
  }
  a->Destroy();
}


} /* namespace Ev3 */
