//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation gradient for elliptical
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
#include "openturns/InverseNatafEllipticalDistributionGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalDistributionGradient
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */

CLASSNAMEINIT(InverseNatafEllipticalDistributionGradient)

static const Factory<InverseNatafEllipticalDistributionGradient> Factory_InverseNatafEllipticalDistributionGradient;

/* Default constructor */
InverseNatafEllipticalDistributionGradient::InverseNatafEllipticalDistributionGradient():
  ConstantGradient()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalDistributionGradient::InverseNatafEllipticalDistributionGradient(const SquareMatrix & cholesky):
  ConstantGradient(cholesky.transpose())
{
  // Nothing to do
}

/* Parameter constructor with recorded (mu, sigma) scales */
InverseNatafEllipticalDistributionGradient::InverseNatafEllipticalDistributionGradient(const SquareMatrix & cholesky,
    const Point & parameters):
  ConstantGradient(cholesky.transpose())
{
  GradientImplementation::setParameter(parameters);
}

/* Virtual constructor */
InverseNatafEllipticalDistributionGradient * InverseNatafEllipticalDistributionGradient::clone() const
{
  return new InverseNatafEllipticalDistributionGradient(*this);
}
/* Comparison operator */
Bool InverseNatafEllipticalDistributionGradient::operator ==(const InverseNatafEllipticalDistributionGradient &) const
{
  return true;
}
Bool InverseNatafEllipticalDistributionGradient::equals(const GradientImplementation & other) const
{
  const InverseNatafEllipticalDistributionGradient * p_other = dynamic_cast<const InverseNatafEllipticalDistributionGradient *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String InverseNatafEllipticalDistributionGradient::__repr__() const
{
  return OSS() << "class=" << InverseNatafEllipticalDistributionGradient::GetClassName()
         << " cholesky=" << getConstant().transpose();
}

/* Parameters value accessor, as (mu, sigma).
 * The stored constant matrix is the transpose of the Cholesky factor
 * L = D L_c, so each column j is rescaled by sigma_new[j] / sigma_old[j],
 * with the old values read on the recorded parameter vector.
 */
void InverseNatafEllipticalDistributionGradient::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getInputDimension();
  if (parameter.getDimension() != 2 * dimension) throw InvalidArgumentException(HERE) << "Required parameter of dimension " << 2 * dimension << ", provided " << parameter.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if (!(parameter[dimension + i] >= 0.0)) throw InvalidArgumentException(HERE) << "The sigma parameter must be non-negative, got sigma[" << i << "]=" << parameter[dimension + i];
  Matrix newConstant(getConstant());
  const Point recorded(GradientImplementation::getParameter());
  if (recorded.getDimension() > 0)
  {
    if (recorded.getDimension() != 2 * dimension) throw InvalidArgumentException(HERE) << "Recorded parameter of dimension " << recorded.getDimension() << " is inconsistent with dimension " << dimension;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar newSigma = parameter[dimension + j];
      const Scalar oldSigma = recorded[dimension + j];
      Scalar factor = 1.0;
      if (newSigma != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        if (!(newSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update to a degenerate transformation, please rebuild it";
        factor = newSigma / oldSigma;
      }
      for (UnsignedInteger i = 0; i < dimension; ++i)
        newConstant(i, j) = newConstant(i, j) * factor;
    }
  }
  else
  {
    // No recorded scales (direct construction): require a diagonal map,
    // for which the scales are recovered exactly, else rebuild from a distribution.
    Bool diagonal = true;
    for (UnsignedInteger i = 0; i < dimension; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if ((i != j) && (newConstant(i, j) != 0.0)) diagonal = false;
    if (!diagonal) throw InvalidArgumentException(HERE) << "Cannot update the parameters of a directly constructed correlated transformation without recorded scales, please rebuild it from a distribution";
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar newSigma = parameter[dimension + j];
      const Scalar oldSigma = newConstant(j, j);
      if (newSigma != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        if (!(newSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update to a degenerate transformation, please rebuild it";
        const Scalar factor = newSigma / oldSigma;
        for (UnsignedInteger i = 0; i < dimension; ++i)
          newConstant(i, j) = newConstant(i, j) * factor;
      }
    }
  }
  setConstant(newConstant);
  GradientImplementation::setParameter(parameter);
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalDistributionGradient::save(Advocate & adv) const
{
  ConstantGradient::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalDistributionGradient::load(Advocate & adv)
{
  ConstantGradient::load(adv);
}

END_NAMESPACE_OPENTURNS
