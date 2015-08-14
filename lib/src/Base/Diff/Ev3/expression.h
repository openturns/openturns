/**********************************************************************
 * Author:      Leo Liberti                                            *
 * Name:        expression.h                                           *
 * Source:      GNU C++                                                *
 * Purpose:     symbolic expression (base classes and functionality)   *
 * History:     010517 0.0 work started                                *
 * License:    (C) Leo Liberti, all rights reserved. Code published under the
               Common Public License.
***********************************************************************/

#ifndef __EV3EXPRESSIONH__
#define __EV3EXPRESSIONH__

#include <iostream>
#include <vector>
#include <map>

#include "common.h"
#include "tree.h"
#include "exceptions.h"
#include "operand.h"

namespace Ev3
{

class BasicExpression;

typedef Pointer<BasicExpression> Expression;

class BasicExpression : public Operand, public Tree<BasicExpression>
{

private:

public:

  // constructors

  // create empty
  BasicExpression();

  // create a constant leaf
  explicit BasicExpression(const double t);

  // create a constant (integer-valued) leaf
  explicit BasicExpression(const Int t);

  // create an (empty) operator or a variable leaf
  BasicExpression(const Int t,
                  const bool isvar);

  // create a variable leaf and set coefficient
  BasicExpression(const double c,
                  const Int t,
                  const std::string & vn);

  // user-defined copy constructor with two options (to make a copy)
  BasicExpression(const Expression & t,
                  const bool iscopy);

  // copy constructor
  BasicExpression(const BasicExpression & t);

  // destructor
  ~BasicExpression();

  // BasicExpression class methods:
  void Debug () const;

  // prints to a string
  std::string ToString() const;

  // output is a tree
  std::string PrintTree(const int blanks,
                        const int tabs) const;

  // sets an expression to zero (deleting all existing subnodes)
  void Zero();

  // sets an expression to one (deleting all existing subnodes)
  void One();

  // is expression this == expression t?
  // (note that this half-replicates Tree::operator==,
  //  but I couldn't think of any other convenient way to fit in
  //  the operand data in == and still use the Tree's ==)
  bool IsEqualTo(const Expression & t) const;
  bool operator == (const BasicExpression & t) const;

  // other comparison operators
  bool IsEqualTo(const double t) const;

  // this returns true if args are equal up to top node coefficient
  bool IsEqualToNoCoeff(const Expression & t) const;

  // this returns true if args have equal operator structures and
  // maintain variable/constant assignment to leaf nodes
  // (a schema is an expression modulo leaves)
  bool IsEqualBySchema(const Expression & t) const;

  // this returns true if args have equal operator label
  bool IsEqualByOperator(const int theoplabel) const;

  // this returns the number of variables in the expression
  int NumberOfVariables() const;
  int NumberOfVariables(int & maxvi) const;

  // whether expression depends on variable
  bool DependsOnVariable(const Int vi) const;

  // whether expression depends on variable linearly
  // (0=depends nonlinearly, 1=depends linearly, 2=doesn't depend at all)
  int DependsLinearlyOnVariable(const Int vi) const;

  // in a product, multiply all coeffs. of the operands, set result
  // as coeff of whole product, reset all operand coffs at 1; if
  // resulting global coeff is zero, delete all nodes and set
  // this to zero constant.
  // don't do anything on other operators
  void ConsolidateProductCoeffs();

  // in a sum or product, if coeff of sum operand is not 1, distribute it
  // to the operands and set whole coeff to 1
  void DistributeCoeffOverSum();
  void DistributeCoeffOverProduct();

  // enforce constant dependencies (added for MORON - see ../PROGNOTES)
  // this only acts on the proper leaf nodes
  void EnforceDependency();

  // substitute a variable with a constant
  void VariableToConstant(const int varindex,
                          const double c);

  // replace variable indexed v1 with variable indexed v2 (with varname vn)
  void ReplaceVariable(const int v1,
                       const int v2,
                       const std::string & vn);
  void ReplaceVariable(const int v1,
                       const int v2,
                       const std::string & vn,
                       const double c2);

