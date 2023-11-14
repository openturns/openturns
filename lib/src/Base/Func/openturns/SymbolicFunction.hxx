//                                               -*- C++ -*-
/**
 *  @brief The class that implements symbolic functions.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SYMBOLICFUNCTION_HXX
#define OPENTURNS_SYMBOLICFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class SymbolicFunction
 *
 */
class OT_API SymbolicFunction
  : public Function
{
  CLASSNAME
public:

  /* Default constructor */
  SymbolicFunction();

  /** Parameter constructor */
  SymbolicFunction (const String & inputVariableName,
                    const String & formula);

  /** Parameter constructor */
  SymbolicFunction (const Description & inputVariablesNames,
                    const Description & formulas);

  /** Parameter constructor */
  SymbolicFunction (const Description & inputVariablesNames,
                    const Description & outputVariablesNames,
                    const String & formula);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Accessor to the formulas */
  Description getFormulas() const;

  /** Static methods for documentation of analytical fonctions */
  static Description GetValidParsers();
  static Description GetValidConstants();
  static Description GetValidFunctions();
  static Description GetValidOperators();

private:
  /** List of valid parsers */
  static Description ValidParsers_;

  /** List of valid constants */
  static Description ValidConstants_;

  /** List of valid functions */
  static Description ValidFunctions_;

  /** List of valid operators */
  static Description ValidOperators_;

  /** Flag to tell if the documentation has been initialized */
  static Bool IsDocumentationInitialized_;

  /** Method that initialize the fields related to the documentation of the analytical functions */
  static void InitializeDocumentation();

}; /* class QuadraticFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SYMBOLICFUNCTION_HXX */
