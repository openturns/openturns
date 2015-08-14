//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "MaternModel.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaternModel);

static Factory<MaternModel> RegisteredFactory("MaternModel");


/* Default constructor */
MaternModel::MaternModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(1, ResourceMap::GetAsNumericalScalar("MaternModel-DefaultTheta")))
  , nu_(ResourceMap::GetAsNumericalScalar("MaternModel-DefaultNu"))
{
  // Compute the normalization factor
  logNormalizationFactor_ = (1.0 - nu_) * std::log(2.0) - SpecFunc::LogGamma(nu_);
  // Compute usefull constant
  sqrt2nuOverTheta_ = sqrt(2.0 * nu_) / scale_[0];
}

/* Parameters constructor */
MaternModel::MaternModel(const UnsignedInteger spatialDimension,
                         const NumericalScalar theta,
                         const NumericalScalar nu)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(1, theta))
  , nu_(nu)
{
  if (nu <= 0.0) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  // Compute the normalization factor
  logNormalizationFactor_ = (1.0 - nu_) * std::log(2.0) - SpecFunc::LogGamma(nu_);
  // Compute usefull constant
  sqrt2nuOverTheta_ = sqrt(2.0 * nu_) / scale_[0];
}

/* Virtual constructor */
MaternModel * MaternModel::clone() const
{
  return new MaternModel(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix MaternModel::operator() (const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  CovarianceMatrix covariance(1);
  const NumericalScalar scaledPoint(sqrt2nuOverTheta_ * tau.norm());
  if (std::abs(scaledPoint) <= SpecFunc::NumericalScalarEpsilon)
    covariance(0, 0) = 1.0 + nuggetFactor_;
  else covariance(0, 0) = exp(logNormalizationFactor_ + nu_ * std::log(scaledPoint) + SpecFunc::LogBesselK(nu_, scaledPoint));
  return covariance;
}

/* Gradient */
Matrix MaternModel::partialGradient(const NumericalPoint & s,
                                    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  const NumericalScalar norm2(tau.normSquare());
  // For zero norm
  if (norm2 == 0.0)
  {
    // Infinite gradient for nu < 1/2
    if (nu_ < 0.5) return Matrix(spatialDimension_, 1, NumericalPoint(spatialDimension_, -SpecFunc::MaxNumericalScalar));
    // Non-zero gradient for nu = 1/2
    if (nu_ == 0.5) return Matrix(spatialDimension_, 1, NumericalPoint(spatialDimension_, -1.0 / scale_[0]));
    // Zero gradient for p > 1
    return Matrix(spatialDimension_, 1);
  }
  // General case
  const NumericalScalar scaledPoint(sqrt2nuOverTheta_ * sqrt(norm2));
  const NumericalScalar value(std::exp(logNormalizationFactor_ + nu_ * std::log(scaledPoint)) * (nu_ * SpecFunc::BesselK(nu_, scaledPoint) + SpecFunc::BesselKDerivative(nu_, scaledPoint) * scaledPoint) / norm2);
  return Matrix(spatialDimension_, 1, tau * value);
}

/* Parameters accessor */
void MaternModel::setParameters(const NumericalPoint & parameters)
{
  if (parameters.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: parameters dimension should be 1 (got " << parameters.getDimension() << ")";
  setScale(parameters);
}

NumericalPointWithDescription MaternModel::getParameters() const
{
  NumericalPointWithDescription result(1, scale_[0]);
  result.setDescription(Description(1, "theta"));
  return result;
}

/* String converter */
String MaternModel::__repr__() const
{
  OSS oss;
  oss << "class=" << MaternModel::GetClassName()
      << " input dimension=" << spatialDimension_
      << " theta=" << scale_[0]
      << " nu=" << nu_;
  return oss;
}

/* String converter */
String MaternModel::__str__(const String & offset) const
{
  return __repr__();
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
