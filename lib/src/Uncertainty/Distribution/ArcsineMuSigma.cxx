//                                               -*- C++ -*-
/**
 *  @brief Arcsine distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ArcsineFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/ArcsineMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ArcsineMuSigma)

static const Factory<ArcsineMuSigma> Factory_ArcsineMuSigma;
/* Default constructor */
ArcsineMuSigma::ArcsineMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
{
  // Nothing to do
}

ArcsineMuSigma::ArcsineMuSigma(const Scalar mu, const Scalar sigma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
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
  Point newParameters(2);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;

  Point nativeParameters(operator()(newParameters));

  return ArcsineFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix ArcsineMuSigma::gradient() const
{
  const Scalar dadmu = 1.0;
  const Scalar dadsigma = -sqrt(2.0);
  const Scalar dbdmu = 1.0;
  const Scalar dbdsigma = sqrt(2.0);

  Matrix nativeParametersGradient(IdentityMatrix(2));
  nativeParametersGradient(0, 0) = dadmu;
  nativeParametersGradient(1, 0) = dadsigma;

  nativeParametersGradient(0, 1) = dbdmu;
  nativeParametersGradient(1, 1) = dbdsigma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point ArcsineMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  const Scalar a = mu - sigma * sqrt(2.0);
  const Scalar b = mu + sigma * sqrt(2.0);

  Point nativeParameters(inP);
  nativeParameters[0] = a;
  nativeParameters[1] = b;

  return nativeParameters;
}


Point ArcsineMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar a = inP[0];
  const Scalar b = inP[1];

  if (a >= b) throw InvalidArgumentException(HERE) << "a must be smaller than b";

  const Scalar mu = (a + b) / 2.0;
  const Scalar sigma = 0.5 * (b - a) * M_SQRT1_2;

  Point muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void ArcsineMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
}

Point ArcsineMuSigma::getValues() const
{
  Point point(2);
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


String ArcsineMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ")";
  return oss;
}

void ArcsineMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
}

/* Method load() reloads the object from the StorageManager */
void ArcsineMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
}

END_NAMESPACE_OPENTURNS
