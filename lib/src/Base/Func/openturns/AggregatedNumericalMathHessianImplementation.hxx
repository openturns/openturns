//                                               -*- C++ -*-
/**
 *  @brief The hessian part of linear combination of polynomials
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_AGGREGATEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_AGGREGATEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX

#include "openturns/NumericalMathHessianImplementation.hxx"
#include "openturns/AggregatedNumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class AggregatedNumericalMathHessianImplementation
 *
 * The hessian part of linear combination of polynomials
 */

class OT_API AggregatedNumericalMathHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;
public:


  // friend class Factory<AggregatedNumericalMathHessianImplementation>;

  /** Default constructor */
  AggregatedNumericalMathHessianImplementation();

  /** Parameters constructor */
  AggregatedNumericalMathHessianImplementation(const AggregatedNumericalMathEvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual AggregatedNumericalMathHessianImplementation * clone() const;

  /** Hessian method */
  SymmetricTensor hessian(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:
  /** The associated evaluation */
  AggregatedNumericalMathEvaluationImplementation evaluation_;

} ; /* class AggregatedNumericalMathHessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_AGGREGATEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX */
