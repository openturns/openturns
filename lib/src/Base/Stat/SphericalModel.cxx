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
#include "openturns/SphericalModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SphericalModel
 */

CLASSNAMEINIT(SphericalModel)

static const Factory<SphericalModel> Factory_SphericalModel;

/* Constructor from input dimension */
SphericalModel::SphericalModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , radius_(1.0)
{
  isStationary_ = true;
  if (outputDimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << outputDimension_;
}

/* Constructor with parameters */
SphericalModel::SphericalModel(const Point & scale,
                               const Point & amplitude,
                               const Scalar radius)
  : CovarianceModelImplementation(scale, amplitude)
  , radius_(-1.0)
{
  isStationary_ = true;
  if (outputDimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << outputDimension_;
  setRadius(radius);
}

/* Virtual constructor */
SphericalModel * SphericalModel::clone() const
{
  return new SphericalModel(*this);
}

/* Computation of the representative function:
 * rho(tau) = amplitude_ * (1 - 0.5|tau/scale| (3 - (|tau/scale| / radius)^2)) for 0<=|tau/scale|<=radius, 0 otherwise
 */
Scalar SphericalModel::computeAsScalar(const Point &tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In SphericalModel::computeStandardRepresentative: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Scalar normTauOverScaleA = 0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    normTauOverScaleA += dx * dx;
  }
  normTauOverScaleA = sqrt(normTauOverScaleA);
  normTauOverScaleA /= radius_;
  if (normTauOverScaleA <= SpecFunc::ScalarEpsilon)
    return amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_);
  if (normTauOverScaleA >= 1.0)
    return 0.0;
  return amplitude_[0] * amplitude_[0] * (1.0 - 0.5 * normTauOverScaleA * (3.0 - normTauOverScaleA * normTauOverScaleA));
}

Scalar SphericalModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                                       const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar normTauOverScaleA = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / scale_[i];
    normTauOverScaleA += dx * dx;
  }
  normTauOverScaleA = sqrt(normTauOverScaleA) / radius_;
  if (normTauOverScaleA <= SpecFunc::ScalarEpsilon)
    return amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_);
  if (normTauOverScaleA >= 1.0) return 0.0;
  return amplitude_[0] * amplitude_[0] * (1.0 - 0.5 * normTauOverScaleA * (3.0 - normTauOverScaleA * normTauOverScaleA));
}

Scalar SphericalModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar normTauOverScaleA = std::abs(tau / scale_[0]) / radius_;
  if (normTauOverScaleA <= SpecFunc::ScalarEpsilon)
    return amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_);
  if (normTauOverScaleA >= 1.0)
    return 0.0;
  return amplitude_[0] * amplitude_[0] * (1.0 - 0.5 * normTauOverScaleA * (3.0 - normTauOverScaleA * normTauOverScaleA));
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
String SphericalModel::__str__(const String & ) const
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
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("radius_", radius_);
}

/* Method load() reloads the object from the StorageManager */
void SphericalModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("radius_", radius_);
}

END_NAMESPACE_OPENTURNS
