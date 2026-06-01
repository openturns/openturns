//                                               -*- C++ -*-
/**
 *  @brief The VonMisesFisher distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/VonMisesFisher.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VonMisesFisher)

static const Factory<VonMisesFisher> Factory_VonMisesFisher;


/* Default constructor */
VonMisesFisher::VonMisesFisher()
  : DistributionImplementation()
{
  setName("VonMisesFisher");
  setMuKappa({1.0, 0.0}, 1.0);
  computeRange();
}

/* Parameters constructor */
VonMisesFisher::VonMisesFisher(const Point & mu,
                 const Scalar kappa)
  : DistributionImplementation()
{
  setName("VonMisesFisher");
  setMuKappa(mu, kappa);
  computeRange();
}

/* Comparison operator */
Bool VonMisesFisher::operator ==(const VonMisesFisher & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (kappa_ == other.kappa_);
}

Bool VonMisesFisher::equals(const DistributionImplementation & other) const
{
  const VonMisesFisher* p_other = dynamic_cast<const VonMisesFisher*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String VonMisesFisher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << VonMisesFisher::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mu=" << mu_
      << " kappa=" << kappa_;
  return oss;
}

String VonMisesFisher::__str__(const String &) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", kappa = " << kappa_ << ")";
  return oss;
}

/* Virtual constructor */
VonMisesFisher * VonMisesFisher::clone() const
{
  return new VonMisesFisher(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void VonMisesFisher::computeRange()
{
  setRange(Interval(Point(dimension_, -1.0), Point(dimension_, 1.0)));
}

/* Generate size realizations of the cosine part, as a Point */
Point VonMisesFisher::getCosSample(const UnsignedInteger size) const
{
  Point cosRNG(size);
  const UnsignedInteger dM1 = dimension_ - 1;
  const Scalar alpha = 0.5 * dM1;
  UnsignedInteger found = 0;
  while (found < size)
    {
      // Heuristic size to generate based on the acceptance ratio
      UnsignedInteger m = std::max(size, static_cast<UnsignedInteger>(1.5 * (size - found)));
      for (UnsignedInteger i = 0; i < m; ++i)
        {
          const Scalar z = DistFunc::rBeta(alpha, alpha);
          const Scalar t = (1.0 - (1.0 + b_) * z) / (1.0 - (1.0 - b_) * z);
          const Scalar test = kappa_ * t + dM1 * std::log1p(-t * x0_) - c_;
          if (test > std::log(RandomGenerator::Generate()))
            {
              cosRNG[found] = t;
              ++found;
              if (found == size) break;
            } // if test
        } // for i
    } // while found
  return cosRNG;
}

/* Get one realization of the distribution */
Point VonMisesFisher::getRealization() const
{
  return getSample(1)[0];
}

/* Get a sample of the distribution */
Sample VonMisesFisher::getSample(const UnsignedInteger size) const
{
  Sample result(size, dimension_);
  // Generate samples orthogonal to mu
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar zNorm = 0.0;
      // Standard normal samples
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        {
          const Scalar rNorm = DistFunc::rNormal();
          result(i, j) = rNorm;
          zNorm += rNorm * rNorm;
        }
      zNorm = std::sqrt(zNorm);

      // Projection on the mu orthogonal space
      Scalar dotProduct = 0.0;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        {
          result(i, j) /= zNorm;
          dotProduct += result(i, j) * mu_[j];
        }

      // Skip the last step if uniform
      if (kappa_ == 0.0) continue;

      Scalar newZNorm = 0.0;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        {
          const Scalar newZ = result(i, j) - dotProduct * mu_[j];
          result(i, j) = newZ;
          newZNorm += newZ * newZ;
        }
      newZNorm = std::sqrt(newZNorm);
      
      // Final normalization
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        result(i, j) /= newZNorm;
    } // Generate samples orthogonal to mu
  if (kappa_ > 0.0)
    {
      const Point cosRNG(getCosSample(size));
      // Combine angles with direction
      for (UnsignedInteger i = 0; i < size; ++i)
        {
          const Scalar cosI = cosRNG[i];
          const Scalar sinI = std::sqrt(1.0 - cosI * cosI);
          for (UnsignedInteger j = 0; j < dimension_; ++j)
            result(i, j) = result(i, j) * sinI + mu_[j] * cosI;
        } // Combine angles with direction
    }
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

Scalar VonMisesFisher::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const Scalar logPDF = computeLogPDF(point);
  return std::exp(logPDF);
}

Scalar VonMisesFisher::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const Scalar normPoint = point.norm();
  if (std::abs(normPoint - 1.0) > pdfEpsilon_) return SpecFunc::LowestScalar;
  if (kappa_ == 0.0)
    return logNormalizationFactor_;
  // kappa_ > 0.0
  return logNormalizationFactor_ + kappa_ * mu_.dot(point);
}

/* Compute the CDF of the distribution */
Scalar VonMisesFisher::computeCDF(const Point &) const
{
  throw NotYetImplementedException(HERE) << "In VonMisesFisher::computeCDF()";
}

