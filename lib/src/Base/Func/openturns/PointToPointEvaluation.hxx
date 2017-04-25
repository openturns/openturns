//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations or a point to field function
 *        and a field to point function
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

#ifndef OPENTURNS_POINTTOPOINTEVALUATION_HXX
#define OPENTURNS_POINTTOPOINTEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/FieldToPointFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointToPointEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations or a field to point and a point to field function.
 */
class OT_API PointToPointEvaluation
  : public EvaluationImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  PointToPointEvaluation();

  /** Parameter constructor */
  PointToPointEvaluation(const Function & leftFunction,
                         const Function & rightFunction);

  /** parameter constructor */
  PointToPointEvaluation(const FieldToPointFunction & fieldToPoint,
                         const PointToFieldFunction & pointToField);

  /** Virtual constructor */
  virtual PointToPointEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const PointToPointEvaluation & other) const;

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
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Composed implementation accessor */
  Function getLeftFunction() const;
  Function getRightFunction() const;
  PointToFieldFunction getPointToFieldFunction() const;
  FieldToPointFunction getFieldToPointFunction() const;

protected:


private:

  /** Flag to tell if it is a composition of functions or a
   * composition of point to field and field to point functions */
  Bool isFunctionComposition_;

  /** The function f in h = f o g */
  Function leftFunction_;

  /** The function g in h = f o g */
  Function rightFunction_;

  /** The field to point function */
  FieldToPointFunction fieldToPointFunction_;

  /** The point to field function */
  PointToFieldFunction pointToFieldFunction_;

}; /* class PointToPointEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOPOINTEVALUATION_HXX */
