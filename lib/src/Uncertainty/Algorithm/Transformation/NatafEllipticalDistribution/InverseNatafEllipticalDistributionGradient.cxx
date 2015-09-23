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
#include "OTprivate.hxx"
#include "InverseNatafEllipticalDistributionGradient.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalDistributionGradient
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */

CLASSNAMEINIT(InverseNatafEllipticalDistributionGradient);

static const Factory<InverseNatafEllipticalDistributionGradient> RegisteredFactory;

/* Default constructor */
InverseNatafEllipticalDistributionGradient::InverseNatafEllipticalDistributionGradient():
  ConstantNumericalMathGradientImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalDistributionGradient::InverseNatafEllipticalDistributionGradient(const SquareMatrix & cholesky):
  ConstantNumericalMathGradientImplementation(cholesky.transpose())
{
  // Nothing to do
}

/* Virtual constructor */
InverseNatafEllipticalDistributionGradient * InverseNatafEllipticalDistributionGradient::clone() const
{
  return new InverseNatafEllipticalDistributionGradient(*this);
}

/* String converter */
String InverseNatafEllipticalDistributionGradient::__repr__() const
{
  return OSS() << "class=" << InverseNatafEllipticalDistributionGradient::GetClassName()
         << " cholesky=" << getConstant().transpose();
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalDistributionGradient::save(Advocate & adv) const
{
  ConstantNumericalMathGradientImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalDistributionGradient::load(Advocate & adv)
{
  ConstantNumericalMathGradientImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
