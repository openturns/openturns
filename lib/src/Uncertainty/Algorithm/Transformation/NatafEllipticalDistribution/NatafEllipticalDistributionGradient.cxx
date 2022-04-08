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
#include "openturns/OTprivate.hxx"
#include "openturns/NatafEllipticalDistributionGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafEllipticalDistributionGradient
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */

CLASSNAMEINIT(NatafEllipticalDistributionGradient)

static const Factory<NatafEllipticalDistributionGradient> Factory_NatafEllipticalDistributionGradient;

/* Default constructor */
NatafEllipticalDistributionGradient::NatafEllipticalDistributionGradient()
  : ConstantGradient()
{
  // Nothing to do
}

/* Parameter constructor */
NatafEllipticalDistributionGradient::NatafEllipticalDistributionGradient(const TriangularMatrix & inverseCholesky)
  : ConstantGradient(inverseCholesky.transpose())
{
  // Nothing to do
}

/* Virtual constructor */
NatafEllipticalDistributionGradient * NatafEllipticalDistributionGradient::clone() const
{
  return new NatafEllipticalDistributionGradient(*this);
}

/* String converter */
String NatafEllipticalDistributionGradient::__repr__() const
{
  return OSS() << "class=" << NatafEllipticalDistributionGradient::GetClassName()
         << " inverseCholesky=" << getConstant().transpose();
}

/* Method save() stores the object through the StorageManager */
void NatafEllipticalDistributionGradient::save(Advocate & adv) const
{
  ConstantGradient::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void NatafEllipticalDistributionGradient::load(Advocate & adv)
{
  ConstantGradient::load(adv);
}

END_NAMESPACE_OPENTURNS
