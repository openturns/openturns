//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblemImplementation allows to describe an optimization problem
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
#ifndef OPENTURNS_OPTIMIZATIONPROBLEMIMPLEMENTATION_HXX
#define OPENTURNS_OPTIMIZATIONPROBLEMIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationProblemImplementation
 * OptimizationProblemImplementation allows to describe an optimization problem
 */

class OT_API OptimizationProblemImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  /** enum and typedef declarations */

  enum VariableType
  {
    CONTINUOUS,
    BINARY,
    INTEGER
  };

  /** Default constructor */
  OptimizationProblemImplementation();

  /** Constructor with parameters */
  explicit OptimizationProblemImplementation(const Function& objective);

  /** Constructor with parameters */
  OptimizationProblemImplementation(const Function & objective,
                                    const Function & equalityConstraint,
                                    const Function & inequalityConstraint,
                                    const Interval & bounds);

  /** Virtual constructor */
  virtual OptimizationProblemImplementation * clone() const;

  /** Objective functions accessor */
  Function getObjective() const;
  void setObjective(const Function & objective);
  Bool hasMultipleObjective() const;

  /** Equality constraint function accessor */
  Function getEqualityConstraint() const;
  void setEqualityConstraint(const Function & equalityConstraint);
  Bool hasEqualityConstraint() const;

  /** Inequality constraint function accessor */
  Function getInequalityConstraint() const;
  void setInequalityConstraint(const Function & inequalityConstraint);
  Bool hasInequalityConstraint() const;

  /** Bounds accessor */
  Interval getBounds() const;
  void setBounds(const Interval & bounds);
  Bool hasBounds() const;

  /** Level function accessor */
  virtual Function getLevelFunction() const;
  virtual void setLevelFunction(const Function & levelFunction);
  virtual Bool hasLevelFunction() const;

  /** Level value accessor */
  virtual Scalar getLevelValue() const;
  virtual void setLevelValue(Scalar levelValue);

  /** Residual function accessor */
  virtual Function getResidualFunction() const;
  virtual void setResidualFunction(const Function & residualFunction);
  virtual Bool hasResidualFunction() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Minimization accessor */
  void setMinimization(Bool minimization);
  Bool isMinimization() const;

  /** Variable types accessors */
  void setVariablesType(const Indices & variableType);
  Indices getVariablesType() const;
  bool isContinuous() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  // The objective function
  Function objective_;

  // The equality constraint function
  Function equalityConstraint_;

  // The inequality constraint function
  Function inequalityConstraint_;

  // The bounds
  Interval bounds_;

  // Minimization problem
  Bool minimization_;

  // The dimension of the search space
  UnsignedInteger dimension_;

  // The type of the variables
  Indices variablesType_;

} ; /* class OptimizationProblemImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONPROBLEMIMPLEMENTATION_HXX */
