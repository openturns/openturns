//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation hessian for elliptical
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
#include "InverseNatafEllipticalDistributionHessian.hxx"
#include "SymmetricTensor.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalDistributionHessian
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */

CLASSNAMEINIT(InverseNatafEllipticalDistributionHessian);

static const Factory<InverseNatafEllipticalDistributionHessian> RegisteredFactory;

/* Default constructor */
InverseNatafEllipticalDistributionHessian::InverseNatafEllipticalDistributionHessian():
  ConstantNumericalMathHessianImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalDistributionHessian::InverseNatafEllipticalDistributionHessian(const UnsignedInteger dimension):
  ConstantNumericalMathHessianImplementation(SymmetricTensor(dimension, dimension))
{
  // Nothing to do
}

/* Virtual constructor */
InverseNatafEllipticalDistributionHessian * InverseNatafEllipticalDistributionHessian::clone() const
{
  return new InverseNatafEllipticalDistributionHessian(*this);
}

/* String converter */
String InverseNatafEllipticalDistributionHessian::__repr__() const
{
  return OSS() << "class=" << InverseNatafEllipticalDistributionHessian::GetClassName();
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalDistributionHessian::save(Advocate & adv) const
{
  ConstantNumericalMathHessianImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalDistributionHessian::load(Advocate & adv)
{
  ConstantNumericalMathHessianImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
