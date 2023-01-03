//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_NATAFELLIPTICALCOPULAHESSIAN_HXX
#define OPENTURNS_NATAFELLIPTICALCOPULAHESSIAN_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafEllipticalCopulaHessian
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API NatafEllipticalCopulaHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  NatafEllipticalCopulaHessian();

  /** Parameter constructor */
  NatafEllipticalCopulaHessian(const Distribution & standardDistribution,
                               const TriangularMatrix & inverseCholesky);


  /** Virtual constructor */
  NatafEllipticalCopulaHessian * clone() const override;

  /** Hessian */
  SymmetricTensor hessian(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  Distribution standardDistribution_;
  TriangularMatrix inverseCholesky_;

}; /* NatafEllipticalCopulaHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFELLIPTICALCOPULAHESSIAN_HXX */