  // replace with a variable the deepest node conforming to schema and
  // return replaced term or zero expression if no replacement occurs
  Expression ReplaceBySchema(const int vi,
                             const std::string & vn,
                             const Expression & schema);
  // works on subnodes not on current node
  Expression ReplaceBySchemaRecursive(const int vi,
                                      const std::string & vn,
                                      const Expression & schema);

  // replace with a variable the deepest node with given operator label
  // return replaced term or zero expression if no replacement occurs
  Expression ReplaceByOperator(const int vi,
                               const std::string & vn,
                               const int oplabel);
  // works on subnodes not on current node
  Expression ReplaceByOperatorRecursive(const int vi,
                                        const std::string & vn,
                                        const int oplabel);

  // replace all occurrences of subexpression needle with replace
  // return number of replacements
  int ReplaceSubexpression(const Expression & needle,
                           const Expression & replace);

  // replace this with given expression (SIGSEGV risk, see implementation)
  void ReplaceWithExpression(const Expression & replace);

  // reset all names of variables having IDs between lid, uid to vn
  void ResetVarNames(const std::string & vn,
                     int lid,
                     int uid);

  // distribute products over sums - returns true if changed
  // (re-call until false)
  bool DistributeProductsOverSums();

  // find variable indices in an expression
  void GetVarIndices(std::vector<int> & vidx);

  // return list of varIDs involved in a certain schema
  // e.g. f(x1*x2+x1*x3+x4*log(x5), x*y) = {1,2,3}
  void GetVarIndicesInSchema(std::vector<int> & vidx,
                             const Expression & schema);

  // find the variable name corresponding to variable index vi
  std::string FindVariableName(int vi);

  // is this expression linear?
  bool IsLinear() const;

  // is this expression a quadratic product of variables?
  // If yes, return the product type: PRODUCT, POWER or VAR
  bool IsQuadratic(int & prodtype) const;
  bool IsQuadratic() const;

  // return info about the linear part (assumes Simplify() has already been
  // called on this) - return false if expression has no linear part
  // by "linear part" we mean lin(x) in expr(x,y) = lin(x) + nonlin(y)
  // lincoeff[i] is the i-th nonzero coeff, linvi[i] is the corresponding
  // i-th varindex (starts from 1), c is the constant term to be added
  bool GetLinearInfo(std::vector<double> & lincoeff,
                     std::vector<int> & linvi,
                     std::vector<std::string> & linvn,
                     double & c);

  // return info about the pure linear part (assumes Simplify() has
  // already been called on this) - much as above call but the "pure
  // linear part" is e.g. x+y in x+y+y^2
  bool GetPureLinearInfo(std::vector<double> & lincoeff,
                         std::vector<int>& linvi,
                         std::vector<std::string> & linvn,
                         double & c);

  // get the linear part - x in x+y+y^2
  Expression GetLinearPart();

  // get the pure linar part - x+y in x+y+y^2
  Expression GetPureLinearPart();

  // get the nonlinear part - nonlin(y) in expr(x,y) = lin(x) + nonlin(y)
  Expression GetNonlinearPart();

  // get the purely nonlinear part - eg. y^2 in x+y+y^2
  Expression GetPureNonlinearPart();

  // return the additive constant of the expression
  double GetConstantPart();

