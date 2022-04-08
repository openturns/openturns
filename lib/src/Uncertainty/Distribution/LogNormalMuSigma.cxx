//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/LogNormalFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/LogNormalMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogNormalMuSigma)
static const Factory<LogNormalMuSigma> Factory_LogNormalMuSigma;

/* Default constructor */
LogNormalMuSigma::LogNormalMuSigma()
  : DistributionParametersImplementation()
  , mu_(exp(0.5))
  , sigma_(sqrt(exp(2.0) - exp(1.0)))
  , gamma_(0.)
{
  // Nothing to do
}

LogNormalMuSigma::LogNormalMuSigma(const Scalar mu, const Scalar sigma, const Scalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;
}

/* Virtual constructor */
LogNormalMuSigma * LogNormalMuSigma::clone() const
{
  return new LogNormalMuSigma(*this);
}

/* Comparison operator */
Bool LogNormalMuSigma::operator ==(const LogNormalMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution LogNormalMuSigma::getDistribution() const
{
  Point newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  Point nativeParameters(operator()(newParameters));

  return LogNormalFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix LogNormalMuSigma::gradient() const
{
  const Scalar mu = mu_;
  const Scalar sigma = sigma_;
  const Scalar gamma = gamma_;

  Point newParameters(3);
  newParameters[0] = mu;
  newParameters[1] = sigma;
  newParameters[2] = gamma;
  const Scalar sigmalog = operator()(newParameters)[1];

  const Scalar dsigmalogdmu = - sigma * sigma / (sigmalog * (mu - gamma) * (sigma * sigma + (mu - gamma) * (mu - gamma)));
  const Scalar dsigmalogdsigma = sigma / (sigmalog * (sigma * sigma + (mu - gamma) * (mu - gamma)));
  const Scalar dsigmalogdgamma = -dsigmalogdmu;
  const Scalar dmulogdmu = 1 / (mu - gamma) - sigmalog * dsigmalogdmu;
  const Scalar dmulogdsigma = -sigmalog * dsigmalogdsigma;
  const Scalar dmulogdgamma = -dmulogdmu;

  Matrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(0, 0) = dmulogdmu;
  nativeParametersGradient(1, 0) = dmulogdsigma;
  nativeParametersGradient(2, 0) = dmulogdgamma;

  nativeParametersGradient(0, 1) = dsigmalogdmu;
  nativeParametersGradient(1, 1) = dsigmalogdsigma;
  nativeParametersGradient(2, 1) = dsigmalogdgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point LogNormalMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];
  const Scalar gamma = inP[2];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;

  Point nativeParameters(inP);
  Scalar sigmalog = sqrt(log(1 + sigma * sigma / ((mu - gamma) * (mu - gamma))));
  Scalar mulog = log(mu - gamma) - sigmalog * sigmalog / 2;
  nativeParameters[0] = mulog;
  nativeParameters[1] = sigmalog;

  return nativeParameters;
}


Point LogNormalMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar muLog = inP[0];
  const Scalar sigmaLog = inP[1];
  const Scalar gamma = inP[2];

  if (!(sigmaLog > 0.0)) throw InvalidArgumentException(HERE) << "SigmaLog MUST be positive, here sigmaLog=" << sigmaLog;

  const Scalar mu = gamma + std::exp(muLog + 0.5 * sigmaLog * sigmaLog);
  const Scalar expSigmaLog2 = std::exp(sigmaLog * sigmaLog);
  const Scalar sigma = std::exp(muLog) * std::sqrt(expSigmaLog2 * (expSigmaLog2 - 1.0));

  Point muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void LogNormalMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
  gamma_ = inP[2];
}

Point LogNormalMuSigma::getValues() const
{
  Point point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  return point;
}

Description LogNormalMuSigma::getDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  return description;
}

/* String converter */
String LogNormalMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " gamma=" << gamma_;
  return oss;
}


String LogNormalMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
  return oss;
}

void LogNormalMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

void LogNormalMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "gamma_", gamma_ );
}

END_NAMESPACE_OPENTURNS
