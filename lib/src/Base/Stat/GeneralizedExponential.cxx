//                                               -*- C++ -*-
/**
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
#include "openturns/GeneralizedExponential.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExponential);

static const Factory<GeneralizedExponential> Factory_GeneralizedExponential;


/* Default constructor */
GeneralizedExponential::GeneralizedExponential(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(Point(spatialDimension, ResourceMap::GetAsNumericalScalar("GeneralizedExponential-DefaultTheta")), Point(1, 1.0))
  , p_(1.0)
{
  // Nothing to do
}

/** Parameters constructor */
GeneralizedExponential::GeneralizedExponential(const Point & scale,
    const NumericalScalar p)
  : StationaryCovarianceModel(scale, Point(1, 1.0))
  , p_(0.0) // To pass the test !(p_ == p)
{
  setP(p);
}

/** Parameters constructor */
GeneralizedExponential::GeneralizedExponential(const Point & scale,
    const Point & amplitude,
    const NumericalScalar p)
  : StationaryCovarianceModel(scale, amplitude)
  , p_(0.0) // To pass the test !(p_ == p)
{
  if (getDimension() != 1)
    throw InvalidArgumentException(HERE) << "In GeneralizedExponential::GeneralizedExponential, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getDimension() << ")";
  setP(p);
}

/* Virtual constructor */
GeneralizedExponential * GeneralizedExponential::clone() const
{
  return new GeneralizedExponential(*this);
}

/* Computation of the covariance density function */
NumericalScalar GeneralizedExponential::computeStandardRepresentative(const Point & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  Point tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar tauOverThetaNorm = tauOverTheta.norm();
  return tauOverThetaNorm <= SpecFunc::NumericalScalarEpsilon ? 1.0 + nuggetFactor_ : exp(-pow(tauOverThetaNorm, p_));
}

/* Gradient wrt s */
Matrix GeneralizedExponential::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const Point tau(s - t);
  Point tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar norm2 = tauOverTheta.normSquare();
  // For zero norm
  if (norm2 == 0.0)
  {
    // Negative infinite gradient for p < 1
    if (p_ < 1.0) return Matrix(spatialDimension_, 1, Point(spatialDimension_, -SpecFunc::MaxNumericalScalar));
    // Non-zero gradient for p == 1
    if (p_ == 1.0)
    {
      Matrix gradient(spatialDimension_, 1);
      for (UnsignedInteger i = 0; i < spatialDimension_; ++i) gradient(i, 0) = - amplitude_[0] * amplitude_[0] / scale_[i];
      return gradient;
    }
    // Zero gradient for p > 1
    return Matrix(spatialDimension_, 1);
  }
  // General case
  const NumericalScalar exponent = -std::pow(sqrt(norm2), p_);
  const NumericalScalar value = p_ * exponent * std::exp(exponent) / norm2;
  // Needs tau/theta ==> reuse same NP
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] /= scale_[i];
  return Matrix(spatialDimension_, 1, tauOverTheta * value) * amplitude_[0] * amplitude_[0];
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
String GeneralizedExponential::__str__(const String & offset) const
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
NumericalScalar GeneralizedExponential::getP() const
{
  return p_;
}

void GeneralizedExponential::setP(const NumericalScalar p)
{
  if (!(p > 0.0)) throw InvalidArgumentException(HERE) << "Error: p must be positive.";
  if (!(p <= 2.0)) throw InvalidArgumentException(HERE) << "Error: p must be less or equal to 2.";
  p_ = p;
}

/* Method save() stores the object through the StorageManager */
void GeneralizedExponential::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("p_", p_);
}

/* Method load() reloads the object from the StorageManager */
void GeneralizedExponential::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("p_", p_);
}

END_NAMESPACE_OPENTURNS
