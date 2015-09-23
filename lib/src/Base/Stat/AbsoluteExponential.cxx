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
#include "AbsoluteExponential.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AbsoluteExponential);

static const Factory<AbsoluteExponential> RegisteredFactory;


/* Constructor based on spatial dimension */
AbsoluteExponential::AbsoluteExponential(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(1, ResourceMap::GetAsNumericalScalar("AbsoluteExponential-DefaultTheta")))
{
  // Nothing to do
}

/* Parameters constructor */
AbsoluteExponential::AbsoluteExponential(const UnsignedInteger spatialDimension,
    const NumericalScalar theta)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(1, theta))
{
  // Nothing to do
}

/* Virtual constructor */
AbsoluteExponential * AbsoluteExponential::clone() const
{
  return new AbsoluteExponential(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix AbsoluteExponential::operator() (const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  CovarianceMatrix covariance(1);
  const NumericalScalar tauNorm(tau.norm());
  covariance(0, 0) = (tauNorm == 0.0 ? 1.0 + nuggetFactor_ : exp(-tauNorm / scale_[0]));
  return covariance;
}

/* Gradient */
Matrix AbsoluteExponential::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  const NumericalScalar norm2(tau.normSquare());
  // For zero norm
  if (norm2 == 0.0) return Matrix(spatialDimension_, 1, NumericalPoint(spatialDimension_, -1.0 / scale_[0]));
  // General case
  const NumericalScalar exponent(-sqrt(norm2) / scale_[0]);
  const NumericalScalar value(exponent * std::exp(exponent) / norm2);
  return Matrix(spatialDimension_, 1, tau * value);
}

/* Parameters accessor */
void AbsoluteExponential::setParameters(const NumericalPoint & parameters)
{
  if (parameters.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: parameters dimension should be 1 (got " << parameters.getDimension() << ")";
  setScale(parameters);
}

NumericalPointWithDescription AbsoluteExponential::getParameters() const
{
  NumericalPointWithDescription result(1, scale_[0]);
  result.setDescription(Description(1, "theta"));
  return result;
}

/* String converter */
String AbsoluteExponential::__repr__() const
{
  OSS oss;
  oss << "class=" << AbsoluteExponential::GetClassName()
      << " input dimension=" << spatialDimension_
      << " theta=" << scale_[0];
  return oss;
}

/* String converter */
String AbsoluteExponential::__str__(const String & offset) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void AbsoluteExponential::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void AbsoluteExponential::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
}

END_NAMESPACE_OPENTURNS
