//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_NATAFELLIPTICALCOPULAGRADIENT_HXX
#define OPENTURNS_NATAFELLIPTICALCOPULAGRADIENT_HXX

#include "OTprivate.hxx"
#include "NumericalMathGradientImplementation.hxx"
#include "Matrix.hxx"
#include "SquareMatrix.hxx"
#include "NumericalPoint.hxx"
#include "Distribution.hxx"
#include "StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafEllipticalCopulaGradient
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API NatafEllipticalCopulaGradient
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  NatafEllipticalCopulaGradient();

  /** Parameter constructor */
  NatafEllipticalCopulaGradient(const Distribution & standardDistribution,
                                const TriangularMatrix & inverseCholesky);


  /** Virtual constructor */
  virtual NatafEllipticalCopulaGradient * clone() const;

  /** Gradient */
  Matrix gradient(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  Distribution standardDistribution_;
  TriangularMatrix inverseCholesky_;

}; /* NatafEllipticalCopulaGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFELLIPTICALCOPULAGRADIENT_HXX */
