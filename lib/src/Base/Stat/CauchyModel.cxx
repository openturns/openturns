//                                               -*- C++ -*-
/**
 *  @brief
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
#include <cmath>

#include "openturns/CauchyModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Field.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CauchyModel
 */

CLASSNAMEINIT(CauchyModel)

static const Factory<CauchyModel> Factory_CauchyModel;

/* Constructor with parameters */
CauchyModel::CauchyModel()
  : SpectralModelImplementation()
{
  // Nothing to do
}

CauchyModel::CauchyModel(const Point & scale,
                         const Point & amplitude)
  : SpectralModelImplementation(scale, amplitude)
{
  // Nothing to do
}

CauchyModel::CauchyModel(const Point & scale,
                         const Point & amplitude,
                         const CorrelationMatrix & spatialCorrelation)
  : SpectralModelImplementation(scale, amplitude, spatialCorrelation)
{
  // Nothing to do
}

CauchyModel::CauchyModel(const Point & scale,
                         const CovarianceMatrix & spatialCovariance)
  : SpectralModelImplementation(scale, spatialCovariance)
{
  // Nothing to do
}

/* Virtual constructor */
CauchyModel * CauchyModel::clone() const
{
  return new CauchyModel(*this);
}

/* Computation of the spectral density function */
Complex CauchyModel::computeStandardRepresentative(const Scalar frequency) const

{
  Complex value = 1.0;
  for (UnsignedInteger k = 0; k < inputDimension_; ++k)
  {
    const Scalar scaledFrequency = 2.0 * M_PI * scale_[k] * std::abs(frequency);
    const Scalar scaledFrequencySquared = scaledFrequency * scaledFrequency;
    value *= (2.0 * scale_[k]) / (1.0 + scaledFrequencySquared);
  }
  return value;
}

/* String converter */
String CauchyModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CauchyModel::GetClassName();
  oss << " amplitude=" << amplitude_
      << " scale=" << scale_
      << " spatial correlation=" << outputCorrelation_
      << " isDiagonal=" << isDiagonal_;
  return oss;
}

/* String converter */
String CauchyModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << CauchyModel::GetClassName();
  oss << " amplitude=" << amplitude_
      << " scale=" << scale_;
  if (!isDiagonal_)
    oss << " spatial correlation=" << "\n" << offset << outputCorrelation_.__str__(offset);
  else
    oss << " no spatial correlation";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CauchyModel::save(Advocate & adv) const
{
  SpectralModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void CauchyModel::load(Advocate & adv)
{
  SpectralModelImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
