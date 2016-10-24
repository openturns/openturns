//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
  : StationaryCovarianceModel(NumericalPoint(spatialDimension, ResourceMap::GetAsNumericalScalar("GeneralizedExponential-DefaultTheta")), NumericalPoint(1, 1.0))
  , p_(1.0)
{
  activeParameter_.add(activeParameter_.getSize());// add p
}

/** Parameters constructor */
GeneralizedExponential::GeneralizedExponential(const NumericalPoint & scale,
    const NumericalScalar p)
  : StationaryCovarianceModel(scale, NumericalPoint(1, 1.0))
  , p_(0.0)
{
  setP(p);
  activeParameter_.add(activeParameter_.getSize());// add p
}

/** Parameters constructor */
GeneralizedExponential::GeneralizedExponential(const NumericalPoint & scale,
    const NumericalPoint & amplitude,
    const NumericalScalar p)
  : StationaryCovarianceModel(scale, amplitude)
  , p_(0.0)
{
  if (getDimension() != 1)
    throw InvalidArgumentException(HERE) << "In GeneralizedExponential::GeneralizedExponential, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getDimension() << ")";
  setP(p);
  activeParameter_.add(activeParameter_.getSize());// add p
}

/* Virtual constructor */
GeneralizedExponential * GeneralizedExponential::clone() const
{
  return new GeneralizedExponential(*this);
}

/* Computation of the covariance density function */
NumericalScalar GeneralizedExponential::computeStandardRepresentative(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar tauOverThetaNorm = tauOverTheta.norm();
  return tauOverThetaNorm <= SpecFunc::NumericalScalarEpsilon ? 1.0 + nuggetFactor_ : exp(-pow(tauOverThetaNorm, p_));
}

/* Gradient wrt s */
Matrix GeneralizedExponential::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar norm2 = tauOverTheta.normSquare();
  // For zero norm
  if (norm2 == 0.0)
  {
    // Infinite gradient for p < 1
    if (p_ < 1.0) return Matrix(spatialDimension_, 1, NumericalPoint(spatialDimension_, -SpecFunc::MaxNumericalScalar));
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


void GeneralizedExponential::setFullParameter(const NumericalPoint & parameter)
{
  CovarianceModelImplementation::setFullParameter(parameter);
  setP(parameter[parameter.getSize() - 1]);
}

NumericalPoint GeneralizedExponential::getFullParameter() const
{
  // Get the generic parameter
  NumericalPoint parameter(CovarianceModelImplementation::getFullParameter());
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
  if (p <= 0.0) throw InvalidArgumentException(HERE) << "Error: p must be positive.";
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