/* Compute the mean of the distribution */
void VonMisesFisher::computeMean() const
{
  // Special case for the uniform distribution over the unit sphere
  if (kappa_ == 0.0)
    mean_ = Point(dimension_, 0.0);
  else
    mean_ = mu_ * std::exp(SpecFunc::LogBesselInu(kappa_, 0.5 * dimension_) - SpecFunc::LogBesselInu(kappa_, 0.5 * dimension_ - 1.0));
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void VonMisesFisher::computeCovariance() const
{
  throw NotYetImplementedException(HERE) << "In VonMisesFisher::computeCovariance()";
}

/* Compute the entropy of the distribution */
Scalar VonMisesFisher::computeEntropy() const
{
  return -logNormalizationFactor_ - kappa_ * std::exp(SpecFunc::LogBesselInu(kappa_, 0.5 * dimension_) - SpecFunc::LogBesselInu(kappa_, 0.5 * dimension_ - 1.0));
}

/* Get the standard deviation of the distribution */
Point VonMisesFisher::getStandardDeviation() const
{
  throw NotYetImplementedException(HERE) << "In VonMisesFisher::getStandardDeviation()";
}

/* Get the skewness of the distribution */
Point VonMisesFisher::getSkewness() const
{
  throw NotYetImplementedException(HERE) << "In VonMisesFisher::getSkewness()";
}

/* Get the kurtosis of the distribution */
Point VonMisesFisher::getKurtosis() const
{
  throw NotYetImplementedException(HERE) << "In VonMisesFisher::getKurtosis()";
}

Point VonMisesFisher::getParameter() const
{
  Point point(mu_);
  point.add(kappa_);
  return point;
}

void VonMisesFisher::setParameter(const Point & parameter)
{
  const UnsignedInteger size = parameter.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: the given parameter cannot be converted into a location vector and a shape parameter of a VonMisesFisher distribution of dimension greater or equal to 2.";
  setDimension(size - 1);
  Point mu(dimension_);
  std::copy(parameter.begin(), parameter.begin() + dimension_, mu.begin());
  const Scalar kappa = parameter[dimension_];
  const Scalar w = getWeight();
  *this = VonMisesFisher(mu, kappa);
  setWeight(w);
}

Description VonMisesFisher::getParameterDescription() const
{
  Description description(Description::BuildDefault(dimension_, "mu"));
  description.add("kappa");
  return description;
}

/* Mu accessor */
void VonMisesFisher::setMu(const Point & mu)
{
  if (mu != mu_)
    {
      if (mu.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the dimension of mu=" << mu.getDimension() << " must be equal to the distribution dimension=" << dimension_;
      const Scalar normMu = mu.norm();
      if (normMu == 0.0) throw InvalidArgumentException(HERE) << "Error: mu is null, it should be a unit vector";
      if (std::abs(normMu - 1.0) > SpecFunc::Precision) LOGWARN(OSS() << "Mu has a norm=" << normMu << " not equal to 1, it will be normalized");
      mu_ = mu / normMu;
      isAlreadyComputedMean_ = false;
      isAlreadyComputedCovariance_ = false;
      update();
    }
}

Point VonMisesFisher::getMu() const
{
  return mu_;
}


/* Kappa accessor */
void VonMisesFisher::setKappa(const Scalar kappa)
{
  if (kappa != kappa_)
  {
    if (!(kappa >= 0.0)) throw InvalidArgumentException(HERE) << "Error: kappa must be nonnegative";
    kappa_ = kappa;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    update();
  }
}

Scalar VonMisesFisher::getKappa() const
{
  return kappa_;
}

void VonMisesFisher::setMuKappa(const Point & mu, const Scalar kappa)
{
  const UnsignedInteger dimension = mu.getDimension();
  if (dimension < 2) throw InvalidArgumentException(HERE) << "Error: mu has a dimension=" << dimension << ". It should be greater or equal to 2.";
  setDimension(dimension);
  if (!(kappa >= 0.0)) throw InvalidArgumentException(HERE) << "Error: kappa must be nonnegative";
  kappa_ = kappa;
  const Scalar normMu = mu.norm();
  if (normMu == 0.0) throw InvalidArgumentException(HERE) << "Error: mu is null, it should be a unit vector.";
  if (std::abs(normMu - 1.0) > SpecFunc::Precision) LOGWARN(OSS() << "Mu has a norm=" << normMu << " not equal to 1, it will be normalized.");
  mu_ = mu / normMu;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  update();
}

/* Compute the normalization factor on log-scale */
void VonMisesFisher::update()
{
  const UnsignedInteger p = getDimension();
  if (kappa_ == 0.0)
    logNormalizationFactor_ = SpecFunc::LogGamma(0.5 * p) - std::log(2.0) - 0.5 * p * std::log(M_PI);
  else
    {
      const Scalar logKappa = std::log(kappa_);
      // logNormalizationFactor_ = \log(\kappa^{p/2-1}/((2\pi)^{p/2}I_{p/2-1}(\kappa)))
      logNormalizationFactor_ = 0.5 * p * (logKappa - std::log(SpecFunc::TWOPI)) - logKappa - SpecFunc::LogBesselInu(kappa_, 0.5 * p - 1.0);
    }
  // Constants for the sampling algorithm
  // See Carlos Pinzón, Kangsoo Jung. Fast Python sampler for the von Mises Fisher distribution. 2023. ⟨hal-04004568v2⟩
  b_ = (p - 1.0) / (2.0 * kappa_ + std::sqrt(4.0 * kappa_ * kappa_ + (p - 1.0) * (p - 1.0)));
  x0_ = (1.0 - b_) / (1.0 + b_);
  c_ = kappa_ * x0_ + (p - 1.0) * std::log1p(-x0_ * x0_);
}

/* Method save() stores the object through the StorageManager */
void VonMisesFisher::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "kappa_", kappa_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  adv.saveAttribute( "b_", b_ );
  adv.saveAttribute( "x0_", x0_ );
  adv.saveAttribute( "c_", c_ );
}

/* Method load() reloads the object from the StorageManager */
void VonMisesFisher::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "kappa_", kappa_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  adv.loadAttribute( "b_", b_ );
  adv.loadAttribute( "x0_", x0_ );
  adv.loadAttribute( "c_", c_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
