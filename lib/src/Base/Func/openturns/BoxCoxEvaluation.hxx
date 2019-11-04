//                                               -*- C++ -*-
/**
 *  @brief Class for Box Cox function implementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_BOXCOXEVALUATION_HXX
#define OPENTURNS_BOXCOXEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxEvaluation
 *
 * This class offers an easy evaluation of the Box Cox function defined by :
 *  h(x) = \frac{(x-s)^\lambda - 1}{\lambda} for \lambda non zero, log(x-s) otherwise
 *  Care that x should be > s
 */

class OT_API BoxCoxEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  BoxCoxEvaluation();

  /** Parameter constructor */
  explicit BoxCoxEvaluation(const Point & lamda);

  BoxCoxEvaluation(const Point & lamda,
                   const Point & shift);

  /** Virtual constructor */
  virtual BoxCoxEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const BoxCoxEvaluation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const;
  Sample operator() (const Sample & inS) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** Lambda vector of the box cox transform */
  Point lambda_;

  /** Shift vector of the box cox transform */
  Point shift_;


}; /* class BoxCoxEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXEVALUATION_HXX */
