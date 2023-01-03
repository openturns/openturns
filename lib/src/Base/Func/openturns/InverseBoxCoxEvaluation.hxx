//                                               -*- C++ -*-
/**
 *  @brief Class for Inverse Box Cox function
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_INVERSEBOXCOXEVALUATION_HXX
#define OPENTURNS_INVERSEBOXCOXEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxEvaluation
 *
 * This class offers an easy evaluation of the Box Cox function defined by :
 *  h(x) = \frac{x^\lambda - 1}{\lambda} for \lambda non zero, log(x) oherwise
 *  Care that x should be positive
 */

class OT_API InverseBoxCoxEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseBoxCoxEvaluation();

  /** Parameter constructor */
  explicit InverseBoxCoxEvaluation(const Point & lamda);

  InverseBoxCoxEvaluation(const Point & lamda,
                          const Point & shift);

  /** Virtual constructor */
  InverseBoxCoxEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const override;
  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:



  /** lambda vector of the box cox transform */
  Point lambda_;

  /** Shift vector of the box cox transform */
  Point shift_;


}; /* class InverseBoxCoxEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXEVALUATION_HXX */
