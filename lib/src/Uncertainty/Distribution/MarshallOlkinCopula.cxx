//                                               -*- C++ -*-
/**
 *  @brief The MarshallOlkinCopula distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MarshallOlkinCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarshallOlkinCopula)

static const Factory<MarshallOlkinCopula> Factory_MarshallOlkinCopula;

/* Default constructor */
MarshallOlkinCopula::MarshallOlkinCopula()
  : DistributionImplementation()
  , alpha_(0.5)
  , beta_(0.5)
{
  isCopula_ = true;
  setName( "MarshallOlkinCopula" );
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
MarshallOlkinCopula::MarshallOlkinCopula(const Scalar alpha,
    const Scalar beta)
  : DistributionImplementation()
  , alpha_(-2.0) // To force the update using accessors
  , beta_(-2.0) // To force the update using accessors
{
  isCopula_ = true;
  setName( "MarshallOlkinCopula" );
  // We set the dimension of the MarshallOlkinCopula distribution
  setDimension( 2 );
  // Check the given alpha
  setAlpha(alpha);
  // Check the given beta
  setBeta(beta);
  computeRange();
}

/* Comparison operator */
Bool MarshallOlkinCopula::operator ==(const MarshallOlkinCopula & other) const
{
  if (this == &other) return true;
  return ((alpha_ == other.alpha_) && (beta_ == other.beta_));
}

Bool MarshallOlkinCopula::equals(const DistributionImplementation & other) const
{
  const MarshallOlkinCopula* p_other = dynamic_cast<const MarshallOlkinCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MarshallOlkinCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MarshallOlkinCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_;
  return oss;
}

String MarshallOlkinCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ")";
  return oss;
}

/* Virtual constructor */
MarshallOlkinCopula * MarshallOlkinCopula::clone() const
{
  return new MarshallOlkinCopula(*this);
}

/* Get the CDF of the distribution */
Scalar MarshallOlkinCopula::computeCDF(const Point & point) const
{
  const Scalar u = point[0];
  const Scalar v = point[1];
  const Scalar ualpha = std::pow(u, alpha_);
  const Scalar vbeta = std::pow(v, beta_);
  Scalar p;
  if (ualpha > vbeta)
  {
    if (alpha_ == 1.) /* Prevent zero exponent in pow */
    {
      p = v;
    }
    else
    {
      p = std::pow(u, 1 - alpha_) * v;
    }
  }
  else
  {
    if (beta_ == 1.) /* Prevent zero exponent in pow */
    {
      p = u;
    }
    else
    {
      p = u * std::pow(v, 1 - beta_);
    }
  }
  return p;
}

/* Parameters value accessor */
Point MarshallOlkinCopula::getParameter() const
{
  Point point(2);
  point[0] = alpha_;
  point[1] = beta_;
  return point;
}

void MarshallOlkinCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 value, got " << parameter.getSize();
  setAlpha(parameter[0]);
  setBeta(parameter[1]);
}

/* Tell if the distribution has independent copula */
Bool MarshallOlkinCopula::hasIndependentCopula() const
{
  return ((alpha_ == 0.0) || (beta_ == 0.0));
}

/* Alpha accessor */
void MarshallOlkinCopula::setAlpha(const Scalar alpha)
{
  if (!(alpha >= 0.0) || !(alpha <= 1.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be in [0, 1], here alpha=" << alpha;
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    isAlreadyComputedCovariance_ = false;
  }
}

/* Alpha accessor */
Scalar MarshallOlkinCopula::getAlpha() const
{
  return alpha_;
}

/* Beta accessor */
void MarshallOlkinCopula::setBeta(const Scalar beta)
{
  if (!(beta >= 0.0) || !(beta <= 1.0)) throw InvalidArgumentException(HERE) << "Beta MUST be in [0, 1], here beta=" << beta;
  beta_ = beta;
  isAlreadyComputedCovariance_ = false;
}

/* Beta accessor */
Scalar MarshallOlkinCopula::getBeta() const
{
  return beta_;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix MarshallOlkinCopula::getKendallTau() const
{
  CorrelationMatrix tauKendall(2);
  if ((alpha_ == 0.0) || (beta_ == 0.0))
  {
    tauKendall(1, 0) = 0.0;
  }
  else if ((alpha_ == 1.0) && (beta_ == 1.0))
  {
    tauKendall(1, 0) = 1.0;
  }
  else
  {
    tauKendall(1, 0) = alpha_ * beta_ / (alpha_ + beta_ - alpha_ * beta_);
  }
  return tauKendall;
}

/* Get the Spearman correlation of the distribution
*/
CorrelationMatrix MarshallOlkinCopula::getSpearmanCorrelation() const
{
  CorrelationMatrix rho(2);
  if ((alpha_ == 0.0) || (beta_ == 0.0))
  {
    rho(1, 0) = 0.0;
  }
  else if ((alpha_ == 1.0) && (beta_ == 1.0))
  {
    rho(1, 0) = 1.0;
  }
  else
  {
    rho(1, 0) = 3 * alpha_ * beta_ / (2 * alpha_ + 2 * beta_ - alpha_ * beta_);
  }
  return rho;
}

/* Get one realization of the distribution
Uses the algorithm from Devroye (1987) and
presented in
“An Introduction to Copulas”, 2nd Edition, Nelsen, Springer.
Section Exercice 3.4, page 58.
 */
Point MarshallOlkinCopula::getRealization() const
{
  Point realization(2);
  // We use the general algorithm based on conditional CDF inversion
  if ((alpha_ == 0.0) || (beta_ == 0.0))
  {
    // This is the independent copula
    const Scalar u = RandomGenerator::Generate();
    const Scalar v = RandomGenerator::Generate();
    realization[0] = u;
    realization[1] = v;
  }
  else if ((alpha_ == 1.0) && (beta_ == 1.0))
  {
    // This is the min-copula
    const Scalar u = RandomGenerator::Generate();
    realization[0] = u;
    realization[1] = u;
  }
  else
  {
    const Scalar r = RandomGenerator::Generate();
    const Scalar s = RandomGenerator::Generate();
    const Scalar t = RandomGenerator::Generate();
    const Scalar logr = std::log(r);
    const Scalar logs = std::log(s);
    const Scalar logt = std::log(t);
    /* Given the three parameters lambda1, lambda2, lambda12,
    we have
    alpha = lambda12 /(lambda1 + lambda12 )
    beta = lambda12 /(lambda2 + lambda12 )
    We invert the equations:
    lambda1 = lambda12 / alpha - lambda12
    lambda2 = lambda12 / beta - lambda12
    and set lambda12 to 1.
    */
    const Scalar lambda12 = 1.0;
    const Scalar lambda1 = lambda12 / alpha_ - lambda12;
    const Scalar lambda2 = lambda12 / beta_ - lambda12;
    const Scalar x = std::min(- logr / lambda1, - logt);
    const Scalar y = std::min(- logs / lambda2, - logt);
    const Scalar u = std::exp(- (lambda1 + lambda12) * x);
    const Scalar v = std::exp(- (lambda2 + lambda12) * y);
    realization[0] = u;
    realization[1] = v;
  }
  return realization;
}


/* Method save() stores the object through the StorageManager */
void MarshallOlkinCopula::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void MarshallOlkinCopula::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
