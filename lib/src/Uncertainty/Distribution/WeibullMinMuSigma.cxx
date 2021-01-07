//                                               -*- C++ -*-
/**
 *  @brief WeibullMin distribution with mu and sigma as parameters
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
#include "openturns/WeibullMinFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/WeibullMinMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeibullMinMuSigma)
static const Factory<WeibullMinMuSigma> Factory_WeibullMinMuSigma;

/* Default constructor */
WeibullMinMuSigma::WeibullMinMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
  , gamma_(0.)
{
  // Nothing to do
}

WeibullMinMuSigma::WeibullMinMuSigma(const Scalar mu, const Scalar sigma, const Scalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;
}

/* Virtual constructor */
WeibullMinMuSigma * WeibullMinMuSigma::clone() const
{
  return new WeibullMinMuSigma(*this);
}

/* Comparison operator */
Bool WeibullMinMuSigma::operator ==(const WeibullMinMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution WeibullMinMuSigma::getDistribution() const
{
  Point newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  Point nativeParameters(operator()(newParameters));

  return WeibullMinFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix WeibullMinMuSigma::gradient() const
{
  Point newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  // Use finite difference technique
  const Scalar epsilon = 1e-5;
  Point pt = Point(3);

  pt[0] = epsilon;
  const Scalar dbetadmu = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[0];
  const Scalar dalphadmu = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[1];

  pt[0] = 0.;
  pt[1] = epsilon;
  const Scalar dbetadsigma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[0];
  const Scalar dalphadsigma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[1];

  pt[1] = 0.;
  pt[2] = epsilon;
  const Scalar dbetadgamma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[0];
  const Scalar dalphadgamma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[1];

  Matrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(0, 0) = dbetadmu;
  nativeParametersGradient(1, 0) = dbetadsigma;
  nativeParametersGradient(2, 0) = dbetadgamma;

  nativeParametersGradient(0, 1) = dalphadmu;
  nativeParametersGradient(1, 1) = dalphadsigma;
  nativeParametersGradient(2, 1) = dalphadgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point WeibullMinMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];
  const Scalar gamma = inP[2];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;

  Scalar alpha = 0.0;
  Scalar beta = 0.0;
  const Scalar ratio = 1.0 + pow(sigma / (mu - gamma), 2.0);
  Scalar t = -1.0;
  Scalar alphaMin = 1.0;
  Scalar alphaMax = 1.0;
  Scalar step = 0.5;

  // Bracketing interval
  // Case alpha < 1, i.e. ratio > 2
  if (ratio > 2)
  {
    do
    {
      alphaMin -= step;
      step *= 0.5;
      t = exp(SpecFunc::LnGamma(1.0 + 2.0 / alphaMin) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / alphaMin));
    }
    while (t < ratio);
    // Here, we know that alphaMin <= alpha < alphaMin + 2.0 * step
    alphaMax = alphaMin + 2.0 * step;
  }
  // Case alpha >= 1, i.e. ratio <= 2
  else
  {
    do
    {
      alphaMax += step;
      step *= 2.0;
      t = exp(SpecFunc::LnGamma(1.0 + 2.0 / alphaMax) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / alphaMax));
    }
    while (t >= ratio);
    // Here, we know that alphaMax - 0.5 * step <= alpha < alphaMax
    alphaMin = alphaMax - 0.5 * step;
  }
  // Bisection loop
  for (;;)
  {
    alpha = 0.5 * (alphaMin + alphaMax);
    // Convergence
    if (alphaMax - alphaMin <= ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon") * (1.0 + std::abs(alphaMax + alphaMin)))
    {
      beta = (mu - gamma) / SpecFunc::Gamma(1.0 + 1.0 / alpha);
      break;
    }
    // Non convergence, one step further
    t = exp(SpecFunc::LnGamma(1.0 + 2.0 / alpha) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / alpha));
    if (t < ratio) alphaMax = alpha;
    else alphaMin = alpha;
  }

  Point nativeParameters(inP);
  nativeParameters[0] = beta;
  nativeParameters[1] = alpha;

  return nativeParameters;
}


Point WeibullMinMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar beta = inP[0];
  const Scalar alpha = inP[1];
  const Scalar gamma = inP[2];

  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";

  const Scalar mu = gamma + beta * SpecFunc::Gamma(1.0 + 1.0 / alpha);
  const Scalar sigma = beta * std::sqrt(SpecFunc::Gamma(1.0 + 2.0 / alpha) - std::pow(SpecFunc::Gamma(1.0 + 1.0 / alpha), 2.0));

  Point muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void WeibullMinMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
  gamma_ = inP[2];
}

Point WeibullMinMuSigma::getValues() const
{
  Point point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  return point;
}

Description WeibullMinMuSigma::getDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  return description;
}

/* String converter */
String WeibullMinMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " gamma=" << gamma_;
  return oss;
}


String WeibullMinMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
  return oss;
}

void WeibullMinMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

void WeibullMinMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "gamma_", gamma_ );
}

END_NAMESPACE_OPENTURNS
