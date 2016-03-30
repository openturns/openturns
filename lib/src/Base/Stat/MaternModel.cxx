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
#include "openturns/MaternModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaternModel);

static const Factory<MaternModel> RegisteredFactory;


/* Default constructor */
MaternModel::MaternModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(spatialDimension, ResourceMap::GetAsNumericalScalar("MaternModel-DefaultTheta")))
  , nu_(ResourceMap::GetAsNumericalScalar("MaternModel-DefaultNu"))
  , sqrt2nuOverTheta_(NumericalPoint(spatialDimension, sqrt(2.0 * nu_) / ResourceMap::GetAsNumericalScalar("MaternModel-DefaultTheta") ))
{
  // Compute the normalization factor
  logNormalizationFactor_ = (1.0 - nu_) * std::log(2.0) - SpecFunc::LogGamma(nu_);
}

/* Parameters constructor */
MaternModel::MaternModel(const UnsignedInteger spatialDimension,
                         const NumericalScalar theta,
                         const NumericalScalar nu)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(spatialDimension, theta))
  , nu_(nu)
  , sqrt2nuOverTheta_(NumericalPoint(spatialDimension, 0.0))
{
  initialize();
}

/** Parameters constructor */
MaternModel::MaternModel(const NumericalPoint & theta,
                         const NumericalScalar nu)
  : StationaryCovarianceModel( NumericalPoint(1, 1.0), theta)
  , nu_(nu)
  , sqrt2nuOverTheta_(NumericalPoint(theta.getDimension(), 0.0))
{
  initialize();
}

/** Parameters constructor */
MaternModel::MaternModel(const NumericalPoint & theta,
                         const NumericalPoint & sigma,
                         const NumericalScalar nu)
  : StationaryCovarianceModel(sigma, theta)
  , nu_(nu)
  , sqrt2nuOverTheta_(NumericalPoint(theta.getDimension(), 0.0))
{
  if (getDimension() != 1)
    throw InvalidArgumentException(HERE) << "In MaternModel::MaternModel, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getDimension() <<")";
  initialize();
}

void MaternModel::initialize()
{
  if (nu_ <= 0.0) throw InvalidArgumentException(HERE) << "In MaternModel::MaternModel, nu must be positive, here nu=" << nu_;
  // Compute the normalization factor
  logNormalizationFactor_ = (1.0 - nu_) * std::log(2.0) - SpecFunc::LogGamma(nu_);
  // Compute usefull scaling factor
  for(UnsignedInteger i = 0; i < spatialDimension_; ++i) sqrt2nuOverTheta_[i] = sqrt(2.0 * nu_) / scale_[i];
}

/* Virtual constructor */
MaternModel * MaternModel::clone() const
{
  return new MaternModel(*this);
}

/* Computation of the covariance  function */
NumericalScalar MaternModel::computeStandardRepresentative(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  NumericalPoint scaledTau(spatialDimension_);
  for(UnsignedInteger i = 0; i < spatialDimension_; ++i) scaledTau[i] = tau[i] * sqrt2nuOverTheta_[i];
  const NumericalScalar scaledPoint(scaledTau.norm());
  if (std::abs(scaledPoint) <= SpecFunc::NumericalScalarEpsilon)
    return 1.0 + nuggetFactor_;
  else
    return exp(logNormalizationFactor_ + nu_ * std::log(scaledPoint) + SpecFunc::LogBesselK(nu_, scaledPoint));
}

/* Gradient */
Matrix MaternModel::partialGradient(const NumericalPoint & s,
                                    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  NumericalPoint scaledTau(spatialDimension_);
  for(UnsignedInteger i = 0; i < spatialDimension_; ++i) scaledTau[i] = tau[i] * sqrt2nuOverTheta_[i];
  const NumericalScalar scaledTauNorm = scaledTau.norm();
  const NumericalScalar norm2 = scaledTauNorm * scaledTauNorm;
  // For zero norm
  if (norm2 == 0.0)
  {
    // Infinite gradient for nu < 1/2
    if (nu_ < 0.5) return Matrix(spatialDimension_, 1, NumericalPoint(spatialDimension_, -SpecFunc::MaxNumericalScalar));
    // Non-zero gradient for nu = 1/2
    if (nu_ == 0.5)
    {
      Matrix gradient(spatialDimension_, 1);
      for (UnsignedInteger i = 0; i < spatialDimension_; ++i) gradient(i,0) = -amplitude_[0] / scale_[i];
      return gradient;
    }
    // Zero gradient for p > 1
    return Matrix(spatialDimension_, 1);
  }
  // General case
  const NumericalScalar value(std::exp(logNormalizationFactor_ + nu_ * std::log(scaledTauNorm)) * (nu_ * SpecFunc::BesselK(nu_, scaledTauNorm) + SpecFunc::BesselKDerivative(nu_, scaledTauNorm) * scaledTauNorm) / norm2);
  NumericalPoint tauDotsquareSqrt2nuOverTheta(spatialDimension_);
  for(UnsignedInteger i = 0; i < spatialDimension_; ++i) tauDotsquareSqrt2nuOverTheta[i] = tau[i] * sqrt2nuOverTheta_[i] * sqrt2nuOverTheta_[i];
  return Matrix(spatialDimension_, 1, tauDotsquareSqrt2nuOverTheta * value) * amplitude_[0];
}

void MaternModel::setScale(const NumericalPoint & scale)
{
  // First set scale
  StationaryCovarianceModel::setScale(scale);
  // Update scaling factor
  for(UnsignedInteger i = 0; i < spatialDimension_; ++i) sqrt2nuOverTheta_[i] = sqrt(2.0 * nu_) / scale_[i];
}

/* String converter */
String MaternModel::__repr__() const
{
  OSS oss;
  oss << "class=" << MaternModel::GetClassName()
      << " input dimension=" << spatialDimension_
      << " theta=" << scale_
      << " sigma=" << amplitude_
      << " nu=" << nu_;
  return oss;
}

/* String converter */
String MaternModel::__str__(const String & offset) const
{
  OSS oss;
  oss << MaternModel::GetClassName()
      << "(input dimension=" << spatialDimension_
      << ", theta=" << scale_.__str__()
      << ", sigma=" << amplitude_.__str__()
      << ", nu=" << nu_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MaternModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("nu_", nu_);
  adv.saveAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.saveAttribute("sqrt2nuOverTheta_", sqrt2nuOverTheta_);
}

/* Method load() reloads the object from the StorageManager */
void MaternModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("nu_", nu_);
  adv.loadAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.loadAttribute("sqrt2nuOverTheta_", sqrt2nuOverTheta_);
}

END_NAMESPACE_OPENTURNS
