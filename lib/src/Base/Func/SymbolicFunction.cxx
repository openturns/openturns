//                                               -*- C++ -*-
/**
 *  @brief The class that implements analytical functions.
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SymbolicFunction.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/SymbolicGradient.hxx"
#include "openturns/SymbolicHessian.hxx"
#endif
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SymbolicFunction)

// Inline documentation for analytical functions
Bool SymbolicFunction::IsDocumentationInitialized_ = false;
Description SymbolicFunction::ValidParsers_;
Description SymbolicFunction::ValidConstants_;
Description SymbolicFunction::ValidFunctions_;
Description SymbolicFunction::ValidOperators_;

/* Default constructor */
SymbolicFunction::SymbolicFunction ()
  : Function()
{
  // Nothing to do
}


/* Parameter constructor */
SymbolicFunction::SymbolicFunction (const String & inputVariablesName,
                                    const String & formula)
  : Function()
{
  *this = SymbolicFunction(Description(1, inputVariablesName), Description(1, formula));
}


/* Parameter constructor */
SymbolicFunction::SymbolicFunction (const Description & inputVariablesNames,
                                    const Description & formulas)
  : Function()
{
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  const Description outputVariablesNames(Description::BuildDefault(formulas.getSize(), "y"));

  // Try to build an analytical gradient
  SymbolicEvaluation evaluation(inputVariablesNames, outputVariablesNames, formulas);
  setEvaluation(evaluation.clone());
  try
  {
    setGradient(new SymbolicGradient(evaluation));
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical gradient, using finite differences instead.");
    const Scalar epsilon = ResourceMap::GetAsScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon");
    setGradient(new CenteredFiniteDifferenceGradient(epsilon, getEvaluation()));
  }
  try
  {
    setHessian(new SymbolicHessian(evaluation));
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical hessian, using finite differences instead.");
    const Scalar epsilon = ResourceMap::GetAsScalar("CenteredFiniteDifferenceHessian-DefaultEpsilon");
    setHessian(new CenteredFiniteDifferenceHessian(epsilon, getEvaluation()));
  }
#else
  throw NotYetImplementedException(HERE) << "SymbolicFunction requires muParser or ExprTk";
#endif
}

/* Parameter constructor */
SymbolicFunction::SymbolicFunction (const Description & inputVariablesNames,
                                    const Description & outputVariablesNames,
                                    const String & formula)
  : Function()
{
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  // Try to build an analytical gradient
  SymbolicEvaluation evaluation(inputVariablesNames, outputVariablesNames, formula);
  setEvaluation(evaluation.clone());
  try
  {
    setGradient(new SymbolicGradient(evaluation));
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical gradient, using finite differences instead.");
    const Scalar epsilon = ResourceMap::GetAsScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon");
    setGradient(new CenteredFiniteDifferenceGradient(epsilon, getEvaluation()));
  }
  try
  {
    setHessian(new SymbolicHessian(evaluation));
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical hessian, using finite differences instead.");
    const Scalar epsilon = ResourceMap::GetAsScalar("CenteredFiniteDifferenceHessian-DefaultEpsilon");
    setHessian(new CenteredFiniteDifferenceHessian(epsilon, getEvaluation()));
  }
#else
  throw NotYetImplementedException(HERE) << "SymbolicFunction requires muParser or ExprTk";
#endif
}

