//                                             -*- C++ -*-
/**
 *  @brief Implementation of BlendedStep
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

#include "openturns/BlendedStep.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BlendedStep)

static const Factory<BlendedStep> Factory_BlendedStep;

/* Default constructor */
BlendedStep::BlendedStep()
  : FiniteDifferenceStepImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
BlendedStep::BlendedStep(const Point & epsilon, const Scalar eta)
  : FiniteDifferenceStepImplementation(epsilon)
{
  setEta( Point( epsilon.getDimension(), eta ) );
}

/* Parameters constructor */
BlendedStep::BlendedStep(const Point & epsilon, const Point & eta)
  : FiniteDifferenceStepImplementation(epsilon)
{
  setEta( eta );
}

/* String converter */
String BlendedStep::__repr__() const
{
  OSS oss;
  oss << "class=" << BlendedStep::GetClassName()
      << " derived from " << FiniteDifferenceStepImplementation::__repr__()
      << " eta=" << eta_;
  return oss;
}

/* Virtual constructor */
BlendedStep * BlendedStep::clone() const
{
  return new BlendedStep(*this);
}

/* Compute step */
Point BlendedStep::operator()(const Point & inP) const
{
  const UnsignedInteger dimension = epsilon_.getDimension();
  if (dimension != inP.getDimension()) throw InvalidArgumentException(HERE) << "Invalid dimension eps:" << dimension << " x:" << inP.getDimension();
  Point result( epsilon_ );
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    result[i] *= ( std::abs( inP[i] ) + eta_[i] );
    if (!(result[i] >= SpecFunc::MinScalar )) throw InvalidArgumentException(HERE) << "Null step for component " << i << ": eps=" << epsilon_[i] << " x=" << inP[i];
  }
  return result;
}

/* Method save() stores the object through the StorageManager */
void BlendedStep::save(Advocate & adv) const
{
  FiniteDifferenceStepImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void BlendedStep::load(Advocate & adv)
{
  FiniteDifferenceStepImplementation::load(adv);
}

/* Eta accessor */
void BlendedStep::setEta(const Point & eta)
{
  const UnsignedInteger dimension = epsilon_.getDimension();
  if (eta.getDimension() != epsilon_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid dimension: eta dimension (" << eta.getDimension() << ") doesn't match epsilon dimension (" << epsilon_.getDimension() << ").";
  for( UnsignedInteger i = 0; i < dimension; ++ i )
  {
    if (!(eta[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Negative eta component " << i;
  }
  eta_ = eta;
}

Point BlendedStep::getEta() const
{
  return eta_;
}

END_NAMESPACE_OPENTURNS


