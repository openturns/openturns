/**********************************************************************
 * Author:      Leo Liberti                                            *
 * Name:        expression.h                                           *
 * Source:      GNU C++                                                *
 * Purpose:     symbolic expression (base classes and functionality)   *
 * History:     010517 0.0 work started                                *
 * License:    Code published under the Common Public License.         *
***********************************************************************/

#ifndef __EV3OPERANDH__
#define __EV3OPERANDH__

#include <sstream>

#include "common.h"
#include "exceptions.h"

namespace Ev3
{
// algebraic expression operand
class Operand
{

private:

protected:

  // one of the OperatorTypes above
  int oplabel_;

  // 0 if no dependency, 1 if constant, 2 if coefficient, 3 if exponent
  int dependency_;

  // if oplabel == CONST, the value of the constant
  double constant_;

  // dependency for constants (added for MORON - see ../PROGNOTES)
  double* depconstant_;

  // if oplabel == VAR, the index of the variable - should start from 1
  Int varindex_;

  // if oplabel == VAR, the name of the variable
  std::string varname_;

  // we allow multiplication for a constant coefficient in each Operand
  double coefficient_;

  // dependency for coefficients (added for MORON - see ../PROGNOTES)
  double* depcoefficient_;

  // we allow a real constant exponent in each Operand
  // THIS HAS MEANING ONLY IF operand IS A LEAF!!!
  double exponent_;

  // dependency for exponents (added for MORON - see ../PROGNOTES)
  double* depexponent_;

public:

  // constructors
  Operand();
  explicit Operand(const double t);
  explicit Operand(const Int t);
  Operand(const Int t,
          const bool isvar);
  // create a variable leaf and set coefficient
  Operand(const double c,
          const Int t,
          const std::string & vn);

  // Operand class methods:

  // prints to a string
  std::string ToString() const;

  // get operator type
  int GetOpType() const;

  // get constant value - in CONSTs it multiplies by coefficient and
  // raises to exponent
  double GetValue() const;

  // just get the value, in all cases
  double GetSimpleValue() const;

  // get variable index
  Int GetVarIndex() const;

  // get variable name
  std::string GetVarName() const;

  // get the coefficient
  double GetCoeff() const;

  // get the exponent
  double GetExponent() const;

  // set operator type
  void SetOpType(const int t);

  // set constant value
  void SetValue(const double t);

  // set variable index (start from 1 and add by steps of 1 when creating new
  // variables)
  void SetVarIndex(const Int t);

  // set variable name
  void SetVarName(const std::string & vn);

  // set the exponent
  void SetExponent(const double expon);

  // set the coefficient
  void SetCoeff(const double coeff);

  // set constant dependencies (added for MORON - see ../PROGNOTES)
  void SetDependencyOnOperand(const int whichconstant,
                              double** depvalue);

  // is operand a constant?
  bool IsConstant() const;

  // is operand a variable?
  bool IsVariable() const;

  // is operand a leaf node?
  bool IsLeaf() const;

  // is operand a zero constant?
  bool IsZero() const;

  // is operand a constant == v?
  bool HasValue(double v) const;

  // is operand a constant <= v?
  bool IsLessThan(double v) const;

  // is operand constant >= v?
  bool IsGreaterThan(double v) const;

  // set value = coefficient * value ^ exponent
  void ConsolidateValue();

  // enforce constant dependencies (added for MORON - see ../PROGNOTES)
  void EnforceDependencyOnOperand();

  // is operand this == operand t?
  bool operator == (const Operand & t);

  // substitute a variable with a constant
  void SubstituteVariableWithConstant(const int varindex,
                                      const double c);

};

std::ostream & operator<< (std::ostream & out, const Operand & operand);

} /* namespace Ev3 */

#endif /* __EV3OPERANDHXX__ */
