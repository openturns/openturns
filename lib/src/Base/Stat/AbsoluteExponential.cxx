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
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(spatialDimension, ResourceMap::GetAsNumericalScalar("AbsoluteExponential-DefaultTheta")))
{
  // Nothing to do
}

/* Parameters constructor */
AbsoluteExponential::AbsoluteExponential(const UnsignedInteger spatialDimension,
                                         const NumericalScalar theta)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(spatialDimension, theta))
{
  // Nothing to do
}

/* Virtual constructor */
AbsoluteExponential * AbsoluteExponential::clone() const
{
  return new AbsoluteExponential(*this);
}

/* Computation of the covariance density function */
NumericalScalar AbsoluteExponential::computeStandardRepresentative(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar tauOverThetaNorm = tauOverTheta.norm1();
  return tauOverThetaNorm == 0.0 ? 1.0 + nuggetFactor_ : exp(-tauOverThetaNorm);
}

/* Gradient */
Matrix AbsoluteExponential::partialGradient(const NumericalPoint & s,
                                            const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar norm1 = tauOverTheta.norm1();
  // For zero norm
  // Norm1 is null if all elements are zero
  // In that case gradient is not defined
  if (norm1 == 0.0)
  {
    Matrix gradient(spatialDimension_, 1);
    for (UnsignedInteger i = 0; i < spatialDimension_; ++i) gradient(i,0) = -amplitude_[0] / scale_[i];
    return gradient;
  }
  // General case
  const NumericalScalar value = std::exp(-norm1);
  // Gradient take as factor sign(tau_i) /theta_i
  NumericalPoint factor(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i)
  {
    factor[i] = amplitude_[0] / scale_[i];
    if (tau[i] < 0) factor[i] *= -1.0;
  }
  return Matrix(spatialDimension_, 1, factor * value) ;
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
      << " theta=" << scale_
      << " sigma=" << amplitude_;
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
