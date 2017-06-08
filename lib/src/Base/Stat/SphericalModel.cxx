//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SphericalModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SphericalModel
 */

CLASSNAMEINIT(SphericalModel);

static const Factory<SphericalModel> Factory_SphericalModel;

/* Constructor from spatial dimension */
SphericalModel::SphericalModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
  , radius_(1.0)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
  definesComputeStandardRepresentative_ = true;
}

/* Constructor with parameters */
SphericalModel::SphericalModel(const Point & scale,
                               const Point & amplitude,
                               const Scalar radius)
  : StationaryCovarianceModel(scale, amplitude)
  , radius_(-1.0)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
  definesComputeStandardRepresentative_ = true;
  setRadius(radius);
}

/* Virtual constructor */
SphericalModel * SphericalModel::clone() const
{
  return new SphericalModel(*this);
}

CovarianceMatrix SphericalModel::operator() (const Point & tau) const
{
  CovarianceMatrix covarianceMatrix(dimension_);
  covarianceMatrix(0, 0) = computeAsScalar(tau);
  return covarianceMatrix;
}

Scalar SphericalModel::computeAsScalar(const Point & tau) const
{
  return amplitude_[0] * computeStandardRepresentative(tau);
}

/* Computation of the representative function:
 * rho(tau) = amplitude_ * (1 - 0.5|tau/scale| (3 - (|tau/scale| / radius)^2)) for 0<=|tau/scale|<=radius, 0 otherwise
 */
Scalar SphericalModel::computeStandardRepresentative(const Point & tau) const
{
  if (tau.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In SphericalModel::computeStandardRepresentative: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  Point tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const Scalar normTauOverScaleA = tauOverTheta.norm() / radius_;
  if (normTauOverScaleA <= SpecFunc::ScalarEpsilon) return 1.0 + nuggetFactor_;
  if (normTauOverScaleA >= 1.0) return 0.0;
  return 1.0 - 0.5 * normTauOverScaleA * (3.0 - normTauOverScaleA * normTauOverScaleA);
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix SphericalModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size = timeGrid.getN();
  const UnsignedInteger fullSize = size;
  const Scalar timeStep = timeGrid.getStep();

  CovarianceMatrix cov(fullSize);

  for (UnsignedInteger diag = 0; diag < size; ++diag)
  {
    const Scalar covTau = computeAsScalar(Point(1, diag * timeStep));
    for (UnsignedInteger i = 0; i < size - diag; ++i)
      cov(i, i + diag) = covTau;
  }

  return cov;
}

void SphericalModel::setFullParameter(const Point & parameter)
{
  CovarianceModelImplementation::setFullParameter(parameter);
  setRadius(parameter[parameter.getSize() - 1]);
}

Point SphericalModel::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific one
  parameter.add(radius_);
  return parameter;
}

Description SphericalModel::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  description.add("radius");
  return description;
}

/* String converter */
String SphericalModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SphericalModel::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_
      << " radius=" << radius_;
  return oss;
}

/* String converter */
String SphericalModel::__str__(const String & offset) const
{
  OSS oss;
  oss << SphericalModel::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ", radius=" << radius_
      << ")";
  return oss;
}

/* Radius accessor */
Scalar SphericalModel::getRadius() const
{
  return radius_;
}

void SphericalModel::setRadius(const Scalar radius)
{
  if (!(radius > 0.0)) throw InvalidArgumentException(HERE) << "Error: the radius must be positive.";
  radius_ = radius;
}

/* Method save() stores the object through the StorageManager */
void SphericalModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("radius_", radius_);
}

/* Method load() reloads the object from the StorageManager */
void SphericalModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("radius_", radius_);
}

END_NAMESPACE_OPENTURNS
