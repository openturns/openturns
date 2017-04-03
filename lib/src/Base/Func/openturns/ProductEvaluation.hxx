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
 * \author $LastChangedBy: lebrun $
 * \date   $LastChangedDate: 2008-07-03 08:31:34 +0200 (jeu, 03 jui 2008) $
 */

#ifndef OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ProductEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<EvaluationImplementation>          EvaluationPointer;

  /** Default constructor */
  ProductEvaluation(const EvaluationPointer & p_leftEvaluation,
      const EvaluationPointer & p_rightEvaluation);


  /** Virtual constructor */
  virtual ProductEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ProductEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  NumericalPoint operator() (const NumericalPoint & inP) const;

  NumericalSample operator() (const NumericalSample & inS) const;

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameter);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  ProductEvaluation() {};
  friend class Factory<ProductEvaluation>;

private:

  /** The function f in h = f . g */
  EvaluationPointer p_leftEvaluation_;

  /** The function g in h = f . g */
  EvaluationPointer p_rightEvaluation_;

}; /* class ProductEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
