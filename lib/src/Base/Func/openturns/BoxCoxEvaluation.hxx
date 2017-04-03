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

#ifndef OPENTURNS_BOXCOXEVALUATION_HXX
#define OPENTURNS_BOXCOXEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
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
  CLASSNAME;
public:


  /** Default constructor */
  BoxCoxEvaluation();

  /** Parameter constructor */
  BoxCoxEvaluation(const NumericalPoint & lamda);

  BoxCoxEvaluation(const NumericalPoint & lamda,
                                 const NumericalPoint & shift);

  /** Virtual constructor */
  virtual BoxCoxEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const BoxCoxEvaluation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;
  NumericalSample operator() (const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for the lambda point */
  NumericalPoint getLambda() const;

  /** Accessor for the shift */
  NumericalPoint getShift() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** Lambda vector of the box cox transform */
  NumericalPoint lambda_;

  /** Shift vector of the box cox transform */
  NumericalPoint shift_;


}; /* class BoxCoxEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXEVALUATION_HXX */
