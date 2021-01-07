//                                               -*- C++ -*-
/**
 *  @brief The VonMises distribution
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
#include <cmath>
#include "openturns/VonMises.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VonMises)

static const Factory<VonMises> Factory_VonMises;

/* Default constructor */
VonMises::VonMises()
  : ContinuousDistribution()
  , mu_(0.0)
  , kappa_(0.0)
  , normalizationFactor_(0.0)
  , ratioOfUniformsBound_(0.0)
{
  setName("VonMises");
  setDimension(1);
  // This call also call update and computeRange()
  setKappa(1.0);
  computeRange();
}

/* Parameters constructor */
VonMises::VonMises(const Scalar mu,
                   const Scalar kappa)
  : ContinuousDistribution()
  , mu_(0.0)
  , kappa_(0.0)
  , normalizationFactor_(0.0)
  , ratioOfUniformsBound_(0.0)
{
  setName("VonMises");
  setDimension(1);
  // This call also call update() and computeRange()
  setMu(mu);
  setKappa(kappa);
  computeRange();
}

/* Comparison operator */
Bool VonMises::operator ==(const VonMises & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (kappa_ == other.kappa_);
}

Bool VonMises::equals(const DistributionImplementation & other) const
{
  const VonMises* p_other = dynamic_cast<const VonMises*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String VonMises::__repr__() const
{
  OSS oss;
  oss << "class=" << VonMises::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mu=" << mu_
      << " kappa=" << kappa_;
  return oss;
}

String VonMises::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(mu = " << mu_ << ", kappa=" << kappa_ << ")";
  return oss;
}

/* Mu accessor */
void VonMises::setMu(const Scalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

Scalar VonMises::getMu() const
{
  return mu_;
}

/* Kappa accessor */
void VonMises::setKappa(const Scalar kappa)
{
  if (!(kappa > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive kappa, got kappa=" << kappa;
  if (kappa != kappa_)
  {
    kappa_ = kappa;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    update();
  }
}

Scalar VonMises::getKappa() const
{
  return kappa_;
}

/* Circular moments accessor */
Scalar VonMises::getCircularMean() const
{
  return mu_;
}

Scalar VonMises::getCircularVariance() const
{
  return 1.0 - std::exp(SpecFunc::DeltaLogBesselI10(kappa_));
}

/* Virtual constructor */
VonMises * VonMises::clone() const
{
  return new VonMises(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void VonMises::computeRange()
{
  setRange(Interval(mu_ - M_PI, mu_ + M_PI));
}

/** Update the derivative attributes */
void VonMises::update()
{
  normalizationFactor_ = -std::log(2.0 * M_PI) - SpecFunc::LogBesselI0(kappa_);
  ratioOfUniformsBound_ = (kappa_ > 1.3 ? 1.0 / std::sqrt(kappa_) : M_PI * std::exp(-kappa_));
}


/* Get one realization of the distribution
 * We use the method described in:
 * Lucio Barabesi, "Generating Von Mises variates by the ratio-of-uniforms method",
 * Statistica Applicata Vol.7, n4, 1995.
 */
Point VonMises::getRealization() const
{
  for (;;)
  {
    const Scalar r1 = RandomGenerator::Generate();
    const Scalar r2 = RandomGenerator::Generate();
    const Scalar theta = ratioOfUniformsBound_ * (2.0 * r2 - 1.0) / r1;
    // Quick rejection
    if (std::abs(theta) > M_PI) continue;
    // Quick acceptance
    if (kappa_ * theta * theta  < 4.0 - 4.0 * r1) return Point(1, theta + mu_);
    // Slow rejection
    if (kappa_ * std::cos(theta) < 2.0 * std::log(r1) + kappa_) continue;
    return Point(1, theta + mu_);
  }
  // Should never go there
  return Point(1, 0.0);
}

/* Get the DDF of the distribution */
Point VonMises::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (std::abs(x) > M_PI) return Point(1, 0.0);
  return Point(1, -kappa_ * std::sin(x - mu_) * computePDF(point));
}


/* Get the PDF of the distribution */
Scalar VonMises::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (std::abs(x - mu_) > M_PI) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar VonMises::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (std::abs(x - mu_) > M_PI) return SpecFunc::LowestScalar;
  return normalizationFactor_ + kappa_ * std::cos(x - mu_);
}

/* Parameters value accessor */
Point VonMises::getParameter() const
{
  Point point(2);
  point[0] = mu_;
  point[1] = kappa_;
  return point;
}

/* Compute the entropy of the distribution */
Scalar VonMises::computeEntropy() const
{
  const Scalar logI0 = SpecFunc::LogBesselI0(kappa_);
  const Scalar logI1 = SpecFunc::LogBesselI1(kappa_);
  return -kappa_ * std::exp(logI1 - logI0) + 2.0 * SpecFunc::LOGSQRT2PI + logI0;
}

void VonMises::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = VonMises(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description VonMises::getParameterDescription() const
{
  Description description(2);
  description[0] = "mu";
  description[1] = "kappa";
  return description;
}

/* Method save() stores the object through the StorageManager */
void VonMises::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "kappa_", kappa_ );
}

/* Check if the distribution is elliptical */
Bool VonMises::isElliptical() const
{
  return mu_ == 0.0;
}

/* Method load() reloads the object from the StorageManager */
void VonMises::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "kappa_", kappa_ );
  update();
}

END_NAMESPACE_OPENTURNS
