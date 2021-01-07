//                                               -*- C++ -*-
/**
 *  @brief Class for Box Cox function implementation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_INVERSETRENDEVALUATION_HXX
#define OPENTURNS_INVERSETRENDEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseTrendEvaluation
 *
 * This class offers an easy evaluation of the Box Cox function defined by :
 *  h(x) = \frac{x^\lambda - 1}{\lambda} for \lambda non zero, log(x) oherwise
 *  Care that x should be positive
 */

class OT_API InverseTrendEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseTrendEvaluation();

  /** Parameter constructor */
  InverseTrendEvaluation(const Function & function);

  /** Virtual constructor */
  InverseTrendEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const InverseTrendEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the 1D function */
  Function getFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The 1D function underlying the InverseTrend transform */
  Function function_;


}; /* class InverseTrendEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSETRENDEVALUATION_HXX */
