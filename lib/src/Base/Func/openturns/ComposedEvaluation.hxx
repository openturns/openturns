//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
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

#ifndef OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ComposedEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ComposedEvaluation(const Evaluation & leftFunction,
                     const Evaluation & rightFunction);


  /** Virtual constructor */
  virtual ComposedEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ComposedEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const;

  Sample operator() (const Sample & inS) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameter);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Get the i-th marginal function */
  Evaluation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Evaluation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Composed implementation accessor */
  Evaluation getLeftEvaluation() const;
  Evaluation getRightEvaluation() const;

protected:

  ComposedEvaluation() {};
  friend class Factory<ComposedEvaluation>;

private:

  /** The function f in h = f o g */
  Evaluation leftFunction_;

  /** The function g in h = f o g */
  Evaluation rightFunction_;

}; /* class ComposedEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
