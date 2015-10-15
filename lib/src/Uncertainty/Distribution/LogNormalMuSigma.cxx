//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma as parameters
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

#include "LogNormalFactory.hxx"
#include "SquareMatrix.hxx"
#include "LogNormalMuSigma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogNormalMuSigma);

/* Default constructor */
LogNormalMuSigma::LogNormalMuSigma()
  : DistributionParametersImplementation()
  , mu_(exp(0.5))
  , sigma_(sqrt(exp(2.0) - exp(1.0)))
  , gamma_(0.)
{
  // Nothing to do
}

LogNormalMuSigma::LogNormalMuSigma(const NumericalScalar mu, const NumericalScalar sigma, const NumericalScalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
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
  NumericalPoint newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  NumericalPointCollection nativeParameters(1, operator()(newParameters));

  return LogNormalFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix LogNormalMuSigma::gradient() const
{
  const NumericalScalar mu = mu_;
  const NumericalScalar sigma = sigma_;
  const NumericalScalar gamma = gamma_;

  NumericalPoint newParameters(3);
  newParameters[0] = mu;
  newParameters[1] = sigma;
  newParameters[2] = gamma;
  const NumericalScalar sigmalog = operator()(newParameters)[1];

  const NumericalScalar dsigmalogdmu = - sigma * sigma / (sigmalog * (mu - gamma) * (sigma * sigma + (mu - gamma) * (mu - gamma)));
  const NumericalScalar dsigmalogdsigma = sigma / (sigmalog * (sigma * sigma + (mu - gamma) * (mu - gamma)));
  const NumericalScalar dsigmalogdgamma = -dsigmalogdmu;
  const NumericalScalar dmulogdmu = 1 / (mu - gamma) - sigmalog * dsigmalogdmu;
  const NumericalScalar dmulogdsigma = -sigmalog * dsigmalogdsigma;
  const NumericalScalar dmulogdgamma = -dmulogdmu;

  SquareMatrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(0, 0) = dmulogdmu;
  nativeParametersGradient(1, 0) = dmulogdsigma;
  nativeParametersGradient(2, 0) = dmulogdgamma;

  nativeParametersGradient(0, 1) = dsigmalogdmu;
  nativeParametersGradient(1, 1) = dsigmalogdsigma;
  nativeParametersGradient(2, 1) = dsigmalogdgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
NumericalPoint LogNormalMuSigma::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar mu = inP[0];
  const NumericalScalar sigma = inP[1];
  const NumericalScalar gamma = inP[2];

  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;

  NumericalPoint nativeParameters(inP);
  NumericalScalar sigmaLog = sqrt(log(1 + sigma * sigma / ((mu - gamma) * (mu - gamma))));
  NumericalScalar muLog = log(mu - gamma) - sigmaLog * sigmaLog / 2;
  nativeParameters[0] = muLog;
  nativeParameters[1] = sigmaLog;

  return nativeParameters;
}


NumericalPoint LogNormalMuSigma::inverse(const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar muLog = inP[0];
  const NumericalScalar sigmaLog = inP[1];
  const NumericalScalar gamma = inP[2];

  if (sigmaLog <= 0.0) throw InvalidArgumentException(HERE) << "SigmaLog MUST be positive, here sigmaLog=" << sigmaLog;

  const NumericalScalar mu = gamma + std::exp(muLog + 0.5 * sigmaLog * sigmaLog);
  const NumericalScalar expSigmaLog2 = std::exp(sigmaLog * sigmaLog);
  const NumericalScalar sigma = std::exp(muLog) * std::sqrt(expSigmaLog2 * (expSigmaLog2 - 1.0));

  NumericalPoint muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
NumericalPointWithDescription LogNormalMuSigma::getParameters() const
{
  NumericalPointWithDescription point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  Description description(point.getDimension());
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  point.setDescription(description);
  return point;
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


String LogNormalMuSigma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS