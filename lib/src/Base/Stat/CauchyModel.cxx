//                                               -*- C++ -*-
/**
 *  @brief
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
#include <cmath>

#include "openturns/CauchyModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

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
  if (scale.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: CauchyModel is only defined for input dimension 1, here scale dimension=" << scale.getDimension();
}

CauchyModel::CauchyModel(const Point & scale,
                         const Point & amplitude,
                         const CorrelationMatrix & spatialCorrelation)
  : SpectralModelImplementation(scale, amplitude, spatialCorrelation)
{
  if (scale.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: CauchyModel is only defined for input dimension 1, here scale dimension=" << scale.getDimension();
}

CauchyModel::CauchyModel(const Point & scale,
                         const CovarianceMatrix & spatialCovariance)
  : SpectralModelImplementation(scale, spatialCovariance)
{
  if (scale.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: CauchyModel is only defined for input dimension 1, here scale dimension=" << scale.getDimension();
}

/* Virtual constructor */
CauchyModel * CauchyModel::clone() const
{
  return new CauchyModel(*this);
}

/* Computation of the spectral density function */
Complex CauchyModel::computeStandardRepresentative(const Scalar frequency) const

{
  if (inputDimension_ != 1) throw InvalidArgumentException(HERE) << "Error: CauchyModel is only defined for input dimension 1, here input dimension=" << inputDimension_;
  const Scalar scaledFrequency = 2.0 * M_PI * scale_[0] * std::abs(frequency);
  const Scalar scaledFrequencySquared = scaledFrequency * scaledFrequency;
  const Complex value = (2.0 * scale_[0]) / (1.0 + scaledFrequencySquared);
  return value;
}

/* Scale accessor */
void CauchyModel::setScale(const Point & scale)
{
  if (scale.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: CauchyModel is only defined for input dimension 1, here scale dimension=" << scale.getDimension();
  SpectralModelImplementation::setScale(scale);
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
  if (inputDimension_ != 1) throw InvalidArgumentException(HERE) << "Error: CauchyModel is only defined for input dimension 1, here input dimension=" << inputDimension_;
}

END_NAMESPACE_OPENTURNS
