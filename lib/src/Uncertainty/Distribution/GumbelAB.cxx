//                                               -*- C++ -*-
/**
 *  @brief Gumbel distribution with a and b as parameters
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

#include "GumbelFactory.hxx"
#include "SquareMatrix.hxx"
#include "GumbelAB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelAB);

/* Default constructor */
GumbelAB::GumbelAB()
  : DistributionParametersImplementation()
  , a_(0.0)
  , b_(1.0)
{
  // Nothing to do
}

GumbelAB::GumbelAB(const NumericalScalar a, const NumericalScalar b)
  : DistributionParametersImplementation()
  , a_(a)
  , b_(b)
{
  if (b <= 0.0) throw InvalidArgumentException(HERE) << "b must be > 0, here b=" << b;
}

/* Virtual constructor */
GumbelAB * GumbelAB::clone() const
{
  return new GumbelAB(*this);
}

/* Comparison operator */
Bool GumbelAB::operator ==(const GumbelAB & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution GumbelAB::getDistribution() const
{
  NumericalPoint newParameters(2);
  newParameters[0] = a_;
  newParameters[1] = b_;

  NumericalPoint nativeParameters(operator()(newParameters));

  return GumbelFactory().build(operator()(nativeParameters));
}


/* Compute jacobian / native parameters */
Matrix GumbelAB::gradient() const
{
  const NumericalScalar dalphada = 0.0;
  const NumericalScalar dalphadb = -1 / (b_ * b_);
  const NumericalScalar dbetada = 1.0;
  const NumericalScalar dbetadb = 0.0;

  SquareMatrix nativeParametersGradient(IdentityMatrix(2));
  nativeParametersGradient(0, 0) = dalphada;
  nativeParametersGradient(1, 0) = dalphadb;

  nativeParametersGradient(0, 1) = dbetada;
  nativeParametersGradient(1, 1) = dbetadb;

  return nativeParametersGradient;
}


/* Conversion operator */
NumericalPoint GumbelAB::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const NumericalScalar a = inP[0];
  const NumericalScalar b = inP[1];

  if (b <= 0.0) throw InvalidArgumentException(HERE) << "b must be > 0, here b=" << b;

  const NumericalScalar alpha = 1 / b;
  const NumericalScalar beta = a;

  NumericalPoint nativeParameters(inP);
  nativeParameters[0] = alpha;
  nativeParameters[1] = beta;

  return nativeParameters;
}


NumericalPoint GumbelAB::inverse(const NumericalPoint & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const NumericalScalar alpha = inP[0];
  const NumericalScalar beta = inP[1];

  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "alpha must be > 0, here alpha=" << alpha;

  const NumericalScalar a = beta;
  const NumericalScalar b = 1.0 / alpha;

  NumericalPoint abParameters(inP);
  abParameters[0] = a;
  abParameters[1] = b;

  return abParameters;
}

/* Parameters value and description accessor */
NumericalPoint GumbelAB::getValues() const
{
  NumericalPoint point(2);
  point[0] = a_;
  point[1] = b_;
  return point;
}

Description GumbelAB::getDescription() const
{
  Description description(2);
  description[0] = "a";
  description[1] = "b";
  return description;
}

/* String converter */
String GumbelAB::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " a=" << a_
      << " b=" << b_;
  return oss;
}


String GumbelAB::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS