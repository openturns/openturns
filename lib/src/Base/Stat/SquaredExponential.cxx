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

static Factory<SquaredExponential> RegisteredFactory("SquaredExponential");


/* Default constructor */
SquaredExponential::SquaredExponential(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(1, ResourceMap::GetAsNumericalScalar("SquaredExponential-DefaultTheta")))
{
  // Nothing to do
}

/* Parameters constructor */
SquaredExponential::SquaredExponential(const UnsignedInteger spatialDimension,
                                       const NumericalScalar theta)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, 1.0), NumericalPoint(1, theta))
{
  // Nothing to do
}

/* Virtual constructor */
SquaredExponential * SquaredExponential::clone() const
{
  return new SquaredExponential(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix SquaredExponential::operator() (const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  CovarianceMatrix covariance(1);
  const NumericalScalar tau2(tau.normSquare());
  covariance(0, 0) = (tau2 == 0.0 ? 1.0 + nuggetFactor_ : exp(-0.5 * tau2 / (scale_[0] * scale_[0])));
  return covariance;
}

/* Gradient */
Matrix SquaredExponential::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  const NumericalScalar norm2(tau.normSquare());
  // For zero norm
  if (norm2 == 0.0) return Matrix(spatialDimension_, 1);
  // General case
  const NumericalScalar scale2(scale_[0] * scale_[0]);
  const NumericalScalar value(-std::exp(-0.5 * norm2 / scale2) / scale2);
  return Matrix(spatialDimension_, 1, tau * value);
}

/* Parameters accessor */
void SquaredExponential::setParameters(const NumericalPoint & parameters)
{
  if (parameters.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: parameters dimension should be 1 (got " << parameters.getDimension() << ")";
  setScale(parameters);
}

NumericalPointWithDescription SquaredExponential::getParameters() const
{
  NumericalPointWithDescription result(1, scale_[0]);
  result.setDescription(Description(1, "theta"));
  return result;
}

/* String converter */
String SquaredExponential::__repr__() const
{
  OSS oss;
  oss << "class=" << SquaredExponential::GetClassName()
      << " input dimension=" << spatialDimension_
      << " theta=" << scale_[0];
  return oss;
}

/* String converter */
String SquaredExponential::__str__(const String & offset) const
{
  return __repr__();
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
