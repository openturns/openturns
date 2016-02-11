//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation gradient for elliptical
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_INVERSENATAFELLIPTICALDISTRIBUTIONGRADIENT_HXX
#define OPENTURNS_INVERSENATAFELLIPTICALDISTRIBUTIONGRADIENT_HXX

#include "OTprivate.hxx"
#include "ConstantNumericalMathGradientImplementation.hxx"
#include "SquareMatrix.hxx"
#include "SymmetricTensor.hxx"
#include "StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNatafEllipticalDistributionGradient
 *
 * This class offers an interface for the InverseNataf gradient for elliptical distributions
 */
class OT_API InverseNatafEllipticalDistributionGradient
  : public ConstantNumericalMathGradientImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  InverseNatafEllipticalDistributionGradient();

  /** Parameter constructor */
  explicit InverseNatafEllipticalDistributionGradient(const SquareMatrix & inverseCholesky);

  /** Virtual constructor */
  virtual InverseNatafEllipticalDistributionGradient * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

}; /* InverseNatafEllipticalDistributionGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENATAFELLIPTICALDISTRIBUTIONGRADIENT_HXX */
