//                                               -*- C++ -*-
/**
 *  @brief Weibull distribution with mu and sigma as parameters
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
#include "WeibullFactory.hxx"
#include "SquareMatrix.hxx"
#include "SpecFunc.hxx"
#include "WeibullMuSigma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeibullMuSigma);

/* Default constructor */
WeibullMuSigma::WeibullMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
  , gamma_(0.)
{
  // Nothing to do
}

WeibullMuSigma::WeibullMuSigma(const NumericalScalar mu, const NumericalScalar sigma, const NumericalScalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;
}

/* Virtual constructor */
WeibullMuSigma * WeibullMuSigma::clone() const
{
  return new WeibullMuSigma(*this);
}

/* Comparison operator */
Bool WeibullMuSigma::operator ==(const WeibullMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution WeibullMuSigma::getDistribution() const
{
  NumericalPoint newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  NumericalPoint nativeParameters(operator()(newParameters));

  return WeibullFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix WeibullMuSigma::gradient() const
{
  NumericalPoint newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  // Use finite difference technique
  const NumericalScalar epsilon = 1e-5;
  NumericalPoint pt = NumericalPoint(3);

  pt[0] = epsilon;
  const NumericalScalar dalphadmu = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[0];
  const NumericalScalar dbetadmu = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[1];

  pt[0] = 0.;
  pt[1] = epsilon;
  const NumericalScalar dalphadsigma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[0];
  const NumericalScalar dbetadsigma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[1];

  pt[1] = 0.;
  pt[2] = epsilon;
  const NumericalScalar dalphadgamma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[0];
  const NumericalScalar dbetadgamma = ((operator()(newParameters + pt) - operator()(newParameters - pt)) / (2 * epsilon))[1];

  SquareMatrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(0, 0) = dalphadmu;
  nativeParametersGradient(1, 0) = dalphadsigma;
  nativeParametersGradient(2, 0) = dalphadgamma;

  nativeParametersGradient(0, 1) = dbetadmu;
  nativeParametersGradient(1, 1) = dbetadsigma;
  nativeParametersGradient(2, 1) = dbetadgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
NumericalPoint WeibullMuSigma::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar mu = inP[0];
  const NumericalScalar sigma = inP[1];
  const NumericalScalar gamma = inP[2];

  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;

  NumericalScalar alpha = 0.;
  NumericalScalar beta = 0.;
  const NumericalScalar ratio(1.0 + pow(sigma / (mu - gamma), 2.0));
  NumericalScalar t;
  NumericalScalar betaMin(1.0);
  NumericalScalar betaMax(1.0);
  NumericalScalar step(0.5);

  // Bracketing interval
  // Case beta < 1, i.e. ratio > 2
  if (ratio > 2)
  {
    do
    {
      betaMin -= step;
      step *= 0.5;
      t = exp(SpecFunc::LnGamma(1.0 + 2.0 / betaMin) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / betaMin));
    }
    while (t < ratio);
    // Here, we know that betaMin <= beta < betaMin + 2.0 * step
    betaMax = betaMin + 2.0 * step;
  }
  // Case beta >= 1, i.e. ratio <= 2
  else
  {
    do
    {
      betaMax += step;
      step *= 2.0;
      t = exp(SpecFunc::LnGamma(1.0 + 2.0 / betaMax) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / betaMax));
    }
    while (t >= ratio);
    // Here, we know that betaMax - 0.5 * step <= beta < betaMax
    betaMin = betaMax - 0.5 * step;
  }
  // Bisection loop
  for (;;)
  {
    beta = 0.5 * (betaMin + betaMax);
    // Convergence
    if (betaMax - betaMin <= ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultQuantileEpsilon") * (1.0 + fabs(betaMax + betaMin)))
    {
      alpha = (mu - gamma) / SpecFunc::Gamma(1.0 + 1.0 / beta);
      break;
    }
    // Non convergence, one step further
    t = exp(SpecFunc::LnGamma(1.0 + 2.0 / beta) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / beta));
    if (t < ratio) betaMax = beta;
    else betaMin = beta;
  }

  NumericalPoint nativeParameters(inP);
  nativeParameters[0] = alpha;
  nativeParameters[1] = beta;

  return nativeParameters;
}


NumericalPoint WeibullMuSigma::inverse(const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar alpha = inP[0];
  const NumericalScalar beta = inP[1];
  const NumericalScalar gamma = inP[2];

  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (beta <= 0.0) throw InvalidArgumentException(HERE) << "Beta MUST be positive";

  const NumericalScalar mu = gamma + alpha * SpecFunc::Gamma(1.0 + 1.0 / beta);
  const NumericalScalar sigma = alpha * std::sqrt(SpecFunc::Gamma(1.0 + 2.0 / beta) - std::pow(SpecFunc::Gamma(1.0 + 1.0 / beta), 2.0));

  NumericalPoint muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void WeibullMuSigma::setValues(const NumericalPoint & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
  gamma_ = inP[2];
}

NumericalPoint WeibullMuSigma::getValues() const
{
  NumericalPoint point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  return point;
}

Description WeibullMuSigma::getDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  return description;
}

/* String converter */
String WeibullMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " gamma=" << gamma_;
  return oss;
}


String WeibullMuSigma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS