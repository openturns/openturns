//                                               -*- C++ -*-
/**
 * @brief Class for Inverse Box Cox function
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

#ifndef OPENTURNS_INVERSEBOXCOXEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_INVERSEBOXCOXEVALUATIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "NumericalPoint.hxx"
#include "NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseBoxCoxEvaluationImplementation
 *
 * This class offers an easy evaluation of the Box Cox function defined by :
 *  h(x) = \frac{x^\lambda - 1}{\lambda} for \lambda non zero, log(x) oherwise
 *  Care that x should be positive
 */

class OT_API InverseBoxCoxEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  InverseBoxCoxEvaluationImplementation();

  /** Parameter constructor */
  InverseBoxCoxEvaluationImplementation(const NumericalPoint & lamda);

  InverseBoxCoxEvaluationImplementation(const NumericalPoint & lamda,
                                        const NumericalPoint & shift);

  /** Virtual constructor */
  virtual InverseBoxCoxEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const InverseBoxCoxEvaluationImplementation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Operator () */
  using NumericalMathEvaluationImplementation::operator();
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


}; /* class InverseBoxCoxEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEBOXCOXEVALUATIONIMPLEMENTATION_HXX */
