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

#ifndef OPENTURNS_BOXCOXGRADIENT_HXX
#define OPENTURNS_BOXCOXGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/BoxCoxEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxGradient
 *
 * This class offers an easy gradient of the Box Cox function defined by :
 *  \Delta(h)(x) = (x-s)^{\lambda - 1} for \lambda non zero, 1/(x-s) otherwise
 *  Care that x should be > s
 */

class OT_API BoxCoxGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  BoxCoxGradient();

  /** Parameter constructor */
  explicit BoxCoxGradient(const BoxCoxEvaluation & evaluation);

  /** Virtual constructor */
  virtual BoxCoxGradient * clone() const;

  /** Comparison operator */
  Bool operator ==(const BoxCoxGradient & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Gradient evaluation method */
  using GradientImplementation::gradient;
  Matrix gradient(const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the lambda point */
  Point getLambda() const;

  /** Accessor for the shift */
  Point getShift() const;

  /** Accessor for the evaluation */
  BoxCoxEvaluation getEvaluation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** The underlying evaluation */
  BoxCoxEvaluation evaluation_;

}; /* class BoxCoxGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXGRADIENT_HXX */
