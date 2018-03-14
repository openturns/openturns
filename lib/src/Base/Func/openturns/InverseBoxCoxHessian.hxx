//                                               -*- C++ -*-
/**
 * @brief Class for Box Cox function implementation
 *
 * Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_INVERSEBOXCOXHESSIAN_HXX
#define OPENTURNS_INVERSEBOXCOXHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/InverseBoxCoxEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxHessian
 *
 * This class offers an easy hessian of the Box Cox function defined by :
 *  \Delta(h)(x) = (\lambda - 1)(x-s)^{\lambda - 2} for \lambda non zero, -1/(x-s)^2 otherwise
 *  Care that x should be > s
 */

class OT_API InverseBoxCoxHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseBoxCoxHessian();

  /** Parameter constructor */
  explicit InverseBoxCoxHessian(const InverseBoxCoxEvaluation & evaluation);

#ifndef SWIG
  /** Parameter constructor */
  explicit InverseBoxCoxHessian(const Pointer<InverseBoxCoxEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  virtual InverseBoxCoxHessian * clone() const;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxHessian & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Hessian evaluation method */
  using HessianImplementation::hessian;
  SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Accessor for the evaluation */
  Evaluation getEvaluation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** The underlying evaluation */
  Pointer<InverseBoxCoxEvaluation>  p_evaluation_;

}; /* class InverseBoxCoxHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXHESSIAN_HXX */
