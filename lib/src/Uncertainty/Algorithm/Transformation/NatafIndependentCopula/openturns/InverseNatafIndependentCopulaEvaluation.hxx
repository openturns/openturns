//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for independent
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_INVERSENATAFINDEPENDENTCOPULAEVALUATION_HXX
#define OPENTURNS_INVERSENATAFINDEPENDENTCOPULAEVALUATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNatafIndependentCopulaEvaluation
 *
 * This class offers an interface for the InverseNataf function for independent distributions
 */
class OT_API InverseNatafIndependentCopulaEvaluation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:


  /** Parameter constructor */
  explicit InverseNatafIndependentCopulaEvaluation(const UnsignedInteger dimension);


  /** Virtual constructor */
  virtual InverseNatafIndependentCopulaEvaluation * clone() const;

  /** Evaluation */
  NumericalPoint operator () (const NumericalPoint & inP) const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset) const;

protected:


private:
  UnsignedInteger dimension_;

}; /* InverseNatafIndependentCopulaEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENATAFINDEPENDENTCOPULAEVALUATION_HXX */
