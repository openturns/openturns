//                                               -*- C++ -*-
/**
 * @brief Class for Box Cox function implementation
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_INVERSEBOXCOXHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_INVERSEBOXCOXHESSIANIMPLEMENTATION_HXX

#include "NumericalMathHessianImplementation.hxx"
#include "InverseBoxCoxEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxHessianImplementation
 *
 * This class offers an easy hessian of the Box Cox function defined by :
 *  \Delta(h)(x) = (\lambda - 1)(x-s)^{\lambda - 2} for \lambda non zero, -1/(x-s)^2 otherwise
 *  Care that x should be > s
 */

class OT_API InverseBoxCoxHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  InverseBoxCoxHessianImplementation();

  /** Parameter constructor */
  InverseBoxCoxHessianImplementation(const InverseBoxCoxEvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual InverseBoxCoxHessianImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxHessianImplementation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Hessian evaluation method */
  using NumericalMathHessianImplementation::hessian;
  SymmetricTensor hessian(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the lambda point */
  NumericalPoint getLambda() const;

  /** Accessor for the shift */
  NumericalPoint getShift() const;

  /** Accessor for the evaluation */
  InverseBoxCoxEvaluationImplementation getEvaluation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** The underlying evaluation */
  InverseBoxCoxEvaluationImplementation evaluation_;

}; /* class InverseBoxCoxHessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXHESSIANIMPLEMENTATION_HXX */
