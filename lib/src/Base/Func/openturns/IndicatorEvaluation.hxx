//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
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

#ifndef OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/ComparisonOperator.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class IndicatorEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API IndicatorEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<EvaluationImplementation>           EvaluationPointer;

  /** Default constructor */
  IndicatorEvaluation();

  /** Default constructor */
  IndicatorEvaluation(const EvaluationPointer & p_evaluation,
                      const ComparisonOperator & comparisonOperator,
                      const Scalar threshold);

  /** Virtual constructor */
  virtual IndicatorEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const IndicatorEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const;

  /** Accessor for the underlying evaluation */
  EvaluationPointer getEvaluation() const;
  void setEvaluation(const EvaluationPointer & p_evaluation);

  /** Accessor for the comparison operator */
  ComparisonOperator getComparisonOperator() const;
  void setComparisonOperator(const ComparisonOperator & comparisonOperator);

  /** Accessor for the threshold */
  Scalar getThreshold() const;
  void setThreshold(const Scalar threshold);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  EvaluationPointer p_evaluation_;
  ComparisonOperator comparisonOperator_;
  Scalar threshold_;

}; /* class IndicatorEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
