//                                               -*- C++ -*-
/**
 *  @brief Arcsine distribution with mu and sigma as parameters
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

#include <cmath>
#include "openturns/ArcsineFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/ArcsineMuSigma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ArcsineMuSigma);

/* Default constructor */
ArcsineMuSigma::ArcsineMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
{
  // Nothing to do
}

ArcsineMuSigma::ArcsineMuSigma(const NumericalScalar mu, const NumericalScalar sigma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
}

/* Virtual constructor */
ArcsineMuSigma * ArcsineMuSigma::clone() const
{
  return new ArcsineMuSigma(*this);
}

/* Comparison operator */
Bool ArcsineMuSigma::operator ==(const ArcsineMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution ArcsineMuSigma::getDistribution() const
{
  NumericalPoint newParameters(2);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;

  NumericalPoint nativeParameters(operator()(newParameters));

  return ArcsineFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix ArcsineMuSigma::gradient() const
{
  const NumericalScalar dadmu = 1.0;
  const NumericalScalar dadsigma = -sqrt(2.0);
  const NumericalScalar dbdmu = 1.0;
  const NumericalScalar dbdsigma = sqrt(2.0);

  SquareMatrix nativeParametersGradient(IdentityMatrix(2));
  nativeParametersGradient(0, 0) = dadmu;
  nativeParametersGradient(1, 0) = dadsigma;

  nativeParametersGradient(0, 1) = dbdmu;
  nativeParametersGradient(1, 1) = dbdsigma;

  return nativeParametersGradient;
}


/* Conversion operator */
NumericalPoint ArcsineMuSigma::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const NumericalScalar mu = inP[0];
  const NumericalScalar sigma = inP[1];

  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  const NumericalScalar a = mu - sigma * sqrt(2.0);
  const NumericalScalar b = mu + sigma * sqrt(2.0);

  NumericalPoint nativeParameters(inP);
  nativeParameters[0] = a;
  nativeParameters[1] = b;

  return nativeParameters;
}


NumericalPoint ArcsineMuSigma::inverse(const NumericalPoint & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const NumericalScalar a = inP[0];
  const NumericalScalar b = inP[1];

  if (a >= b) throw InvalidArgumentException(HERE) << "a must be smaller than b";

  const NumericalScalar mu = (a + b) / 2.0;
  const NumericalScalar sigma = 0.5 * (b - a) * M_SQRT1_2;

  NumericalPoint muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void ArcsineMuSigma::setValues(const NumericalPoint & inP)
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
}

NumericalPoint ArcsineMuSigma::getValues() const
{
  NumericalPoint point(2);
  point[0] = mu_;
  point[1] = sigma_;
  return point;
}

Description ArcsineMuSigma::getDescription() const
{
  Description description(2);
  description[0] = "mu";
  description[1] = "sigma";
  return description;
}

/* String converter */
String ArcsineMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_;
  return oss;
}


String ArcsineMuSigma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS