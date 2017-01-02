//                                               -*- C++ -*-
/**
 *  @brief The class that implements analytical functions.
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_ANALYTICALFUNCTION_HXX
#define OPENTURNS_ANALYTICALFUNCTION_HXX

#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class AnalyticalFunction
 *
 */
class OT_API AnalyticalFunction
  : public NumericalMathFunction
{
  CLASSNAME;
public:

  /* Default constructor */
  AnalyticalFunction();

  /** Parameter constructor */
  AnalyticalFunction (const String & inputVariableName,
                      const String & formula);

  /** Parameter constructor */
  AnalyticalFunction (const Description & inputVariablesNames,
                      const Description & formulas);

  /** Comparison operator */
  Bool operator ==(const AnalyticalFunction & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Static methods for documentation of analytical fnctions */
  static Description GetValidConstants();
  static Description GetValidFunctions();
  static Description GetValidOperators();

private:
  /** List of muParser valid constants */
  static Description ValidConstants_;

  /** List of muParser valid functions */
  static Description ValidFunctions_;

  /** List of muParser valid operators */
  static Description ValidOperators_;

  /** Flag to tell if the documentation has been initialized */
  static Bool IsDocumentationInitialized_;

  /** Method that initialize the fields related to the documentation of the analytical functions */
  static void InitializeDocumentation();

}; /* class QuadraticNumericalMathFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ANALYTICALFUNCTION_HXX */
