//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
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

#ifndef OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "ComparisonOperator.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class IndicatorNumericalMathEvaluationImplementation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API IndicatorNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathEvaluationImplementation>           EvaluationImplementation;

  /** Default constructor */
  IndicatorNumericalMathEvaluationImplementation();

  /** Default constructor */
  IndicatorNumericalMathEvaluationImplementation(const EvaluationImplementation & p_evaluation,
      const ComparisonOperator & comparisonOperator,
      const NumericalScalar threshold);

  /** Virtual constructor */
  virtual IndicatorNumericalMathEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const IndicatorNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Operator () */
  using NumericalMathEvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;

  /** Accessor for the underlying evaluation */
  EvaluationImplementation getEvaluation() const;
  void setEvaluation(const EvaluationImplementation & p_evaluation);

  /** Accessor for the comparison operator */
  ComparisonOperator getComparisonOperator() const;
  void setComparisonOperator(const ComparisonOperator & comparisonOperator);

  /** Accessor for the threshold */
  NumericalScalar getThreshold() const;
  void setThreshold(const NumericalScalar threshold);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  EvaluationImplementation p_evaluation_;
  ComparisonOperator comparisonOperator_;
  NumericalScalar threshold_;

}; /* class IndicatorNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
