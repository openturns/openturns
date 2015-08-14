//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "NumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductNumericalMathEvaluationImplementation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ProductNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathEvaluationImplementation>          EvaluationImplementation;

  /** Default constructor */
  ProductNumericalMathEvaluationImplementation(const EvaluationImplementation & p_leftEvaluation,
      const EvaluationImplementation & p_rightEvaluation);


  /** Virtual constructor */
  virtual ProductNumericalMathEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ProductNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  NumericalPoint operator() (const NumericalPoint & inP) const;

  NumericalSample operator() (const NumericalSample & inS) const;

  /** Parameters value and description accessor */
  virtual NumericalPointWithDescription getParameters() const;
  virtual void setParameters(const NumericalPointWithDescription & parameters);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  ProductNumericalMathEvaluationImplementation() {};
  friend class Factory<ProductNumericalMathEvaluationImplementation>;

private:

  /** The function f in h = f . g */
  EvaluationImplementation p_leftEvaluation_;

  /** The function g in h = f . g */
  EvaluationImplementation p_rightEvaluation_;

}; /* class ProductNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
