//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation gradient for elliptical
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
/* Comparison operator */
Bool NatafEllipticalDistributionGradient::operator ==(const NatafEllipticalDistributionGradient &) const
{
  return true;
}
Bool NatafEllipticalDistributionGradient::equals(const GradientImplementation & other) const
{
  const NatafEllipticalDistributionGradient * p_other = dynamic_cast<const NatafEllipticalDistributionGradient *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String NatafEllipticalDistributionGradient::__repr__() const
{
  return OSS() << "class=" << NatafEllipticalDistributionGradient::GetClassName()
         << " inverseCholesky=" << getConstant().transpose();
}

/* Parameters value accessor, as (mu, sigma).
 * The stored constant matrix is the transpose of the inverse Cholesky
 * factor Gamma = C^{-1} D^{-1}, so each row j is rescaled by
 * sigma_old[j] / sigma_new[j], with the old values read on the recorded
 * parameter vector. Without record the map is assumed consistent with the
 * provided values and kept as is.
 */
void NatafEllipticalDistributionGradient::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getInputDimension();
  if (parameter.getDimension() != 2 * dimension) throw InvalidArgumentException(HERE) << "Required parameter of dimension " << 2 * dimension << ", provided " << parameter.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if (!(parameter[dimension + i] > 0.0)) throw InvalidArgumentException(HERE) << "The sigma parameter must be positive, got sigma[" << i << "]=" << parameter[dimension + i];
  Matrix newConstant(getConstant());
  const Point recorded(GradientImplementation::getParameter());
  if (recorded.getDimension() > 0)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar newSigma = parameter[dimension + j];
      const Scalar oldSigma = recorded[dimension + j];
      Scalar factor = 1.0;
      if (newSigma != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        factor = oldSigma / newSigma;
      }
      for (UnsignedInteger i = 0; i < dimension; ++i)
        newConstant(j, i) = newConstant(j, i) * factor;
    }
  }
  setConstant(newConstant);
  GradientImplementation::setParameter(parameter);
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
