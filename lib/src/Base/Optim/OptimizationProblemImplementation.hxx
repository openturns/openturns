//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblemImplementation allows to describe an optimization problem
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_OPTIMIZATIONPROBLEMIMPLEMENTATION_HXX
#define OPENTURNS_OPTIMIZATIONPROBLEMIMPLEMENTATION_HXX

#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "NumericalMathFunction.hxx"
#include "Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationProblemImplementation
 * OptimizationProblemImplementation allows to describe an optimization problem
 */

class OptimizationProblemImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  /** Default constructor */
  OptimizationProblemImplementation();

  /** Constructor with parameters */
  OptimizationProblemImplementation(const NumericalMathFunction & objective,
                                    const NumericalMathFunction & equalityConstraint,
                                    const NumericalMathFunction & inequalityConstraint,
                                    const Interval & bounds);

  /** Virtual constructor */
  virtual OptimizationProblemImplementation * clone() const;

  /** Objective functions accessor */
  NumericalMathFunction getObjective() const;
  void setObjective(const NumericalMathFunction & objective);
  Bool hasMultipleObjective() const;

  /** Equality constraint function accessor */
  NumericalMathFunction getEqualityConstraint() const;
  void setEqualityConstraint(const NumericalMathFunction & equalityConstraint);
  Bool hasEqualityConstraint() const;

  /** Inequality constraint function accessor */
  NumericalMathFunction getInequalityConstraint() const;
  void setInequalityConstraint(const NumericalMathFunction & inequalityConstraint);
  Bool hasInequalityConstraint() const;

  /** Bounds accessor */
  Interval getBounds() const;
  void setBounds(const Interval & bounds);
  Bool hasBounds() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  // The objective function
  NumericalMathFunction objective_;

  // The equality constraint function
  NumericalMathFunction equalityConstraint_;

  // The inequality constraint function
  NumericalMathFunction inequalityConstraint_;

  // The bounds
  Interval bounds_;

  // The dimension of the search space
  UnsignedInteger dimension_;
} ; /* class OptimizationProblemImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONPROBLEMIMPLEMENTATION_HXX */
