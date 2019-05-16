//                                               -*- C++ -*-
/**
 *  @brief The Squared Normal distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include <cstdlib>
#include <cmath>

#include "openturns/SquaredNormal.hxx"
//#include "openturns/Distribution.hxx"
//#include "openturns/ChiSquare.hxx"
#include "openturns/SpecFunc.hxx"
//#include "openturns/Log.hxx"
//#include "openturns/OSS.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
//#include "openturns/Matrix.hxx"
//#include "openturns/MatrixImplementation.hxx"
//#include "openturns/IdentityMatrix.hxx"
//#include "openturns/NormalCopula.hxx"
//#include "openturns/ResourceMap.hxx"
//#include "openturns/RandomGenerator.hxx"
//#include "openturns/GaussKronrodRule.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquaredNormal)

static const Factory<SquaredNormal> Factory_SquaredNormal;

/* Default Constructor */
SquaredNormal::SquaredNormal()
{
  *this=SquaredNormal(0.0, 1.0);
}

/* Constructor */
SquaredNormal::SquaredNormal(Scalar mu, Scalar sigma)
  : ContinuousDistribution(), mu_(mu), sigma_(sigma), normal_(mu, sigma)
{
  setName("SquaredNormal");
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool SquaredNormal::operator ==(const SquaredNormal & other) const
{
  if (this == &other) return true;
  return normal_ == other.normal_;
}

/* String converter */
String SquaredNormal::__repr__() const
{
  OSS oss;
  oss << "class=" << SquaredNormal::GetClassName()
      << " squarenormal=" << this->normal_;
  return oss;
}

String SquaredNormal::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << " squared of " << this->normal_; 
  return oss;
}

/* Virtual constructor */
SquaredNormal * SquaredNormal::clone() const
{
  return new SquaredNormal(*this);
}

Point SquaredNormal::getRealization() const
{
  return Point(std::pow(mu_ + sigma_ * DistFunc::rNormal(), 2.0));
}

Sample SquaredNormal::getSample(const UnsignedInteger size) const
{
  const Point xi(DistFunc::rNormal(size));
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    result(i, 0) = std::pow((mu_ + sigma_ * xi[i]), 2); 
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

/* Get the CDF of the distribution */
Scalar SquaredNormal::computeCDF(const Point & point) const
{
  Scalar x = point[0];
  if (x <= 0.0) return 0.0;
  return 0.5 * (SpecFunc::Erf((std::sqrt(x) + mu_) / (M_SQRT2 * sigma_)) + SpecFunc::Erf((std::sqrt(x) - mu_) / (M_SQRT2 * sigma_)));
} // computeCDF

/* Get the PDF of the distribution */
Scalar SquaredNormal::computePDF(const Point & point) const
{
  const Scalar x = point[0];
  if (x <= 0.0) return 0.0;
  Scalar sqrtX = std::sqrt(x);
  return (std::exp(-0.5 * std::pow((sqrtX + mu_), 2) / std::pow(sigma_, 2.0)) + std::exp(-0.5 * std::pow((sqrtX - mu_), 2.0) / std::pow(sigma_, 2))) / (2.0 * M_SQRT2 * sigma_ * std::sqrt(x * M_PI));
} // computePDF

void SquaredNormal::computeMean() const
{
  mean_ = Point(1, std::pow(mu_, 2.0) + std::pow(sigma_, 2.0));
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void SquaredNormal::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 2.0 * (2.0 * std::pow(mu_, 2.0) + std::pow(sigma_, 2.0)) * std::pow(sigma_, 2.0);
  isAlreadyComputedCovariance_ = true;
}

void SquaredNormal::computeRange()
{
  setRange(Interval(Point(1, 0.0), Point(1, std::pow(mu_ + 8.5 * sigma_, 2.0)),
		    Interval::BoolCollection(1, true), Interval::BoolCollection(1, false)));
}


END_NAMESPACE_OPENTURNS
