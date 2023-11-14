//                                               -*- C++ -*-
/**
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
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AbsoluteExponential)

static const Factory<AbsoluteExponential> Factory_AbsoluteExponential;


/* Constructor based on input dimension */
AbsoluteExponential::AbsoluteExponential(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(Point(inputDimension, ResourceMap::GetAsScalar("AbsoluteExponential-DefaultTheta")), Point(1, 1.0))
{
  isStationary_ = true;
}

/** Parameters constructor */
AbsoluteExponential::AbsoluteExponential(const Point & scale)
  : CovarianceModelImplementation(scale, Point(1, 1.0))
{
  isStationary_ = true;
}

/** Parameters constructor */
AbsoluteExponential::AbsoluteExponential(const Point & scale,
    const Point & amplitude)
  : CovarianceModelImplementation(scale, amplitude)
{
  isStationary_ = true;
  if (getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In AbsoluteExponential::AbsoluteExponential, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getOutputDimension() << ")";
}

/* Virtual constructor */
AbsoluteExponential * AbsoluteExponential::clone() const
{
  return new AbsoluteExponential(*this);
}

/* Computation of the covariance function */
Scalar AbsoluteExponential::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Scalar tauOverThetaNorm = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverThetaNorm += std::abs(tau[i] / scale_[i]);
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  return tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance(0, 0) * (1.0 + nuggetFactor_) : outputCovariance(0, 0) * exp(-tauOverThetaNorm);
}

Scalar AbsoluteExponential::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar tauOverThetaNorm = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    tauOverThetaNorm += std::abs(*s_it - *t_it) / scale_[i];
  }
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  return tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance(0, 0) * (1.0 + nuggetFactor_) : outputCovariance(0, 0) * exp(-tauOverThetaNorm);
}

Scalar AbsoluteExponential::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar tauOverThetaNorm = std::abs(tau / scale_[0]);
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  return tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance(0, 0) * (1.0 + nuggetFactor_) : outputCovariance(0, 0) * exp(-tauOverThetaNorm);
}

/* Gradient */
Matrix AbsoluteExponential::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  Scalar norm1 = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
    norm1 += std::abs(s[i] - t[i]) / scale_[i];
  // For zero norm
  // Norm1 is null if all elements are zero
  // In that case gradient is not defined
  Matrix gradient(inputDimension_, 1);
  if (norm1 == 0.0)
  {
    for (UnsignedInteger i = 0; i < inputDimension_; ++i) gradient(i, 0) = -amplitude_[0] * amplitude_[0] / scale_[i];
    return gradient;
  }
  // General case
  const Scalar value = std::exp(-norm1);

  // Gradient take as factor sign(tau_i) /theta_i
  Scalar gradientI = 0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    gradientI = (amplitude_[0] * amplitude_[0]) * value / scale_[i];
    if ((s[i] - t[i]) > 0)
      gradientI *= -1.0;
    gradient(i, 0) = gradientI;
  }
  return gradient;
}

/* String converter */
String AbsoluteExponential::__repr__() const
{
  OSS oss;
  oss << "class=" << AbsoluteExponential::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_;
  return oss;
}

/* String converter */
String AbsoluteExponential::__str__(const String & ) const
{
  OSS oss;
  oss << AbsoluteExponential::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void AbsoluteExponential::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void AbsoluteExponential::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
