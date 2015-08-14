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
#include "SphericalModel.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SphericalModel
 */

CLASSNAMEINIT(SphericalModel);

static Factory<SphericalModel> RegisteredFactory("SphericalModel");

/* Constructor from spatial dimension */
SphericalModel::SphericalModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
}

/* Constructor with parameters */
SphericalModel::SphericalModel(const UnsignedInteger spatialDimension,
                               const NumericalPoint & amplitude,
                               const NumericalPoint & scale)
  : StationaryCovarianceModel(spatialDimension, amplitude, scale)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
}

/* Virtual constructor */
SphericalModel * SphericalModel::clone() const
{
  return new SphericalModel(*this);
}


/* Computation of the covariance function, stationary interface
 * C_{0,0}(tau) = amplitude_ * (1 - |tau|/(2a)(3 - |tau|^2/a^2)) for 0<=|tau<=a, 0 otherwise
 */
CovarianceMatrix SphericalModel::operator() (const NumericalPoint & tau) const
{
  CovarianceMatrix covarianceMatrix(dimension_);
  covarianceMatrix(0, 0) = computeAsScalar(tau);
  return covarianceMatrix;
}

NumericalScalar SphericalModel::computeAsScalar(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  const NumericalScalar absTau(tau.norm1());
  return (absTau == 0.0 ? amplitude_[0] * (1.0 + nuggetFactor_) : (absTau >= scale_[0] ? 0.0 : amplitude_[0] * (1.0 - 0.5 * absTau / scale_[0] * (3.0 - absTau * absTau / (scale_[0] * scale_[0])))));
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix SphericalModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size(timeGrid.getN());
  const UnsignedInteger fullSize(size);
  const NumericalScalar timeStep(timeGrid.getStep());

  CovarianceMatrix cov(fullSize);

  for (UnsignedInteger diag = 0; diag < size; ++diag)
  {
    const NumericalScalar covTau(computeAsScalar(NumericalPoint(1, diag * timeStep)));
    for (UnsignedInteger i = 0; i < size - diag; ++i)
      cov(i, i + diag) = covTau;
  }

  return cov;
}

/* Is it a stationary model ? */
Bool SphericalModel::isStationary() const
{
  return true;
}

/* String converter */
String SphericalModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SphericalModel::GetClassName();
  oss << " input dimension=" << spatialDimension_
      << " amplitude=" << amplitude_
      << " scale=" << scale_;
  return oss;
}

/* String converter */
String SphericalModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << SphericalModel::GetClassName();
  oss << " input dimension=" << spatialDimension_
      << " amplitude=" << amplitude_[0]
      << " scale=" << scale_[0];
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SphericalModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void SphericalModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
}

END_NAMESPACE_OPENTURNS
