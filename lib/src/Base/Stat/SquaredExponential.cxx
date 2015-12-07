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
#include "SquaredExponential.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquaredExponential);

static const Factory<SquaredExponential> RegisteredFactory;


/* Default constructor */
SquaredExponential::SquaredExponential(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(spatialDimension, ResourceMap::GetAsNumericalScalar("SquaredExponential-DefaultTheta")))
{
  // Nothing to do
}

/* Parameters constructor */
SquaredExponential::SquaredExponential(const UnsignedInteger spatialDimension,
                                       const NumericalScalar theta)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(spatialDimension, theta))
{
  // Nothing to do
}

/** Parameters constructor */
SquaredExponential::SquaredExponential(const NumericalPoint & theta)
  : StationaryCovarianceModel( NumericalPoint(1, 1.0), theta)
{
  // Nothing to do
}

/** Parameters constructor */
SquaredExponential::SquaredExponential(const NumericalPoint & theta,
                                       const NumericalPoint & sigma)
  : StationaryCovarianceModel(sigma, theta)
{
  if (getDimension() != 1)
    throw InvalidArgumentException(HERE) << "In SquaredExponential::SquaredExponential, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getDimension() <<")";
}

/* Virtual constructor */
SquaredExponential * SquaredExponential::clone() const
{
  return new SquaredExponential(*this);
}

/* Computation of the covariance density function */
NumericalScalar SquaredExponential::computeStandardRepresentative(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar tauOverTheta2 = tauOverTheta.normSquare();
  return tauOverTheta2 == 0.0 ? 1.0 + nuggetFactor_ : exp(-0.5 * tauOverTheta2);
}

/* Gradient */
Matrix SquaredExponential::partialGradient(const NumericalPoint & s,
                                           const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau = s - t;
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar norm2 = tauOverTheta.normSquare();
  // For zero norm
  if (norm2 == 0.0) return Matrix(spatialDimension_, 1);
  // General case
  const NumericalScalar value(-std::exp(-0.5 * norm2));
  // Compute tau/theta^2
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] /= scale_[i];
  return Matrix(spatialDimension_, 1, tauOverTheta * value) * amplitude_[0];
}

/* String converter */
String SquaredExponential::__repr__() const
{
  OSS oss;
  oss << "class=" << SquaredExponential::GetClassName()
      << " input dimension=" << spatialDimension_
      << " theta=" << scale_
      << " sigma=" << amplitude_;
  return oss;
}

/* String converter */
String SquaredExponential::__str__(const String & offset) const
{
  OSS oss;
  oss << SquaredExponential::GetClassName()
      << "(input dimension=" << spatialDimension_
      << ", theta=" << scale_.__str__()
      << ", sigma=" << amplitude_.__str__()
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
