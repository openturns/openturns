//                                               -*- C++ -*-
/**
 *  @brief The Squared Normal distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/SquaredNormal.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquaredNormal)

static const Factory<SquaredNormal> Factory_SquaredNormal;

/* Default Constructor */
SquaredNormal::SquaredNormal()
{
  *this = SquaredNormal(0.0, 1.0);
}

/* Constructor */
SquaredNormal::SquaredNormal(Scalar mu, Scalar sigma)
  : ContinuousDistribution(), mu_(mu), sigma_(sigma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "The standard deviation must be > 0 sigma=" << sigma;
  setName("SquaredNormal");
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool SquaredNormal::operator ==(const SquaredNormal & other) const
{
  if (this == &other) return true;
  return mu_ == other.mu_ && sigma_ == other.sigma_;
}

Bool SquaredNormal::equals(const DistributionImplementation & other) const
{
  const SquaredNormal* p_other = dynamic_cast<const SquaredNormal*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String SquaredNormal::__repr__() const
{
  OSS oss;
  oss << "class=" << SquaredNormal::GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_;
  return oss;
}

String SquaredNormal::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName();
  oss << "(mu = " << getMu() << ", sigma = " << getSigma() << ")";
  return oss;
}

/* Virtual constructor */
SquaredNormal * SquaredNormal::clone() const
{
  return new SquaredNormal(*this);
}

Point SquaredNormal::getRealization() const
{
  return Point(1, std::pow(mu_ + sigma_ * DistFunc::rNormal(), 2.0));
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
  return (std::exp(-0.5 * std::pow((sqrtX + mu_), 2.0) / std::pow(sigma_, 2.0)) + std::exp(-0.5 * std::pow((sqrtX - mu_), 2.0) / std::pow(sigma_, 2.0))) / (2.0 * M_SQRT2 * sigma_ * std::sqrt(x * M_PI));
} // computePDF

/* Get the characteristic function of the distribution */
Complex SquaredNormal::computeCharacteristicFunction(const Scalar x) const
{
  if (x == 0.0) return 1.0;
  const Complex den(1.0, -2.0 * x * std::pow(sigma_, 2.0));
  return std::exp(Complex(0.0, x * std::pow(mu_, 2.0)) / den) / std::sqrt(den);
}

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

/* Parameters value accessor */
Point SquaredNormal::getParameter() const
{
  Point point(2);
  point[0] = mu_;
  point[1] = sigma_;
  return point;
}

void SquaredNormal::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = SquaredNormal(parameter[0], parameter[1]);
  setWeight(w);
}

/** Parameters description accessor */
Description SquaredNormal::getParameterDescription() const
{
  Description description(2);
  description[0] = "mu";
  description[1] = "sigma";
  return description;
}

/* Mu accessor */
void SquaredNormal::setMu(const Scalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar SquaredNormal::getMu() const
{
  return mu_;
}

/* Sigma accessor */
void SquaredNormal::setSigma(const Scalar sigma)
{
  if (0 >= sigma) throw InvalidArgumentException(HERE) << "in SquaredNormal : sigma must be stricly positive";
  if (sigma != sigma_)
  {
    sigma_ = sigma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar SquaredNormal::getSigma() const
{
  return sigma_;
}

/* Method save() stores the object through the StorageManager */
void SquaredNormal::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
}

/* Method load() reloads the object from the StorageManager */
void SquaredNormal::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  computeRange();
}
END_NAMESPACE_OPENTURNS
