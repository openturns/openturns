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

/* Comparison operators */
Bool ExponentiallyDampedCosineModel::operator ==(const ExponentiallyDampedCosineModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (frequency_ == other.frequency_);
}

Bool ExponentiallyDampedCosineModel::equals(const CovarianceModelImplementation & other) const
{
  const ExponentiallyDampedCosineModel * p_other = dynamic_cast<const ExponentiallyDampedCosineModel *>(&other);
  return p_other && (*this == *p_other);
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

/* Gradient wrt the parameters */
Matrix ExponentiallyDampedCosineModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "ExponentiallyDampedCosineModel::parameterGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "ExponentiallyDampedCosineModel::parameterGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (outputDimension_ != 1) return CovarianceModelImplementation::parameterGradient(s, t);
  const Point tau(s - t);
  Scalar norm = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    norm += dx * dx;
  }
  norm = std::sqrt(norm);
  const Bool isZero = (norm <= SpecFunc::ScalarEpsilon);
  const Scalar k = computeAsScalar(s, t);
  Point fullGradient(inputDimension_ + 3, 0.0);
  if (!isZero)
  {
    // k = amplitude^2 exp(-norm) cos(omega norm), omega = 2 pi frequency
    const Scalar omega = 2.0 * M_PI * frequency_;
    const Scalar rho = std::exp(-norm);
    const Scalar cosOmegaNorm = std::cos(omega * norm);
    const Scalar sinOmegaNorm = std::sin(omega * norm);
    const Scalar dkOverDnorm = amplitude_[0] * amplitude_[0] * rho * (-cosOmegaNorm - omega * sinOmegaNorm);
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
      fullGradient[i] = dkOverDnorm * (-tau[i] * tau[i] / (norm * scale_[i] * scale_[i] * scale_[i]));
    // dk/dfrequency = -2 pi norm amplitude^2 exp(-norm) sin(omega norm)
    fullGradient[inputDimension_ + 2] = -2.0 * M_PI * norm * amplitude_[0] * amplitude_[0] * rho * sinOmegaNorm;
  }
  else
  {
    // The nugget factor is the only parameter that affects the value at tau=0
    fullGradient[inputDimension_] = outputCovariance_(0, 0);
  }
  // Amplitude: k = amplitude^2 * rho
  fullGradient[inputDimension_ + 1] = 2.0 * k / amplitude_[0];
  return filterActiveParameterGradient(fullGradient);
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

/** Gradient wrt the first input point */
Matrix ExponentiallyDampedCosineModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "ExponentiallyDampedCosineModel::partialGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "ExponentiallyDampedCosineModel::partialGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  Point tau(s - t);
  Scalar z = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    z += dx * dx;
  }
  z = std::sqrt(z);
  // At zero norm the gradient is not defined (conical kernel): fall back to the finite-difference implementation
  if (z <= SpecFunc::ScalarEpsilon) return CovarianceModelImplementation::partialGradient(s, t);
  // k(s, t) = amplitude^2 exp(-z) cos(omega z), z = ||tau/scale||, omega = 2 pi frequency
  // dk/dz = amplitude^2 exp(-z) (-cos(omega z) - omega sin(omega z)), dz/dtau_a = tau_a / (scale_a^2 z)
  const Scalar omega = 2.0 * M_PI * frequency_;
  const Scalar dkOverDz = amplitude_[0] * amplitude_[0] * std::exp(-z) * (-std::cos(omega * z) - omega * std::sin(omega * z));
  Matrix gradient(inputDimension_, 1);
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
    gradient(a, 0) = dkOverDz * tau[a] / (scale_[a] * scale_[a] * z);
  return gradient;
}

/** Hessian wrt s */
SymmetricMatrix ExponentiallyDampedCosineModel::partialHessian(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "ExponentiallyDampedCosineModel::partialHessian, the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "ExponentiallyDampedCosineModel::partialHessian, the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  Scalar norm = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = (s[i] - t[i]) / scale_[i];
    norm += dx * dx;
  }
  norm = std::sqrt(norm);
  // At zero norm the Hessian is not defined: fall back to the finite-difference implementation
  if (norm <= SpecFunc::ScalarEpsilon) return CovarianceModelImplementation::partialHessian(s, t);
  // k(s, t) = amplitude^2 exp(-z) cos(omega z), z = ||tau/scale||, omega = 2 pi frequency
  // d^2 k / ds_a ds_b = u_a u_b [rho''(z) z - rho'(z)] / z^3
  //   + delta_ab rho'(z) / (scale_a^2 z), with u_i = tau_i / scale_i^2
  const Scalar z = norm;
  const Scalar omega = 2.0 * M_PI * frequency_;
  const Scalar exponent = std::exp(-z);
  const Scalar cosOmegaZ = std::cos(omega * z);
  const Scalar sinOmegaZ = std::sin(omega * z);
  // [rho''(z) z - rho'(z)] / z^3 = exp(-z) [cos(1 + z - omega^2 z) + omega sin(1 + 2z)] / z^3
  const Scalar factor = amplitude_[0] * amplitude_[0] * exponent * (cosOmegaZ * (1.0 + z * (1.0 - omega * omega)) + sinOmegaZ * omega * (1.0 + 2.0 * z)) / (z * z * z);
  // rho'(z) / z = -exp(-z) (cos + omega sin) / z
  const Scalar diagonalFactor = -amplitude_[0] * amplitude_[0] * exponent * (cosOmegaZ + omega * sinOmegaZ) / z;
  Point u(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) u[i] = (s[i] - t[i]) / (scale_[i] * scale_[i]);
  SymmetricMatrix hessian(inputDimension_);
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
  {
    for (UnsignedInteger b = 0; b < a; ++b)
      hessian(a, b) = factor * u[a] * u[b];
    hessian(a, a) = factor * u[a] * u[a] + diagonalFactor / (scale_[a] * scale_[a]);
  }
  return hessian;
}

END_NAMESPACE_OPENTURNS
