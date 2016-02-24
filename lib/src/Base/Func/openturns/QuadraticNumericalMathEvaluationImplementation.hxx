//                                               -*- C++ -*-
/**
 * @brief Class for a quadratic numerical math function implementation
 *        of the form y = constant + <linear, x-c> + <<quadratic, x-c>, x-c>
 *        where constant is a dim(y) numerical point, c a dim(x) numerical
 *        point, linear is a dim(x) by dim(y) matrix, quadratic is a
 *        dim(x) by dim(x) by dim(y) symmetric tensor and <linear, x>
 *        means Transpose(linear).x, <quadratic, x> means
 *        Transpose_kj(quadratic).x
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

#ifndef OPENTURNS_QUADRATICNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_QUADRATICNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class QuadraticNumericalMathEvaluationImplementation
 *
 * This class offers an interface for an real numerical mathematical evaluation
 * implementations into the platform. It realizes the computation of the image vector
 * of a vector (aka NumericalPoint) through the evaluation. The
 * evaluation may be loaded in an external way through a wrapper.
 */
class OT_API QuadraticNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  QuadraticNumericalMathEvaluationImplementation();

  /** Parameter constructor */
  QuadraticNumericalMathEvaluationImplementation(const NumericalPoint & center,
      const NumericalPoint & constant,
      const Matrix & linear,
      const SymmetricTensor & quadratic);


  /** Virtual constructor */
  virtual QuadraticNumericalMathEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const QuadraticNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for the center */
  NumericalPoint getCenter() const;

  /** Accessor for the constant term */
  NumericalPoint getConstant() const;

  /** Accessor for the linear term */
  Matrix getLinear() const;

  /** Accessor for the quadratic term */
  SymmetricTensor getQuadratic() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  NumericalPoint center_;
  NumericalPoint constant_;
  Matrix linear_;
  SymmetricTensor quadratic_;
}; /* class QuadraticNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX */
