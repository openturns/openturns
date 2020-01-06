//                                               -*- C++ -*-
/**
 *  @brief Gumbel distribution with a and b as parameters
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/GumbelFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/GumbelLambdaGamma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelLambdaGamma)
static const Factory<GumbelLambdaGamma> Factory_GumbelLambdaGamma;

/* Default constructor */
GumbelLambdaGamma::GumbelLambdaGamma()
  : DistributionParametersImplementation()
  , lambda_(0.0)
  , gamma_(1.0)
{
  // Nothing to do
}

GumbelLambdaGamma::GumbelLambdaGamma(const Scalar lambda, const Scalar gamma)
  : DistributionParametersImplementation()
  , lambda_(lambda)
  , gamma_(gamma)
{
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda must be > 0, here lambda=" << lambda;
}

/* Virtual constructor */
GumbelLambdaGamma * GumbelLambdaGamma::clone() const
{
  return new GumbelLambdaGamma(*this);
}

/* Comparison operator */
Bool GumbelLambdaGamma::operator ==(const GumbelLambdaGamma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution GumbelLambdaGamma::getDistribution() const
{
  Point newParameters(2);
  newParameters[0] = lambda_;
  newParameters[1] = gamma_;

  Point nativeParameters(operator()(newParameters));
  return GumbelFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix GumbelLambdaGamma::gradient() const
{
  const Scalar dbetadlambda = -1.0 / (lambda_ * lambda_);
  const Scalar dbetadgamma = 0.0;
  const Scalar dgammadlambda = 0.0;
  const Scalar dgammadgamma = 1.0;

  Matrix nativeParametersGradient(IdentityMatrix(2));
  nativeParametersGradient(0, 0) = dbetadlambda;
  nativeParametersGradient(1, 0) = dbetadgamma;

  nativeParametersGradient(0, 1) = dgammadlambda;
  nativeParametersGradient(1, 1) = dgammadgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point GumbelLambdaGamma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar lambda = inP[0];

  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "lambda must be > 0, here lambda=" << lambda;

  const Scalar beta = 1.0 / lambda;

  Point nativeParameters(inP);
  nativeParameters[0] = beta;

  return nativeParameters;
}


Point GumbelLambdaGamma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar beta = inP[0];

  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "beta must be > 0, here beta=" << beta;

  const Scalar lambda = 1.0 / beta;

  Point abParameters(inP);
  abParameters[0] = lambda;

  return abParameters;
}

/* Parameters value and description accessor */
void GumbelLambdaGamma::setValues(const Point & inP)
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  lambda_ = inP[0];
  gamma_ = inP[1];
}

Point GumbelLambdaGamma::getValues() const
{
  Point point(2);
  point[0] = lambda_;
  point[1] = gamma_;
  return point;
}

Description GumbelLambdaGamma::getDescription() const
{
  Description description(2);
  description[0] = "lambda";
  description[1] = "gamma";
  return description;
}

/* String converter */
String GumbelLambdaGamma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " lambda=" << lambda_
      << " gamma=" << gamma_;
  return oss;
}


String GumbelLambdaGamma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(a = " << lambda_ << ", b = " << gamma_ << ")";
  return oss;
}

void GumbelLambdaGamma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

void GumbelLambdaGamma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "gamma_", gamma_ );
}

END_NAMESPACE_OPENTURNS
