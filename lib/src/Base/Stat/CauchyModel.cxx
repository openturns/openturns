//                                               -*- C++ -*-
/**
 *  @brief
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
#include <cmath>

#include "CauchyModel.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"
#include "Field.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CauchyModel
 */

CLASSNAMEINIT(CauchyModel);

static const Factory<CauchyModel> RegisteredFactory;

/* Constructor with parameters */
CauchyModel::CauchyModel()
  : SpectralModelImplementation()
{
  // Nothing to do
}

CauchyModel::CauchyModel(const NumericalPoint & amplitude,
                         const NumericalPoint & scale)
  : SpectralModelImplementation(amplitude, scale)
{
  // Nothing to do
}

CauchyModel::CauchyModel(const NumericalPoint & amplitude,
                         const NumericalPoint & scale,
                         const CorrelationMatrix & spatialCorrelation)
  : SpectralModelImplementation(amplitude, scale, spatialCorrelation)
{
  // Nothing to do
}

CauchyModel::CauchyModel(const NumericalPoint & scale,
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
HermitianMatrix CauchyModel::operator() (const NumericalScalar frequency) const
{
  // TODO change the implementation
  const NumericalScalar scaledFrequencySquared(pow(2.0 * M_PI * fabs(frequency), 2));
  HermitianMatrix spectralDensityMatrix(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    spectralDensityMatrix(i, i) = 2.0 * amplitude_[i] * amplitude_[i] * scale_[i] / (1.0 + scale_[i] * scale_[i] * scaledFrequencySquared);
  if (!isDiagonal_)
    for (UnsignedInteger j = 0; j < dimension_ ; ++j)
      for (UnsignedInteger i = j + 1; i < dimension_ ; ++i)
        spectralDensityMatrix(i, j) = amplitude_[i] * spatialCorrelation_(i, j) * amplitude_[j] * (scale_[i] + scale_[j]) / (0.25 * (scale_[i] / scale_[j] + 2.0 + scale_[j] / scale_[i]) + scale_[i] * scale_[j] * scaledFrequencySquared);

  return spectralDensityMatrix;
}

/* String converter */
String CauchyModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CauchyModel::GetClassName();
  oss << " amplitude=" << amplitude_
      << " scale=" << scale_
      << " spatial correlation=" << spatialCorrelation_
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
    oss << " spatial correlation=\n" << spatialCorrelation_.__str__(offset);
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
