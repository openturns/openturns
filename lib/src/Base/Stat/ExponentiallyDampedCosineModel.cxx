//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/ExponentiallyDampedCosineModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExponentiallyDampedCosineModel
 */

CLASSNAMEINIT(ExponentiallyDampedCosineModel)

static const Factory<ExponentiallyDampedCosineModel> Factory_ExponentiallyDampedCosineModel;

/* Constructor from input dimension */
ExponentiallyDampedCosineModel::ExponentiallyDampedCosineModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , frequency_(1.0)
{
  isStationary_ = true;
}

/** Standard constructor with amplitude and scale parameters */
ExponentiallyDampedCosineModel::ExponentiallyDampedCosineModel(const Point & scale,
    const Point & amplitude,
    const Scalar frequency)
  : CovarianceModelImplementation(scale, amplitude)
  , frequency_(0.0)
{
  isStationary_ = true;
  if (outputDimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << outputDimension_;
  setFrequency(frequency);
}

/* Virtual constructor */
ExponentiallyDampedCosineModel * ExponentiallyDampedCosineModel::clone() const
{
  return new ExponentiallyDampedCosineModel(*this);
}

/* Computation of the covariance function, stationary interface
 * C_{0,0}(tau) = amplitude_ * exp(-|tau / scale_|) * cos(2 * pi * frequency_ * |tau / scale|)
 */
SquareMatrix ExponentiallyDampedCosineModel::operator() (const Point & tau) const
{
  SquareMatrix covarianceMatrix(outputDimension_, outputDimension_);

  covarianceMatrix(0, 0) = computeAsScalar(tau);
  return covarianceMatrix;
}

Scalar ExponentiallyDampedCosineModel::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In ExponentiallyDampedCosineModel::computeAsScalar: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Scalar absTau = 0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    absTau += dx * dx;
  }
  absTau = sqrt(absTau);
  if (absTau <= SpecFunc::ScalarEpsilon)
    return amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_);
  return amplitude_[0] * amplitude_[0] * exp(-absTau) * cos(2.0 * M_PI * frequency_ * absTau);
}

Scalar ExponentiallyDampedCosineModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  Scalar absTau = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / scale_[i];
    absTau += dx * dx;
  }
  absTau = sqrt(absTau);
  if (absTau <= SpecFunc::ScalarEpsilon)
    return amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_);
  return amplitude_[0] * amplitude_[0] * exp(-absTau) * cos(2.0 * M_PI * frequency_ * absTau);
}

Scalar ExponentiallyDampedCosineModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar absTau = std::abs(tau / scale_[0]);
  if (absTau <= SpecFunc::ScalarEpsilon)
    return amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_);
  return amplitude_[0] * amplitude_[0] * exp(-absTau) * cos(2.0 * M_PI * frequency_ * absTau);
}

/* String converter */
String ExponentiallyDampedCosineModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ExponentiallyDampedCosineModel::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_
      << " frequency=" << frequency_;
  return oss;
}

/* String converter */
String ExponentiallyDampedCosineModel::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << ExponentiallyDampedCosineModel::GetClassName()
      << "(scale=" << scale_
      << ", amplitude=" << amplitude_
      << ", frequency=" << frequency_
      << ")";
  return oss;
}

/* Frequency accessor */
void ExponentiallyDampedCosineModel::setFrequency(const Scalar frequency)
{
  if (!(frequency > 0.0)) throw InvalidArgumentException(HERE) << "Error: the frequency must be positive.";
  frequency_ = frequency;
}

Scalar ExponentiallyDampedCosineModel::getFrequency() const
{
  return frequency_;
}

void ExponentiallyDampedCosineModel::setFullParameter(const Point & parameter)
{
  CovarianceModelImplementation::setFullParameter(parameter);
  setFrequency(parameter[parameter.getSize() - 1]);
}

Point ExponentiallyDampedCosineModel::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific one
  parameter.add(frequency_);
  return parameter;
}

Description ExponentiallyDampedCosineModel::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  description.add("frequency");
  return description;
}

/* Method save() stores the object through the StorageManager */
void ExponentiallyDampedCosineModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute( "frequency_", frequency_);
}

/* Method load() reloads the object from the StorageManager */
void ExponentiallyDampedCosineModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute( "frequency_", frequency_);
}

END_NAMESPACE_OPENTURNS
