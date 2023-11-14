//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation hessian for elliptical
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
#include "openturns/OTprivate.hxx"
#include "openturns/NatafEllipticalDistributionHessian.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafEllipticalDistributionHessian
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */

CLASSNAMEINIT(NatafEllipticalDistributionHessian)

static const Factory<NatafEllipticalDistributionHessian> Factory_NatafEllipticalDistributionHessian;

/* Default constructor */
NatafEllipticalDistributionHessian::NatafEllipticalDistributionHessian():
  ConstantHessian()
{
  // Nothing to do
}

/* Parameter constructor */
NatafEllipticalDistributionHessian::NatafEllipticalDistributionHessian(const UnsignedInteger dimension):
  ConstantHessian(SymmetricTensor(dimension, dimension))
{
  // Nothing to do
}

/* Virtual constructor */
NatafEllipticalDistributionHessian * NatafEllipticalDistributionHessian::clone() const
{
  return new NatafEllipticalDistributionHessian(*this);
}

/* String converter */
String NatafEllipticalDistributionHessian::__repr__() const
{
  return OSS() << "class=" << NatafEllipticalDistributionHessian::GetClassName();
}

/* Method save() stores the object through the StorageManager */
void NatafEllipticalDistributionHessian::save(Advocate & adv) const
{
  ConstantHessian::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void NatafEllipticalDistributionHessian::load(Advocate & adv)
{
  ConstantHessian::load(adv);
}

END_NAMESPACE_OPENTURNS
