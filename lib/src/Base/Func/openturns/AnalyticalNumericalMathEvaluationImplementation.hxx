//                                               -*- C++ -*-
/**
 * @brief The class that implements the evaluation of an analytical function.
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

#ifndef OPENTURNS_ANALYTICALNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_ANALYTICALNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/AnalyticalParser.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AnalyticalNumericalMathEvaluationImplementation
 *
 * The class that implement the evaluation of an analytical function.
 */
class OT_API AnalyticalNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  AnalyticalNumericalMathEvaluationImplementation();

  /** Default constructor */
  AnalyticalNumericalMathEvaluationImplementation(const Description & inputVariablesNames,
      const Description & outputVariablesNames,
      const Description & formulas);

  /** Virtual constructor */
  virtual AnalyticalNumericalMathEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const AnalyticalNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using NumericalMathEvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;
  NumericalSample operator() (const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Accessor to the input variables names */
  Description getInputVariablesNames() const;

  /** Accessor to the output variables names */
  Description getOutputVariablesNames() const;

  /** Accessor to the formulas */
  Description getFormulas() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:
  friend class AnalyticalNumericalMathGradientImplementation;
  friend class AnalyticalNumericalMathHessianImplementation;

  Description inputVariablesNames_;
  Description outputVariablesNames_;
  Description formulas_;

  /** A mathematical expression parser from the muParser library */
  mutable AnalyticalParser parser_;

}; /* class AnalyticalNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ANALYTICALNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX */
