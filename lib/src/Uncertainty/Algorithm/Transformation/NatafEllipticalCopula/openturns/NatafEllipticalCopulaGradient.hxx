//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
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
#ifndef OPENTURNS_NATAFELLIPTICALCOPULAGRADIENT_HXX
#define OPENTURNS_NATAFELLIPTICALCOPULAGRADIENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafEllipticalCopulaGradient
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API NatafEllipticalCopulaGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  NatafEllipticalCopulaGradient();

  /** Parameter constructor */
  NatafEllipticalCopulaGradient(const Distribution & standardDistribution,
                                const TriangularMatrix & inverseCholesky);


  /** Virtual constructor */
  NatafEllipticalCopulaGradient * clone() const override;

  /** Gradient */
  Matrix gradient(const Point & inP) const override;

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

}; /* NatafEllipticalCopulaGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFELLIPTICALCOPULAGRADIENT_HXX */
