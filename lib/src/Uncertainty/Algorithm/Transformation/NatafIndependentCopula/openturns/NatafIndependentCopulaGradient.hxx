//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for independent
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
#ifndef OPENTURNS_NATAFINDEPENDENTCOPULAGRADIENT_HXX
#define OPENTURNS_NATAFINDEPENDENTCOPULAGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafIndependentCopulaGradient
 *
 * This class offers an interface for the Nataf function for independent distributions
 */
class OT_API NatafIndependentCopulaGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  NatafIndependentCopulaGradient();

  /** Parameter constructor */
  explicit NatafIndependentCopulaGradient(const UnsignedInteger dimension);


  /** Virtual constructor */
  NatafIndependentCopulaGradient * clone() const override;

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

}; /* NatafIndependentCopulaGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFINDEPENDENTCOPULAGRADIENT_HXX */