/* String converter */
String SymbolicFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << SymbolicFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String SymbolicFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Initialization of the documentation */
void SymbolicFunction::InitializeDocumentation()
{
  if (IsDocumentationInitialized_) return;

  // First, the constants
  ValidConstants_.setName("Valid constants");
  ValidConstants_.add("e_ -> Euler's constant (2.71828...)");
  ValidConstants_.add("pi_ -> Pi constant (3.14159...)");

  // Second, the functions
  ValidFunctions_.setName("Valid functions");
  ValidFunctions_.add("sin(arg) -> sine function");
  ValidFunctions_.add("cos(arg) -> cosine function");
  ValidFunctions_.add("cotan(arg) -> cotangent function");
  ValidFunctions_.add("tan(arg) -> tangent function");
  ValidFunctions_.add("asin(arg) -> inverse sine function");
  ValidFunctions_.add("acos(arg) -> inverse cosine function");
  ValidFunctions_.add("acotan(arg) -> inverse cotangent function");
  ValidFunctions_.add("atan(arg) -> inverse tangent function, values in (-pi/2, pi/2)");
  ValidFunctions_.add("atan2(arg1, arg2) -> inverse tangent function, values in (-pi, pi)");
  ValidFunctions_.add("sinh(arg) -> hyperbolic sine function");
  ValidFunctions_.add("cosh(arg) -> hyperbolic cosine function");
  ValidFunctions_.add("tanh(arg) -> hyperbolic tangens function");
  ValidFunctions_.add("asinh(arg) -> inverse hyperbolic sine function");
  ValidFunctions_.add("acosh(arg) -> inverse hyperbolic cosine function");
  ValidFunctions_.add("atanh(arg) -> inverse hyperbolic tangent function");
  ValidFunctions_.add("log2(arg) -> logarithm in base 2");
  ValidFunctions_.add("log10(arg) -> logarithm in base 10");
  ValidFunctions_.add("log(arg) -> logarithm in base e (2.71828...)");
  ValidFunctions_.add("ln(arg) -> alias for log function");
  ValidFunctions_.add("lngamma(arg) -> log of the gamma function");
  ValidFunctions_.add("gamma(arg) -> gamma function");
  ValidFunctions_.add("exp(arg) -> exponential function");
  ValidFunctions_.add("erf(arg) -> error function");
  ValidFunctions_.add("erfc(arg) -> complementary error function");
  ValidFunctions_.add("abs(arg) -> absolute value function");
  ValidFunctions_.add("sqrt(arg) -> square root function");
  ValidFunctions_.add("cbrt(arg) -> cubic root function");
  ValidFunctions_.add("besselJ0(arg) -> 1rst kind Bessel function with parameter 0");
  ValidFunctions_.add("besselJ1(arg) -> 1rst kind Bessel function with parameter 1");
  ValidFunctions_.add("besselY0(arg) -> 2nd kind Bessel function with parameter 0");
  ValidFunctions_.add("besselY1(arg) -> 2nd kind Bessel function with parameter 1");
  ValidFunctions_.add("floor(arg) -> round to nearest integer");
  ValidFunctions_.add("ceil(arg) -> round to nearest integer");
  ValidFunctions_.add("trunc(arg) -> round to nearest integer");
  ValidFunctions_.add("round(arg) -> round to nearest integer");
  ValidFunctions_.add("rint(arg) -> round to nearest integer");
  ValidFunctions_.add("sign(arg) -> sign function -1 if x<0; 1 if x>0");
  ValidFunctions_.add("(condition ? value1 : value2) -> if condition then value1 else value2");
  ValidFunctions_.add("sum(arg1, ..., argn) -> sum of all arguments");
  ValidFunctions_.add("avg(arg1, ..., argn) -> mean value of all arguments");
  ValidFunctions_.add("min(arg1, ..., argn) -> min of all arguments");
  ValidFunctions_.add("max(arg1, ..., argn) -> max of all arguments");

  // Third, the operators
  ValidOperators_.setName("Valid operators");
  ValidOperators_.add("= -> assignement, can only be applied to variable names (priority -1)");
  ValidOperators_.add("&& -> logical and (priority 1)");
  ValidOperators_.add("|| -> logical or (priority 1)");
  ValidOperators_.add("^^ -> logical xor (priority 1)");
  ValidOperators_.add("!  -> logical negation (priority 4)");
  ValidOperators_.add("<= -> less or equal (priority 2)");
  ValidOperators_.add(">= -> greater or equal (priority 2)");
  ValidOperators_.add("!= -> not equal (priority 2)");
  ValidOperators_.add("== -> equal (priority 2)");
  ValidOperators_.add(">  -> greater (priority 2)");
  ValidOperators_.add("<  -> less (priority 2)");
  ValidOperators_.add("+  -> addition (priority 3)");
  ValidOperators_.add("-  -> subtraction (priority 3)");
  ValidOperators_.add("*  -> multiplication (priority 4)");
  ValidOperators_.add("/  -> division (priority 4)");
  ValidOperators_.add("-  -> sign change (priority 4)");
  ValidOperators_.add("^  -> x to the power of y (priority 5)");

  // Fourth, the parsers
  ValidParsers_.setName("Valid parsers");
#ifdef OPENTURNS_HAVE_EXPRTK
  ValidParsers_.add("ExprTk");
#endif
#ifdef OPENTURNS_HAVE_MUPARSER
  ValidParsers_.add("MuParser");
#endif

  IsDocumentationInitialized_ = true;
}

/* Static methods for documentation of analytical fonctions */
Description SymbolicFunction::GetValidParsers()
{
  if (!IsDocumentationInitialized_) InitializeDocumentation();
  return ValidParsers_;
}

Description SymbolicFunction::GetValidConstants()
{
  if (!IsDocumentationInitialized_) InitializeDocumentation();
  return ValidConstants_;
}

Description SymbolicFunction::GetValidFunctions()
{
  if (!IsDocumentationInitialized_) InitializeDocumentation();
  return ValidFunctions_;
}

Description SymbolicFunction::GetValidOperators()
{
  if (!IsDocumentationInitialized_) InitializeDocumentation();
  return ValidOperators_;
}

END_NAMESPACE_OPENTURNS
