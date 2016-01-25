//                                               -*- C++ -*-
/**
 * @brief Class for Box Cox function implementation
 *
 * Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_INVERSEBOXCOXGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_INVERSEBOXCOXGRADIENTIMPLEMENTATION_HXX

#include "NumericalMathGradientImplementation.hxx"
#include "InverseBoxCoxEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxGradientImplementation
 *
 * This class offers an easy gradient of the Box Cox function defined by :
 *  \Delta(h)(x) = (x-s)^{\lambda - 1} for \lambda non zero, 1/(x-s) otherwise
 *  Care that x should be > s
 */

class OT_API InverseBoxCoxGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  InverseBoxCoxGradientImplementation();

  /** Parameter constructor */
  InverseBoxCoxGradientImplementation(const InverseBoxCoxEvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual InverseBoxCoxGradientImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxGradientImplementation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Gradient evaluation method */
  using NumericalMathGradientImplementation::gradient;
  Matrix gradient(const NumericalPoint & inP) const;

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

}; /* class InverseBoxCoxGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXGRADIENTIMPLEMENTATION_HXX */
