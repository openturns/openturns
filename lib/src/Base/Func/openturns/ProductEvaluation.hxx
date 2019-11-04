//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ProductEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ProductEvaluation(const Evaluation & leftEvaluation,
                    const Evaluation & rightEvaluation);


  /** Virtual constructor */
  virtual ProductEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ProductEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  Point operator() (const Point & inP) const;

  Sample operator() (const Sample & inS) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameter);

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
  Evaluation leftEvaluation_;

  /** The function g in h = f . g */
  Evaluation rightEvaluation_;

}; /* class ProductEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
