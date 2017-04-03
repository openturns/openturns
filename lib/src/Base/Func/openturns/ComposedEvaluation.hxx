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

#ifndef OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ComposedEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<EvaluationImplementation> EvaluationPointer;

  /** Default constructor */
  ComposedEvaluation(const EvaluationPointer & p_leftFunction,
      const EvaluationPointer & p_rightFunction);


  /** Virtual constructor */
  virtual ComposedEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ComposedEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;

  NumericalSample operator() (const NumericalSample & inS) const;

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameter);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

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
  EvaluationPointer getLeftEvaluationImplementation() const;
  EvaluationPointer getRightEvaluationImplementation() const;

protected:

  ComposedEvaluation() {};
  friend class Factory<ComposedEvaluation>;

private:

  /** The function f in h = f o g */
  EvaluationPointer p_leftFunction_;

  /** The function g in h = f o g */
  EvaluationPointer p_rightFunction_;

}; /* class ComposedEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
