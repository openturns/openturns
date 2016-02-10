//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

static const Factory<SphericalModel> RegisteredFactory;

/* Constructor from spatial dimension */
SphericalModel::SphericalModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
  , a_(1.0)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
}

/* Constructor with parameters */
SphericalModel::SphericalModel(const UnsignedInteger spatialDimension,
                               const NumericalPoint & amplitude,
                               const NumericalPoint & scale,
                               const NumericalScalar a)
  : StationaryCovarianceModel(spatialDimension, amplitude, scale)
  , a_(a)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
  if (a <= 0)
    throw InvalidArgumentException(HERE) << "In SphericalModel::SphericalModel, the ray parameter a should be stricly positive. Here, got (a=" << a << ")";
}

/* Virtual constructor */
SphericalModel * SphericalModel::clone() const
{
  return new SphericalModel(*this);
}

CovarianceMatrix SphericalModel::operator() (const NumericalPoint & tau) const
{
  CovarianceMatrix covarianceMatrix(dimension_);
  covarianceMatrix(0, 0) = computeAsScalar(tau);
  return covarianceMatrix;
}

NumericalScalar SphericalModel::computeAsScalar(const NumericalPoint & tau) const
{
  return amplitude_[0] * computeStandardRepresentative(tau);
}

/* Computation of the representative function:
 * rho(tau) = amplitude_ * (1 - 0.5|tau/scale| / a) (3 - (|tau/a| / a)^2)) for 0<=|tau/scale|<=a, 0 otherwise
 */
NumericalScalar SphericalModel::computeStandardRepresentative(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In SphericalModel::computeStandardRepresentative: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar absTau(tauOverTheta.norm1());
  if (absTau == 0.0) return 1.0 + nuggetFactor_;
  if (absTau >= 1.0) return 0.0;
  return 1.0 - 0.5 * absTau * (3.0 - absTau * absTau);
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
      << " scale=" << scale_
      << " a=" << a_;
  return oss;
}

/* String converter */
String SphericalModel::__str__(const String & offset) const
{
  OSS oss;
  oss << SphericalModel::GetClassName()
      << "(input dimension=" << spatialDimension_
      << ", theta=" << scale_.__str__()
      << ", sigma=" << amplitude_.__str__()
      << ", scale=" << scale_
      << ", a=" << a_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SphericalModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("a_", a_);
}

/* Method load() reloads the object from the StorageManager */
void SphericalModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("a_", a_);
}

END_NAMESPACE_OPENTURNS
