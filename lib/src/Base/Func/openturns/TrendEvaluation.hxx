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

#ifndef OPENTURNS_TRENDEVALUATION_HXX
#define OPENTURNS_TRENDEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TrendEvaluation
 *
 * This class offers an easy evaluation of the Box Cox function defined by :
 *  h(x) = \frac{x^\lambda - 1}{\lambda} for \lambda non zero, log(x) oherwise
 *  Care that x should be positive
 */

class OT_API TrendEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  TrendEvaluation();

  /** Parameter constructor */
  TrendEvaluation(const NumericalMathFunction & function);

  /** Virtual constructor */
  virtual TrendEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const TrendEvaluation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the 1D function */
  NumericalMathFunction getFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** The 1D function underlying the Trend transform */
  NumericalMathFunction function_;


}; /* class TrendEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRENDEVALUATION_HXX */
