//                                               -*- C++ -*-
/**
 *  @brief Chebychev polynomial factory
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
#include "openturns/ChebychevFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Arcsine.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ChebychevFactory)

static const Factory<ChebychevFactory> Factory_ChebychevFactory;

/* Default constructor */
ChebychevFactory::ChebychevFactory()
  : OrthogonalUniVariatePolynomialFactory(Arcsine(-1.0, 1.0))
{
  initializeCache();
}

/* Virtual constructor */
ChebychevFactory * ChebychevFactory::clone() const
{
  return new ChebychevFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
ChebychevFactory::Coefficients ChebychevFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    recurrenceCoefficients[0] = M_SQRT2;
    return recurrenceCoefficients;
  }
  recurrenceCoefficients[0] = 2.0;
  if (n == 1)
  {
    recurrenceCoefficients[2] = -M_SQRT2;
    return recurrenceCoefficients;
  }
  recurrenceCoefficients[2] = -1.0;
  return recurrenceCoefficients;
}

/* Roots of the polynomial of degree n */
Point ChebychevFactory::getRoots(const UnsignedInteger n) const
{
  Point roots(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    roots[i] = std::cos((i + 0.5) * M_PI / n);
  return roots;
}

/* Nodes and weights of the polynomial of degree n as the eigenvalues of the associated Jacobi matrix and the square
   of the first component of the associated normalized eigenvectors */
Point ChebychevFactory::getNodesAndWeights(const UnsignedInteger n,
    Point & weights) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: cannot compute the roots and weights of a constant polynomial.";
  weights = Point(n, 1.0 / n);
  return getRoots(n);
}

/* String converter */
String ChebychevFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}


END_NAMESPACE_OPENTURNS
