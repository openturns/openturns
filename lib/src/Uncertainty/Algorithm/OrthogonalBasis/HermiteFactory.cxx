//                                               -*- C++ -*-
/**
 *  @brief Hermite polynomial factory
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HermiteFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/FastHermite.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(HermiteFactory)

static const Factory<HermiteFactory> Factory_HermiteFactory;


/* Default constructor */
HermiteFactory::HermiteFactory()
  : OrthogonalUniVariatePolynomialFactory(Normal())
{
  initializeCache();
}


/* Constructor with arbitrary Normal parameters */
HermiteFactory::HermiteFactory(const Scalar mu,
                               const Scalar sigma)
  : OrthogonalUniVariatePolynomialFactory(Normal(mu, sigma))
{
  // Normal has infinite support so the parent constructor cannot compute
  // the affine transform from range bounds. Set it directly:
  // z = (x - mu) / sigma, so a_ = 1/sigma, b_ = -mu/sigma.
  a_ = 1.0 / sigma;
  b_ = -mu / sigma;
  initializeCache();
}


/* Virtual constructor */
HermiteFactory * HermiteFactory::clone() const
{
  return new HermiteFactory(*this);
}

/* Comparison operators */
Bool HermiteFactory::operator ==(const HermiteFactory & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other);
}

Bool HermiteFactory::equals(const OrthogonalUniVariatePolynomialFactory & other) const
{
  const HermiteFactory * p_other = dynamic_cast<const HermiteFactory *>(&other);
  return p_other && (*this == *p_other);
}

/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
HermiteFactory::Coefficients HermiteFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    recurrenceCoefficients[0] = 1.0;
    recurrenceCoefficients[1] = 0.0;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  recurrenceCoefficients[0] = 1.0 / sqrt(n + 1.0);
  recurrenceCoefficients[1] = 0.0;
  recurrenceCoefficients[2] = -sqrt(1.0 - 1.0 / (n + 1));
  return recurrenceCoefficients;
}


/* String converter */
String HermiteFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}


/* Roots of the polynomial of degree n */
Point HermiteFactory::getRoots(const UnsignedInteger n) const
{
  if (n == 0) return Point(0);
  Point weights(0);
  return getNodesAndWeights(n, weights);
}

/* Nodes and weights of the polynomial of degree n */
Point HermiteFactory::getNodesAndWeights(const UnsignedInteger n,
    Point & weightsOut) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: cannot compute the roots and weights of a constant polynomial.";
  Point nodes(n);
  weightsOut = Point(n);
  FastHermite::ComputeNodesAndWeights(n, &nodes[0], &weightsOut[0]);
  // Map the standard N(0, 1) nodes to the actual Normal(mu, sigma) measure:
  // x = (z - b_) / a_ with z = a_ * x + b_
  for (UnsignedInteger i = 0; i < n; ++i)
    nodes[i] = (nodes[i] - b_) / a_;
  return nodes;
}


/* Method save() stores the object through the StorageManager */
void HermiteFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void HermiteFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
}

END_NAMESPACE_OPENTURNS

