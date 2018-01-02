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
  virtual InverseTrendEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const InverseTrendEvaluation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the 1D function */
  Function getFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** The 1D function underlying the InverseTrend transform */
  Function function_;


}; /* class InverseTrendEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSETRENDEVALUATION_HXX */
