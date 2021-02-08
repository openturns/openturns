//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SquaredExponential.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquaredExponential)

static const Factory<SquaredExponential> Factory_SquaredExponential;


/* Default constructor */
SquaredExponential::SquaredExponential(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(Point(inputDimension, ResourceMap::GetAsScalar("SquaredExponential-DefaultTheta")), Point(1, 1.0))
{
  isStationary_ = true;
}

/** Parameters constructor */
SquaredExponential::SquaredExponential(const Point & scale)
  : CovarianceModelImplementation(scale, Point(1, 1.0))
{
  isStationary_ = true;
}

/** Parameters constructor */
SquaredExponential::SquaredExponential(const Point & scale,
                                       const Point & amplitude)
  : CovarianceModelImplementation(scale, amplitude)
{
  isStationary_ = true;
  if (getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In SquaredExponential::SquaredExponential, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getOutputDimension() << ")";
}

/* Virtual constructor */
SquaredExponential * SquaredExponential::clone() const
{
  return new SquaredExponential(*this);
}

/* Computation of the covariance function */
Scalar SquaredExponential::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Scalar tauOverTheta2 = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    tauOverTheta2 += dx * dx;
  }
  return tauOverTheta2 <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-0.5 * tauOverTheta2);
}

Scalar SquaredExponential::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar tauOverTheta2 = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / scale_[i];
    tauOverTheta2 += dx * dx;
  }
  return tauOverTheta2 <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-0.5 * tauOverTheta2);
}

Scalar SquaredExponential::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar tauOverTheta2 = tau * tau / (scale_[0]  * scale_[0]);
  return tauOverTheta2 <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-0.5 * tauOverTheta2);
}

/* Gradient */
Matrix SquaredExponential::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const Point tau = s - t;
  Point tauOverTheta(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const Scalar norm2 = tauOverTheta.normSquare();
  // For zero norm
  if (norm2 == 0.0) return Matrix(inputDimension_, 1);
  // General case
  const Scalar value = -std::exp(-0.5 * norm2);
  // Compute tau/theta^2
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverTheta[i] /= scale_[i];
  return Matrix(inputDimension_, 1, tauOverTheta * value) * amplitude_[0] * amplitude_[0];
}

/* String converter */
String SquaredExponential::__repr__() const
{
  OSS oss;
  oss << "class=" << SquaredExponential::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_;
  return oss;
}

/* String converter */
String SquaredExponential::__str__(const String & ) const
{
  OSS oss;
  oss << SquaredExponential::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SquaredExponential::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void SquaredExponential::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
