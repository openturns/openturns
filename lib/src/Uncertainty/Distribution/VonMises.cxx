//                                               -*- C++ -*-
/**
 *  @brief The VonMises distribution
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
#include <cmath>
#include "VonMises.hxx"
#include "RandomGenerator.hxx"
#include "SpecFunc.hxx"
#include "DistFunc.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VonMises);

static const Factory<VonMises> RegisteredFactory;

/* Default constructor */
VonMises::VonMises()
  : ContinuousDistribution()
  , mu_(0.0)
  , kappa_(0.0)
  , normalizationFactor_(0.0)
  , ratioOfUniformsBound_(0.0)
{
  setName("VonMises");
  // This call also call update and computeRange()
  setKappa(1.0);
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
VonMises::VonMises(const NumericalScalar mu,
                   const NumericalScalar kappa)
  : ContinuousDistribution()
  , mu_(0.0)
  , kappa_(0.0)
  , normalizationFactor_(0.0)
  , ratioOfUniformsBound_(0.0)
{
  setName("VonMises");
  // This call also call update() and computeRange()
  setMu(mu);
  setKappa(kappa);
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool VonMises::operator ==(const VonMises & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (kappa_ == other.kappa_);
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

String VonMises::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(mu = " << mu_ << ", kappa=" << kappa_ << ")";
  return oss;
}

/* Mu accessor */
void VonMises::setMu(const NumericalScalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

NumericalScalar VonMises::getMu() const
{
  return mu_;
}

/* Kappa accessor */
void VonMises::setKappa(const NumericalScalar kappa)
{
  if (kappa <= 0.0) throw InvalidArgumentException(HERE) << "Error: expected a positive kappa, got kappa=" << kappa;
  if (kappa != kappa_)
  {
    kappa_ = kappa;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    update();
  }
}

NumericalScalar VonMises::getKappa() const
{
  return kappa_;
}

/* Circular moments accessor */
NumericalScalar VonMises::getCircularMean() const
{
  return mu_;
}

NumericalScalar VonMises::getCircularVariance() const
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
  setRange(Interval(-M_PI, M_PI));
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
NumericalPoint VonMises::getRealization() const
{
  for (;;)
  {
    const NumericalScalar r1(RandomGenerator::Generate());
    const NumericalScalar r2(RandomGenerator::Generate());
    const NumericalScalar theta(ratioOfUniformsBound_ * (2.0 * r2 - 1.0) / r1);
    // Quick rejection
    if (std::abs(theta) > M_PI) continue;
    // Quick acceptance
    if (kappa_ * theta * theta  < 4.0 - 4.0 * r1)
    {
      const NumericalScalar y(theta + fmod(mu_ + M_PI, 2.0 * M_PI) - M_PI);
      return NumericalPoint(1, (y > M_PI ? y - M_PI : (y < -M_PI ? y + M_PI : y)));
    }
    // Slow rejection
    if (kappa_ * std::cos(theta) < 2.0 * std::log(r1) + kappa_) continue;
    // Acceptance
    const NumericalScalar y(theta + fmod(mu_ + M_PI, 2.0 * M_PI) - M_PI);
    return NumericalPoint(1, (y > M_PI ? y - M_PI : (y < -M_PI ? y + M_PI : y)));
  }
  return NumericalPoint(1, 0.0);
}

/* Get the DDF of the distribution */
NumericalPoint VonMises::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (std::abs(x) > M_PI) return NumericalPoint(1, 0.0);
  return NumericalPoint(1, -kappa_ * std::sin(x - mu_) * computePDF(point));
}


/* Get the PDF of the distribution */
NumericalScalar VonMises::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (std::abs(x) > M_PI) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar VonMises::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (std::abs(x) > M_PI) return -SpecFunc::MaxNumericalScalar;
  return normalizationFactor_ + kappa_ * std::cos(x - mu_);
}

/* Parameters value and description accessor */
VonMises::NumericalPointWithDescriptionCollection VonMises::getParametersCollection() const
{
  NumericalPointWithDescriptionCollection parameters(1);
  NumericalPointWithDescription point(2);
  Description description(point.getDimension());
  point[0] = mu_;
  description[0] = "mu";
  point[1] = kappa_;
  description[1] = "kappa";
  point.setDescription(description);
  point.setName(getDescription()[0]);
  parameters[0] = point;
  return parameters;
}

void VonMises::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  const NumericalScalar w(getWeight());
  *this = VonMises(parametersCollection[0][0], parametersCollection[0][1]);
  setWeight(w);
}

/* Method save() stores the object through the StorageManager */
void VonMises::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "kappa_", kappa_ );
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
