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
#include "openturns/GeneralizedExponential.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExponential)

static const Factory<GeneralizedExponential> Factory_GeneralizedExponential;


/* Default constructor */
GeneralizedExponential::GeneralizedExponential(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(Point(inputDimension, ResourceMap::GetAsScalar("GeneralizedExponential-DefaultTheta")), Point(1, 1.0))
  , p_(1.0)
{
  isStationary_ = true;
}

/** Parameters constructor */
GeneralizedExponential::GeneralizedExponential(const Point & scale,
    const Scalar p)
  : CovarianceModelImplementation(scale, Point(1, 1.0))
  , p_(0.0) // To pass the test !(p_ == p)
{
  isStationary_ = true;
  setP(p);
}

/** Parameters constructor */
GeneralizedExponential::GeneralizedExponential(const Point & scale,
    const Point & amplitude,
    const Scalar p)
  : CovarianceModelImplementation(scale, amplitude)
  , p_(0.0) // To pass the test !(p_ == p)
{
  isStationary_ = true;
  if (getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In GeneralizedExponential::GeneralizedExponential, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getOutputDimension() << ")";
  setP(p);
}

/* Virtual constructor */
GeneralizedExponential * GeneralizedExponential::clone() const
{
  return new GeneralizedExponential(*this);
}

/* Computation of the covariance density function */
Scalar GeneralizedExponential::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Point tauOverTheta(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const Scalar tauOverThetaNorm = tauOverTheta.norm();
  return tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-pow(tauOverThetaNorm, p_));
}

Scalar GeneralizedExponential::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar tauOverThetaNorm = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / scale_[i];
    tauOverThetaNorm += dx * dx;
  }
  tauOverThetaNorm = sqrt(tauOverThetaNorm);
  return tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-pow(tauOverThetaNorm, p_));
}

Scalar GeneralizedExponential::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar tauOverThetaNorm = std::abs(tau / scale_[0]);
  return tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-pow(tauOverThetaNorm, p_));
}

/* Gradient wrt s */
Matrix GeneralizedExponential::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const Point tau(s - t);
  Point tauOverTheta(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const Scalar norm2 = tauOverTheta.normSquare();
  // For zero norm
  if (norm2 == 0.0)
  {
    // Negative infinite gradient for p < 1
    if (p_ < 1.0) return Matrix(inputDimension_, 1, Point(inputDimension_, SpecFunc::LowestScalar));
    // Non-zero gradient for p == 1
    if (p_ == 1.0)
    {
      Matrix gradient(inputDimension_, 1);
      for (UnsignedInteger i = 0; i < inputDimension_; ++i) gradient(i, 0) = - amplitude_[0] * amplitude_[0] / scale_[i];
      return gradient;
    }
    // Zero gradient for p > 1
    return Matrix(inputDimension_, 1);
  }
  // General case
  const Scalar exponent = -std::pow(sqrt(norm2), p_);
  const Scalar value = p_ * exponent * std::exp(exponent) / norm2;
  // Needs tau/theta ==> reuse same NP
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) tauOverTheta[i] /= scale_[i];
  return Matrix(inputDimension_, 1, tauOverTheta * value) * amplitude_[0] * amplitude_[0];
}


void GeneralizedExponential::setFullParameter(const Point & parameter)
{
  CovarianceModelImplementation::setFullParameter(parameter);
  setP(parameter[parameter.getSize() - 1]);
}

Point GeneralizedExponential::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific one
  parameter.add(p_);
  return parameter;
}

Description GeneralizedExponential::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  description.add("p");
  return description;
}

/* String converter */
String GeneralizedExponential::__repr__() const
{
  OSS oss;
  oss << "class=" << GeneralizedExponential::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_
      << " p=" << p_;
  return oss;
}

/* String converter */
String GeneralizedExponential::__str__(const String & ) const
{
  OSS oss;
  oss << GeneralizedExponential::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ", p=" << p_
      << ")";
  return oss;
}

/* P accessor */
Scalar GeneralizedExponential::getP() const
{
  return p_;
}

void GeneralizedExponential::setP(const Scalar p)
{
  if (!(p > 0.0)) throw InvalidArgumentException(HERE) << "Error: p must be positive.";
  if (!(p <= 2.0)) throw InvalidArgumentException(HERE) << "Error: p must be less or equal to 2.";
  p_ = p;
}

/* Method save() stores the object through the StorageManager */
void GeneralizedExponential::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("p_", p_);
}

/* Method load() reloads the object from the StorageManager */
void GeneralizedExponential::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("p_", p_);
}

END_NAMESPACE_OPENTURNS
