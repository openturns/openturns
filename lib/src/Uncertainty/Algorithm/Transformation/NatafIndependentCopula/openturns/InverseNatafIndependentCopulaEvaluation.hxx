//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for independent
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
#ifndef OPENTURNS_INVERSENATAFINDEPENDENTCOPULAEVALUATION_HXX
#define OPENTURNS_INVERSENATAFINDEPENDENTCOPULAEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNatafIndependentCopulaEvaluation
 *
 * This class offers an interface for the InverseNataf function for independent distributions
 */
class OT_API InverseNatafIndependentCopulaEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit InverseNatafIndependentCopulaEvaluation(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  InverseNatafIndependentCopulaEvaluation * clone() const override;

  /** Evaluation */
  Point operator () (const Point & inP) const override;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

protected:


private:
  UnsignedInteger dimension_;

}; /* InverseNatafIndependentCopulaEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENATAFINDEPENDENTCOPULAEVALUATION_HXX */
