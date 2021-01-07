//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INVERSENATAFELLIPTICALCOPULAHESSIAN_HXX
#define OPENTURNS_INVERSENATAFELLIPTICALCOPULAHESSIAN_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNatafEllipticalCopulaHessian
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */
class OT_API InverseNatafEllipticalCopulaHessian
  : public HessianImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseNatafEllipticalCopulaHessian();

  /** Parameter constructor */
  InverseNatafEllipticalCopulaHessian(const Distribution & standardDistribution,
                                      const TriangularMatrix & cholesky);


  /** Virtual constructor */
  InverseNatafEllipticalCopulaHessian * clone() const override;

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
  TriangularMatrix cholesky_;

}; /* InverseNatafEllipticalCopulaHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENATAFELLIPTICALCOPULAHESSIAN_HXX */
