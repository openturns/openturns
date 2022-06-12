//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation gradient for elliptical
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_NATAFELLIPTICALDISTRIBUTIONGRADIENT_HXX
#define OPENTURNS_NATAFELLIPTICALDISTRIBUTIONGRADIENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ConstantGradient.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafEllipticalDistributionGradient
 *
 * This class offers an interface for the Nataf gradient for elliptical distributions
 */
class OT_API NatafEllipticalDistributionGradient
  : public ConstantGradient
{
  CLASSNAME
public:


  /** Default constructor */
  NatafEllipticalDistributionGradient();

  /** Parameter constructor */
  NatafEllipticalDistributionGradient(const TriangularMatrix & inverseCholesky);

  /** Virtual constructor */
  NatafEllipticalDistributionGradient * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

}; /* NatafEllipticalDistributionGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFELLIPTICALDISTRIBUTIONGRADIENT_HXX */
