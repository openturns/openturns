//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma over mu as parameters
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/LogNormalFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/LogNormalMuSigma.hxx"
#include "openturns/LogNormalMuSigmaOverMu.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogNormalMuSigmaOverMu);

/* Default constructor */
LogNormalMuSigmaOverMu::LogNormalMuSigmaOverMu()
  : DistributionParametersImplementation()
  , mu_(exp(0.5))
  , sigmaOverMu_(sqrt(exp(2.0) - exp(1.0)) / exp(0.5))
  , gamma_(0.)
{
  // Nothing to do
}

LogNormalMuSigmaOverMu::LogNormalMuSigmaOverMu(const NumericalScalar mu, const NumericalScalar sigmaOverMu, const NumericalScalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigmaOverMu_(sigmaOverMu)
  , gamma_(gamma)
{
  if (mu == 0.0) throw InvalidArgumentException(HERE) << "mu cannot be null in the parameter set (mu, sigmaOverMu)";
  if (!(sigmaOverMu * mu > 0.0)) throw InvalidArgumentException(HERE) << "sigmaOverMu*mu must be > 0, here sigmaOverMu*mu=" << sigmaOverMu*mu;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;
}

/* Virtual constructor */
LogNormalMuSigmaOverMu * LogNormalMuSigmaOverMu::clone() const
{
  return new LogNormalMuSigmaOverMu(*this);
}

/* Comparison operator */
Bool LogNormalMuSigmaOverMu::operator ==(const LogNormalMuSigmaOverMu & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution LogNormalMuSigmaOverMu::getDistribution() const
{
  NumericalPoint newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigmaOverMu_;
  newParameters[2] = gamma_;

  NumericalPoint nativeParameters(operator()(newParameters));

  return LogNormalFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix LogNormalMuSigmaOverMu::gradient() const
{
// compute the jacobian of the transformation (mulog, sigmalog, gamma) -> (mu, sigma, gamma)
  const LogNormalMuSigma muSigmaParameters(mu_, sigmaOverMu_ * mu_, gamma_);
  const Matrix muSigmaJacobian = muSigmaParameters.gradient();

  // compute the jacobian of the transformation (mu, sigma, gamma) -> (mu, sigma/mu, gamma)
  SquareMatrix muSigmaOverMuJacobian(IdentityMatrix(3));
  muSigmaOverMuJacobian(0, 1) =  sigmaOverMu_;
  muSigmaOverMuJacobian(1, 1) =  mu_;

  return muSigmaOverMuJacobian * muSigmaJacobian;
}


/* Conversion operator */
NumericalPoint LogNormalMuSigmaOverMu::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar mu = inP[0];
  const NumericalScalar sigmaOverMu = inP[1];
  const NumericalScalar gamma = inP[2];

  if (mu == 0.0) throw InvalidArgumentException(HERE) << "mu cannot be null in the parameter set (mu, sigmaOverMu)";
  if (!(sigmaOverMu * mu > 0.0)) throw InvalidArgumentException(HERE) << "sigmaOverMu*mu must be > 0, here sigmaOverMu*mu=" << sigmaOverMu*mu;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;

  NumericalPoint muSigmaParametersValues(inP);
  muSigmaParametersValues[1] *= mu;
  const LogNormalMuSigma muSigmaParameters(mu, sigmaOverMu * mu, gamma);

  return muSigmaParameters(muSigmaParametersValues);
}


NumericalPoint LogNormalMuSigmaOverMu::inverse(const NumericalPoint & inP) const
{
  const LogNormalMuSigma muSigmaParameters;
  NumericalPoint muSigmaOverMuParameters(muSigmaParameters.inverse(inP));
  const NumericalScalar mu = muSigmaOverMuParameters[0];
  if (mu == 0.0) throw InvalidArgumentException(HERE) << "Error: mu cannot be null in the parameter set (mu, sigmaOverMu)";
  muSigmaOverMuParameters[1] /= mu;
  return muSigmaOverMuParameters;
}

/* Parameters value and description accessor */
void LogNormalMuSigmaOverMu::setValues(const NumericalPoint & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigmaOverMu_ = inP[1];
  gamma_ =  inP[2];
}

NumericalPoint LogNormalMuSigmaOverMu::getValues() const
{
  NumericalPoint point(3);
  point[0] = mu_;
  point[1] = sigmaOverMu_;
  point[2] = gamma_;
  return point;
}

Description LogNormalMuSigmaOverMu::getDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigmaOverMu";
  description[2] = "gamma";
  return description;
}

/* String converter */
String LogNormalMuSigmaOverMu::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigmaOverMu=" << sigmaOverMu_
      << " gamma=" << gamma_;
  return oss;
}


String LogNormalMuSigmaOverMu::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigmaOverMu = " << sigmaOverMu_ << ", gamma = " << gamma_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS