//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformationHessian evaluation for elliptical
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
 *
 */
#ifndef OPENTURNS_MARGINALTRANSFORMATIONHESSIAN_HXX
#define OPENTURNS_MARGINALTRANSFORMATIONHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/Point.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MarginalTransformationHessian
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API MarginalTransformationHessian
  : public HessianImplementation
{
  CLASSNAME
public:

  typedef Collection<Distribution>           DistributionCollection;

  /** Default constructor */
  MarginalTransformationHessian();

  /** Parameter constructor */
  MarginalTransformationHessian(const MarginalTransformationEvaluation & evaluation);


  /** Virtual constructor */
  virtual MarginalTransformationHessian * clone() const;

  /** Hessian */
  SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  // The evaluation class associated with the hessian
  Pointer<MarginalTransformationEvaluation> p_evaluation_;

}; /* MarginalTransformationHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALTRANSFORMATIONHESSIAN_HXX */
