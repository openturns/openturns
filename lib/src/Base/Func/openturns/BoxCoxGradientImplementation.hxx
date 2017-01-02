//                                               -*- C++ -*-
/**
 * @brief Class for Box Cox function implementation
 *
 * Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_BOXCOXGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_BOXCOXGRADIENTIMPLEMENTATION_HXX

#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/BoxCoxEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxGradientImplementation
 *
 * This class offers an easy gradient of the Box Cox function defined by :
 *  \Delta(h)(x) = (x-s)^{\lambda - 1} for \lambda non zero, 1/(x-s) otherwise
 *  Care that x should be > s
 */

class OT_API BoxCoxGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  BoxCoxGradientImplementation();

  /** Parameter constructor */
  BoxCoxGradientImplementation(const BoxCoxEvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual BoxCoxGradientImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const BoxCoxGradientImplementation & other) const;

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
  BoxCoxEvaluationImplementation getEvaluation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** The underlying evaluation */
  BoxCoxEvaluationImplementation evaluation_;

}; /* class BoxCoxGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXGRADIENTIMPLEMENTATION_HXX */
