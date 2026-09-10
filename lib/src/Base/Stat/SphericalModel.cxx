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

/* Comparison operators */
Bool SphericalModel::operator ==(const SphericalModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (radius_ == other.radius_);
}

Bool SphericalModel::equals(const CovarianceModelImplementation & other) const
{
  const SphericalModel * p_other = dynamic_cast<const SphericalModel *>(&other);
  return p_other && (*this == *p_other);
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

/* Gradient wrt the parameters */
Matrix SphericalModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "SphericalModel::parameterGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "SphericalModel::parameterGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (outputDimension_ != 1) return CovarianceModelImplementation::parameterGradient(s, t);
  const Point tau(s - t);
  Scalar normTauOverScale = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    normTauOverScale += dx * dx;
  }
  normTauOverScale = std::sqrt(normTauOverScale);
  const Scalar n = normTauOverScale / radius_;
  const Bool isZero = (normTauOverScale <= SpecFunc::ScalarEpsilon);
  const Bool isOutside = (n >= 1.0);
  const Scalar k = computeAsScalar(s, t);
  Point fullGradient(inputDimension_ + 3, 0.0);
  if (isZero)
  {
    // The nugget factor is the only parameter that affects the value at tau=0
    fullGradient[inputDimension_] = outputCovariance_(0, 0);
  }
  else if (!isOutside)
  {
    // k = amplitude^2 (1 - 1.5 n + 0.5 n^3), dk/dn = 1.5 amplitude^2 (n^2 - 1)
    const Scalar dkOverDn = 1.5 * amplitude_[0] * amplitude_[0] * (n * n - 1.0);
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
      fullGradient[i] = dkOverDn * (-tau[i] * tau[i] / (n * radius_ * radius_ * scale_[i] * scale_[i] * scale_[i]));
    // dk/dradius = dk/dn * dn/dradius, dn/dradius = -n/radius
    fullGradient[inputDimension_ + 2] = dkOverDn * (-n / radius_);
  }
  // Amplitude: k = amplitude^2 * rho
  fullGradient[inputDimension_ + 1] = 2.0 * k / amplitude_[0];
  return filterActiveParameterGradient(fullGradient);
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

/** Gradient wrt the first input point */
Matrix SphericalModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "SphericalModel::partialGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "SphericalModel::partialGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  Point tau(s - t);
  Scalar normTauOverScale = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    normTauOverScale += dx * dx;
  }
  normTauOverScale = std::sqrt(normTauOverScale);
  // At zero norm the gradient is not defined (conical kernel): fall back to the finite-difference implementation
  if (normTauOverScale <= SpecFunc::ScalarEpsilon) return CovarianceModelImplementation::partialGradient(s, t);
  const Scalar z = normTauOverScale / radius_;
  // Outside the support the kernel vanishes
  if (z >= 1.0) return Matrix(inputDimension_, 1);
  // k(s, t) = amplitude^2 (1 - 1.5 z + 0.5 z^3), z = ||tau/scale|| / radius
  // dk/dz = 1.5 amplitude^2 (z^2 - 1), dz/dtau_a = tau_a / (scale_a^2 normTauOverScale radius)
  const Scalar dkOverDz = amplitude_[0] * amplitude_[0] * 1.5 * (z * z - 1.0);
  Matrix gradient(inputDimension_, 1);
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
    gradient(a, 0) = dkOverDz * tau[a] / (scale_[a] * scale_[a] * normTauOverScale * radius_);
  return gradient;
}

/** Hessian wrt s */
SymmetricMatrix SphericalModel::partialHessian(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "SphericalModel::partialHessian, the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "SphericalModel::partialHessian, the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  Scalar norm = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = (s[i] - t[i]) / scale_[i];
    norm += dx * dx;
  }
  norm = std::sqrt(norm) / radius_;
  // At zero norm the Hessian is not defined: fall back to the finite-difference implementation
  if (norm <= SpecFunc::ScalarEpsilon) return CovarianceModelImplementation::partialHessian(s, t);
  // Outside the support the kernel vanishes
  if (norm >= 1.0) return SymmetricMatrix(inputDimension_);
  // k(s, t) = amplitude^2 (1 - 1.5 z + 0.5 z^3), z = ||tau/scale|| / radius
  // d^2 k / ds_a ds_b = u_a u_b 1.5 (1 + z^2) / z^3
  //   + delta_ab 1.5 (z^2 - 1) / (scale_a^2 z), with u_i = tau_i / scale_i^2
  const Scalar z = norm;
  const Scalar factor = amplitude_[0] * amplitude_[0] * 1.5 * (1.0 + z * z) / (z * z * z);
  const Scalar diagonalFactor = amplitude_[0] * amplitude_[0] * 1.5 * (z * z - 1.0) / z;
  const Scalar radiusSquared = radius_ * radius_;
  Point u(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) u[i] = (s[i] - t[i]) / (scale_[i] * scale_[i] * radiusSquared);
  SymmetricMatrix hessian(inputDimension_);
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
  {
    for (UnsignedInteger b = 0; b < a; ++b)
      hessian(a, b) = factor * u[a] * u[b];
    hessian(a, a) = factor * u[a] * u[a] + diagonalFactor / (scale_[a] * scale_[a] * radiusSquared);
  }
  return hessian;
}

END_NAMESPACE_OPENTURNS
