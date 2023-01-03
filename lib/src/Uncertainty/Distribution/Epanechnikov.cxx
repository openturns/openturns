//                                               -*- C++ -*-
/**
 *  @brief The Epanechnikov distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include "openturns/Epanechnikov.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Beta.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Epanechnikov)

static const Factory<Epanechnikov> Factory_Epanechnikov;


/* Default constructor */
Epanechnikov::Epanechnikov()
  : ContinuousDistribution()
{
  setName("Epanechnikov");
  setDimension(1);
  setRange(Interval(-1.0, 1.0));
}

/* Comparison operator */
Bool Epanechnikov::operator ==(const Epanechnikov & ) const
{
  return true;
}

Bool Epanechnikov::equals(const DistributionImplementation & other) const
{
  const Epanechnikov* p_other = dynamic_cast<const Epanechnikov*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Epanechnikov::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Epanechnikov::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  return oss;
}

String Epanechnikov::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "()";
  return oss;
}

/* Virtual constructor */
Epanechnikov * Epanechnikov::clone() const
{
  return new Epanechnikov(*this);
}

/* Get the DDF of the distribution */
Point Epanechnikov::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if ((x <= -1.0) || (x > 1.0)) return Point(1, 0.0);
  return Point(1, -1.5 * x);
}


/* Get the PDF of the distribution */
Scalar Epanechnikov::computePDF(const Scalar x) const
{
  if ((x <= -1.0) || (x > 1.0)) return 0.0;
  return 0.75 * (1.0 + x) * (1.0 - x);
}

Scalar Epanechnikov::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computePDF(point[0]);
}


/* Get the CDF of the distribution */
Scalar Epanechnikov::computeCDF(const Scalar x) const
{
  if (x <= -1.0) return 0.0;
  if (x >= 1.0) return 1.0;
  return 0.5 + x * (0.75 - 0.25 * x * x);
}

Scalar Epanechnikov::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeCDF(point[0]);
}

Scalar Epanechnikov::computeComplementaryCDF(const Scalar x) const
{
  if (x <= -1.0) return 1.0;
  if (x > 1.0) return 0.0;
  return 0.5 - x * (0.75 - 0.25 * x * x);
}

Scalar Epanechnikov::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeComplementaryCDF(point[0]);
}

/** Get the PDFGradient of the distribution */
Point Epanechnikov::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return Point(0);
}

/** Get the CDFGradient of the distribution */
Point Epanechnikov::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return Point(0);
}

/* Get the quantile of the distribution */
Scalar Epanechnikov::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  // 2.094395102393195492308429 = 2 * Pi / 3
  if (tail) return 2.0 * std::cos(0.3333333333333333333333333 * std::acos(2.0 * prob - 1.0) - 2.094395102393195492308429);
  return 2.0 * std::cos(0.3333333333333333333333333 * std::acos(1.0 - 2.0 * prob) - 2.094395102393195492308429);
}

/* Compute the entropy of the distribution */
Scalar Epanechnikov::computeEntropy() const
{
  return 5.0 / 3.0 - std::log(3.0);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Epanechnikov::getRoughness() const
{
  return 0.6;
}

/* Compute the mean of the distribution */
void Epanechnikov::computeMean() const
{
  mean_ = Point(1, 0.0);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Epanechnikov::getStandardDeviation() const
{
  // 0.4472135954999579392818348 = 1 / sqrt(5)
  return Point(1, 0.4472135954999579392818348);
}

/* Get the skewness of the distribution */
Point Epanechnikov::getSkewness() const
{
  return Point(1, 0.0);
}

/* Get the kurtosis of the distribution */
Point Epanechnikov::getKurtosis() const
{
  // 2.142857142857142857142857 = 15 / 7
  return Point(1, 2.142857142857142857142857);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Epanechnikov::getStandardRepresentative() const
{
  return new Beta(2.0, 2.0, -1.0, 1.0);
}

/* Compute the covariance of the distribution */
void Epanechnikov::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 0.2;
  isAlreadyComputedCovariance_ = 0;
}

/* Check if the distribution is elliptical */
Bool Epanechnikov::isElliptical() const
{
  return true;
}

/* Parameters value and description accessor */
Point Epanechnikov::getParameter() const
{
  return Point();
}

Description Epanechnikov::getParameterDescription() const
{
  return Description();
}

/* Method save() stores the object through the StorageManager */
void Epanechnikov::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void Epanechnikov::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
}

END_NAMESPACE_OPENTURNS