  // return the additive constant of the expression and remove it from the
  // expression itself
  double RemoveAdditiveConstant();
};

std::ostream & operator<< (std::ostream & out, const BasicExpression & expr);
std::ostream & operator<< (std::ostream & out, const Expression & expr);

// All these functions contain tricks to simplify the operands. This
// means that both the operands may be changed, and indeed that the
// return value is often one of the changed operands.  To make sure
// you are not changing the operands, use the CopyOf function or the
// Copy() method in Pointer class; or use the equivalent operators below
// in order not to touch the argument expressions.

// add a link of b to a, return link to created expression
Expression SumLink(Expression a,
                   Expression b);
// add a link of b to a with coeff -1, return link of a
Expression DifferenceLink(Expression a,
                          Expression b);
// multiply a by a link of b, return link of a
Expression ProductLink(Expression a,
                       Expression b);
// divide a by a link of b, return link of a
Expression FractionLink(Expression a,
                        Expression b);
// raise a to power b, return link of a
Expression PowerLink(Expression a,
                     Expression b);
// change sign to coeff of a, return link of a
Expression MinusLink(Expression a);
// other unary functions (of a):
Expression SinLink(Expression a);
Expression CosLink(Expression a);
Expression TanLink(Expression a);
Expression AsinLink(Expression a);
Expression AcosLink(Expression a);
Expression AtanLink(Expression a);
Expression SinhLink(Expression a);
Expression CoshLink(Expression a);
Expression TanhLink(Expression a);
Expression AsinhLink(Expression a);
Expression AcoshLink(Expression a);
Expression AtanhLink(Expression a);
Expression Log2Link(Expression a);
Expression Log10Link(Expression a);
Expression LogLink(Expression a);
Expression LnLink(Expression a);
Expression LngammaLink(Expression a);
Expression GammaLink(Expression a);
Expression ExpLink(Expression a);
Expression ErfLink(Expression a);
Expression ErfcLink(Expression a);
Expression SqrtLink(Expression a);
Expression CbrtLink(Expression a);
Expression BesselJ0Link(Expression a);
Expression BesselJ1Link(Expression a);
Expression BesselY0Link(Expression a);
Expression BesselY1Link(Expression a);
Expression SignLink(Expression a);
Expression RintLink(Expression a);
Expression AbsLink(Expression a);
Expression CotLink(Expression a);
Expression CothLink(Expression a);

// these are equivalent to the above but they don't change the arguments
Expression operator + (Expression a,
                       Expression b);
Expression operator - (Expression a,
                       Expression b);
Expression operator * (Expression a,
                       Expression b);
Expression operator / (Expression a,
                       Expression b);
Expression operator ^ (Expression a,
                       Expression b);
Expression operator - (Expression a);
Expression Sin(Expression a);
Expression Cos(Expression a);
Expression Tan(Expression a);
Expression Asin(Expression a);
Expression Acos(Expression a);
Expression Atan(Expression a);
Expression Sinh(Expression a);
Expression Cosh(Expression a);
Expression Tanh(Expression a);
Expression Asinh(Expression a);
Expression Acosh(Expression a);
Expression Atanh(Expression a);
Expression Log2(Expression a);
Expression Log10(Expression a);
Expression Log(Expression a);
Expression Ln(Expression a);
Expression Lngamma(Expression a);
Expression Gamma(Expression a);
Expression Exp(Expression a);
Expression Erf(Expression a);
Expression Erfc(Expression a);
Expression Sqrt(Expression a);
Expression Cbrt(Expression a);
Expression BesselJ0(Expression a);
Expression BesselJ1(Expression a);
Expression BesselY0(Expression a);
Expression BesselY1(Expression a);
Expression Sign(Expression a);
Expression Rint(Expression a);
Expression Abs(Expression a);
Expression Cot(Expression a);
Expression Coth(Expression a);

// symbolic differentiation of a w.r.t. variable with index vi,
// return the created expression (a is not changed)
Expression Diff(const Expression & a,
                Int vi);
Expression DiffNoSimplify(const Expression & ac,
                          Int vi);

// SIMPLIFICATIONS - all simplifications return true if they were
// effective or false if they weren't

// sin^2+cos^2 = 1 simplification
bool TrigSimp(Expression a);

// generic simplification with modification of the expression
bool Simplify(Expression* a);
bool SimplifyRecursive(Expression* a);
bool SimplifyConstant(Expression* a);
bool DifferenceToSum(Expression* a);
bool CompactLinearPart(Expression* a);
bool CompactLinearPartRecursive(Expression* a);
bool CompactProducts(Expression* a);
bool ReorderNodes(Expression* a);

// destroys the whole tree and all nodes, be careful
void RecursiveDestroy(Expression* a);

// generic simplification on a copy of the expression
Expression SimplifyCopy(Expression* a, bool& ischanged);

} /* namespace Ev3 */

#endif /* __EV3EXPRESSIONH__ */
