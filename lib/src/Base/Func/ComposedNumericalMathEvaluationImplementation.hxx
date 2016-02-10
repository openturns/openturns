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

#ifndef OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedNumericalMathEvaluationImplementation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ComposedNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathEvaluationImplementation> EvaluationImplementation;

  /** Default constructor */
  ComposedNumericalMathEvaluationImplementation(const EvaluationImplementation & p_leftFunction,
      const EvaluationImplementation & p_rightFunction);


  /** Virtual constructor */
  virtual ComposedNumericalMathEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ComposedNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using NumericalMathEvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;

  NumericalSample operator() (const NumericalSample & inS) const;

  /** Parameters value and description accessor */
  virtual NumericalPointWithDescription getParameter() const;
  virtual void setParameter(const NumericalPointWithDescription & parameters);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Composed implementation accessor */
  EvaluationImplementation getLeftEvaluationImplementation() const;
  EvaluationImplementation getRightEvaluationImplementation() const;

protected:

  ComposedNumericalMathEvaluationImplementation() {};
  friend class Factory<ComposedNumericalMathEvaluationImplementation>;

private:

  /** The function f in h = f o g */
  EvaluationImplementation p_leftFunction_;

  /** The function g in h = f o g */
  EvaluationImplementation p_rightFunction_;

}; /* class ComposedNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
