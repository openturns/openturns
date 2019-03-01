//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  : StationaryCovarianceModel(inputDimension)
  , frequency_(1.0)
{
  definesComputeStandardRepresentative_ = true;
}

/** Standard constructor with amplitude and scale parameters */
ExponentiallyDampedCosineModel::ExponentiallyDampedCosineModel(const Point & scale,
    const Point & amplitude,
    const Scalar frequency)
  : StationaryCovarianceModel(scale, amplitude)
  , frequency_(0.0)
{
  if (outputDimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << outputDimension_;
  setFrequency(frequency);
  definesComputeStandardRepresentative_ = true;
}

/* Virtual constructor */
ExponentiallyDampedCosineModel * ExponentiallyDampedCosineModel::clone() const
{
  return new ExponentiallyDampedCosineModel(*this);
}

/* Computation of the covariance function, stationary interface
 * C_{0,0}(tau) = amplitude_ * exp(-|tau / scale_|) * cos(2 * pi * frequency_ * |tau / scale|)
 */
CovarianceMatrix ExponentiallyDampedCosineModel::operator() (const Point & tau) const
{
  CovarianceMatrix covarianceMatrix(outputDimension_);

  covarianceMatrix(0, 0) = computeAsScalar(tau);
  return covarianceMatrix;
}

Scalar ExponentiallyDampedCosineModel::computeAsScalar(const Point & tau) const
{
  return amplitude_[0] * computeStandardRepresentative(tau);
}

Scalar ExponentiallyDampedCosineModel::computeStandardRepresentative(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In ExponentiallyDampedCosineModel::computeStandardRepresentative: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();

  Point theta(scale_);
  switch (scaleParametrization_)
  {
    case STANDARD:
      // nothing to do
      break;
    case INVERSE:
      for(UnsignedInteger i = 0; i < inputDimension_; ++i)
        theta[i] = 1.0 / scale_[i];
      break;
    case LOGINVERSE:
      for(UnsignedInteger i = 0; i < inputDimension_; ++i)
        theta[i] = std::exp(- scale_[i]);
      break;
  }

  Point tauOverTheta(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverTheta[i] = tau[i] / theta[i];

  const Scalar absTau = tauOverTheta.norm();
  if (absTau <= SpecFunc::ScalarEpsilon) return 1.0 + nuggetFactor_;
  return exp(-absTau) * cos(2.0 * M_PI * absTau);
}

Scalar ExponentiallyDampedCosineModel::computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Point theta(scale_);
  switch (scaleParametrization_)
  {
    case STANDARD:
      // nothing to do
      break;
    case INVERSE:
      for(UnsignedInteger i = 0; i < inputDimension_; ++i)
        theta[i] = 1.0 / scale_[i];
      break;
    case LOGINVERSE:
      for(UnsignedInteger i = 0; i < inputDimension_; ++i)
        theta[i] = std::exp(- scale_[i]);
      break;
  }

  Scalar absTau = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / theta[i];
    absTau += dx * dx;
  }
  absTau = sqrt(absTau);
  if (absTau <= SpecFunc::ScalarEpsilon) return 1.0 + nuggetFactor_;
  return exp(-absTau) * cos(2.0 * M_PI * absTau);
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix ExponentiallyDampedCosineModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size = timeGrid.getN();
  const UnsignedInteger fullSize = size * outputDimension_;
  const Scalar timeStep = timeGrid.getStep();

  CovarianceMatrix cov(fullSize);

  // The stationary property of this model allows to optimize the discretization
  // over a regular time grid: the large covariance matrix is block-diagonal
  // Fill the matrix by block-diagonal
  // The main diagonal has a specific treatment as only its lower triangular part
  // has to be copied
  for (UnsignedInteger diag = 0; diag < size; ++diag)
  {
    const Scalar covTau = computeAsScalar(Point(1, diag * timeStep));
    for (UnsignedInteger i = 0; i < size - diag; ++i) cov(i, i + diag) = covTau;
  }

  return cov;
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
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute( "frequency_", frequency_);
}

/* Method load() reloads the object from the StorageManager */
void ExponentiallyDampedCosineModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute( "frequency_", frequency_);
}

END_NAMESPACE_OPENTURNS
