//                                               -*- C++ -*-
/**
 * @brief Class for Inverse Box Cox function
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

#ifndef OPENTURNS_INVERSEBOXCOXEVALUATION_HXX
#define OPENTURNS_INVERSEBOXCOXEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxEvaluation
 *
 * This class offers an easy evaluation of the Box Cox function defined by :
 *  h(x) = \frac{x^\lambda - 1}{\lambda} for \lambda non zero, log(x) oherwise
 *  Care that x should be positive
 */

class OT_API InverseBoxCoxEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  InverseBoxCoxEvaluation();

  /** Parameter constructor */
  InverseBoxCoxEvaluation(const NumericalPoint & lamda);

  InverseBoxCoxEvaluation(const NumericalPoint & lamda,
                                        const NumericalPoint & shift);

  /** Virtual constructor */
  virtual InverseBoxCoxEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxEvaluation & other) const;

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



  /** lambda vector of the box cox transform */
  NumericalPoint lambda_;

  /** Shift vector of the box cox transform */
  NumericalPoint shift_;


}; /* class InverseBoxCoxEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXEVALUATION_HXX */
