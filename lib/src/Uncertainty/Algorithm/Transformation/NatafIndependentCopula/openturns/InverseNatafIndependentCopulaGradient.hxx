//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for independent
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INVERSENATAFINDEPENDENTCOPULAGRADIENT_HXX
#define OPENTURNS_INVERSENATAFINDEPENDENTCOPULAGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNatafIndependentCopulaGradient
 *
 * This class offers an interface for the InverseNataf function for independent distributions
 */
class OT_API InverseNatafIndependentCopulaGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseNatafIndependentCopulaGradient();

  /** Parameter constructor */
  explicit InverseNatafIndependentCopulaGradient(const UnsignedInteger dimension);


  /** Virtual constructor */
  InverseNatafIndependentCopulaGradient * clone() const override;

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
  UnsignedInteger dimension_;

}; /* InverseNatafIndependentCopulaGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENATAFINDEPENDENTCOPULAGRADIENT_HXX */
