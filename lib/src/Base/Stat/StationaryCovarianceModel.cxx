//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StationaryCovarianceModel
 */

CLASSNAMEINIT(StationaryCovarianceModel)

static const Factory<StationaryCovarianceModel> Factory_StationaryCovarianceModel;

/* Constructor with parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
{
  // Nothing to do
}

/** Standard constructor with scale and amplitude parameters parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const Point & scale,
    const Point & amplitude)
  : CovarianceModelImplementation(scale, amplitude)
{
  // Nothing to do
}

/** Standard constructor with scale, amplitude and spatial correlation parameters parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const Point & scale,
    const Point & amplitude,
    const CorrelationMatrix & spatialCorrelation)
  : CovarianceModelImplementation(scale, amplitude, spatialCorrelation)
{
  // Nothing to do
}

/** Standard constructor with scale and spatial covariance parameters parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const Point & scale,
    const CovarianceMatrix & spatialCovariance)
  : CovarianceModelImplementation(scale, spatialCovariance)
{
  // Nothing to do
}

/* Virtual constructor */
StationaryCovarianceModel * StationaryCovarianceModel::clone() const
{
  return new StationaryCovarianceModel(*this);
}

/* Computation of the covariance function */
SquareMatrix StationaryCovarianceModel::operator() (const Point & s,
    const Point & t) const
{
  return (*this)(t - s);
}

SquareMatrix StationaryCovarianceModel::operator() (const Point & tau) const
{
  const Scalar rho = computeStandardRepresentative(tau);
  // outputCovariance_ * rho is a SymmetricMatrix.
  // Its implementation only contains terms in the lower half.
  // outputCovariance_ needs to be symmetrized.
  outputCovariance_.checkSymmetry();
  return outputCovariance_ * rho;
}

Scalar StationaryCovarianceModel::computeAsScalar(const Point & s,
    const Point & t) const
{
  if (outputDimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << outputDimension_ << ", expected dimension=1.";
  return outputCovariance_(0, 0) * computeStandardRepresentative(t - s);
}

Scalar StationaryCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (outputDimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << outputDimension_ << ", expected dimension=1.";
  return outputCovariance_(0, 0) * computeStandardRepresentative(tau);
}

Scalar StationaryCovarianceModel::computeStandardRepresentative(const Point & s,
    const Point & t) const
{
  return computeStandardRepresentative(t - s);
}

Scalar StationaryCovarianceModel::computeStandardRepresentative(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In StationaryCovarianceModel::computeStandardRepresentative (const Point & tau) const";
}

/* Is the underlying a stationary covariance model ? */
Bool StationaryCovarianceModel::isStationary() const
{
  return true;
}

/* String converter */
String StationaryCovarianceModel::__repr__() const
{
  OSS oss(true);
  oss << "class= " << StationaryCovarianceModel::GetClassName();
  return oss;
}

/* String converter */
String StationaryCovarianceModel::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class= " << StationaryCovarianceModel::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void StationaryCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void StationaryCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
