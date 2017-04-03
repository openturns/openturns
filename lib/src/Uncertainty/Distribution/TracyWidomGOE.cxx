//                                               -*- C++ -*-
/**
 *  @brief The TracyWidomGOE distribution
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
#include <cmath>
#include "openturns/TracyWidomGOE.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TracyWidomGOE);

static const Factory<TracyWidomGOE> Factory_TracyWidomGOE;


/* Default constructor */
TracyWidomGOE::TracyWidomGOE()
  : ContinuousDistribution()
{
  setName("TracyWidomGOE");
  throw NotYetImplementedException(HERE) << "In TracyWidomGOE::TracyWidomGOE()";
}

/* Comparison operator */
Bool TracyWidomGOE::operator ==(const TracyWidomGOE & other) const
{
  return true;
}

Bool TracyWidomGOE::equals(const DistributionImplementation & other) const
{
  const TracyWidomGOE* p_other = dynamic_cast<const TracyWidomGOE*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String TracyWidomGOE::__repr__() const
{
  OSS oss;
  oss << "class=" << TracyWidomGOE::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  return oss;
}

/* Virtual constructor */
TracyWidomGOE * TracyWidomGOE::clone() const
{
  return new TracyWidomGOE(*this);
}

/* Get the DDF of the distribution */
Point TracyWidomGOE::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In TracyWidomGOE::computeDDF(const Point & point) const";
}


/* Get the PDF of the distribution */
Scalar TracyWidomGOE::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In TracyWidomGOE::computePDF(const Point & point) const";
}


/* Get the CDF of the distribution */
Scalar TracyWidomGOE::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In TracyWidomGOE::computeCDF(const Point & point) const";
}

/** Get the PDFGradient of the distribution */
Point TracyWidomGOE::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return Point(0);
}

/** Get the CDFGradient of the distribution */
Point TracyWidomGOE::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return Point(0);
}

/* Get the quantile of the distribution */
Scalar TracyWidomGOE::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  throw NotYetImplementedException(HERE) << "In TracyWidomGOE::computeScalarQuantile(const Scalar prob, const Bool tail) const";
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar TracyWidomGOE::getRoughness() const
{
  throw NotYetImplementedException(HERE) << "In TracyWidomGOE::getRoughness() const";
}

/* Compute the mean of the distribution */
void TracyWidomGOE::computeMean() const
{
  // RMT matlab toolbox computation
  mean_ = Point(1, -1.2065335745820);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point TracyWidomGOE::getStandardDeviation() const
{
  // RMT matlab toolbox computation
  return Point(1, 1.607781034581);
}

/* Get the skewness of the distribution */
Point TracyWidomGOE::getSkewness() const
{
  // RMT matlab toolbox computation
  return Point(1, 0.29346452408);
}

/* Get the kurtosis of the distribution */
Point TracyWidomGOE::getKurtosis() const
{
  // RMT matlab toolbox computation
  return Point(1, 0.1652429384);
}

/* Compute the covariance of the distribution */
void TracyWidomGOE::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar sigma = getStandardDeviation()[0];
  covariance_(0, 0) = sigma * sigma;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
TracyWidomGOE::PointWithDescriptionCollection TracyWidomGOE::getParametersCollection() const
{
  // No parameter, return a collection with an empty PointWithDescription
  return PointWithDescriptionCollection(1, PointWithDescription(0));
}

/* Method save() stores the object through the StorageManager */
void TracyWidomGOE::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void TracyWidomGOE::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
}

END_NAMESPACE_OPENTURNS
