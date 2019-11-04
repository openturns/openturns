//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D polynomial factories
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
#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Normal.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariatePolynomialFactory)

static const Factory<OrthogonalUniVariatePolynomialFactory> Factory_OrthogonalUniVariatePolynomialFactory;


/* Default constructor */
OrthogonalUniVariatePolynomialFactory::OrthogonalUniVariatePolynomialFactory()
  : PersistentObject()
  , measure_()
  , coefficientsCache_(0)
  , recurrenceCoefficientsCache_(0)
  , polynomialsCache_(0)
{
  // Nothing to do. The derived class will have to call initializeCaches().
}


/* Constructor */
OrthogonalUniVariatePolynomialFactory::OrthogonalUniVariatePolynomialFactory(const Distribution & measure)
  : PersistentObject()
  , measure_(measure)
  , coefficientsCache_(0)
  , recurrenceCoefficientsCache_(0)
  , polynomialsCache_(0)
{
  // Nothing to do. The derived class will have to call initializeCaches().
}


/* Virtual constructor */
OrthogonalUniVariatePolynomialFactory * OrthogonalUniVariatePolynomialFactory::clone() const
{
  return new OrthogonalUniVariatePolynomialFactory(*this);
}


/* String converter */
String OrthogonalUniVariatePolynomialFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}


/* The method to get the polynomial of any degree */
OrthogonalUniVariatePolynomial OrthogonalUniVariatePolynomialFactory::build(const UnsignedInteger degree) const
{
  const UnsignedInteger cacheSize = polynomialsCache_.getSize();
  if (degree < cacheSize) return polynomialsCache_[degree];
  for (UnsignedInteger i = cacheSize; i <= degree; ++i)
  {
    CoefficientsCollection rec(buildRecurrenceCoefficientsCollection(i));
    polynomialsCache_.add(OrthogonalUniVariatePolynomial(rec, buildCoefficients(i)));
  };
  return polynomialsCache_[degree];
}


/* Build the coefficients of the polynomial based on the recurrence coefficients */
OrthogonalUniVariatePolynomialFactory::Coefficients OrthogonalUniVariatePolynomialFactory::buildCoefficients(const UnsignedInteger n) const
{
  const UnsignedInteger size = coefficientsCache_.getSize();
  // If we have already computed the coefficients
  if (n < size) return coefficientsCache_[n];
  // Else we have to compute all the coefficients from the last computed coefficients to the needed ones. The cache will be filled in the correct order thanks to the recursive call
  // Here, n >= 1 as the case n = 0 is already in the cache
  // Other cases
  Coefficients coefficientsN(n + 1);
  Coefficients coefficientsNMinus1(buildCoefficients(n - 1));
  // Leading term
  const Coefficients aN(getRecurrenceCoefficients(n - 1));
  coefficientsN[n] = aN[0] * coefficientsNMinus1[n - 1];
  // Case n == 1 is special as there is no call to the coefficients of degree n-2
  // Constant term, case n = 1
  coefficientsN[0] = aN[1] * coefficientsNMinus1[0];
  if (n == 1)
  {
    coefficientsCache_.add(coefficientsN);
    return coefficientsN;
  }
  // Constant term, case n >= 2
  Coefficients coefficientsNMinus2(buildCoefficients(n - 2));
  coefficientsN[0] += aN[2] * coefficientsNMinus2[0];
  // Leading term
  coefficientsN[n] = aN[0] * coefficientsNMinus1[n - 1];
  // Second leading term
  coefficientsN[n - 1] = aN[0] * coefficientsNMinus1[n - 2] + aN[1] * coefficientsNMinus1[n - 1];
  // Constant term
  coefficientsN[0] = aN[1] * coefficientsNMinus1[0] + aN[2] * coefficientsNMinus2[0];
  // Remaining terms
  for (UnsignedInteger i = 1; i < n - 1; ++i)
    coefficientsN[i] = aN[0] * coefficientsNMinus1[i - 1] + aN[1] * coefficientsNMinus1[i] + aN[2] * coefficientsNMinus2[i];
  coefficientsCache_.add(coefficientsN);
  return coefficientsN;
}

/* Build the 3 terms recurrence coefficients up to the needed degree */
OrthogonalUniVariatePolynomialFactory::CoefficientsCollection OrthogonalUniVariatePolynomialFactory::buildRecurrenceCoefficientsCollection(const UnsignedInteger degree) const
{
  CoefficientsCollection recurrenceCoefficients(degree);
  for (UnsignedInteger i = 0; i < degree; ++i) recurrenceCoefficients[i] = getRecurrenceCoefficients(i);
  return recurrenceCoefficients;
}

/* Measure accessor */
Distribution OrthogonalUniVariatePolynomialFactory::getMeasure() const
{
  return measure_;
}


/* Calculate the coefficients of recurrence a0, a1, a2 such that
   Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
OrthogonalUniVariatePolynomialFactory::Coefficients OrthogonalUniVariatePolynomialFactory::getRecurrenceCoefficients(const UnsignedInteger ) const
{
  throw NotYetImplementedException(HERE) << "In OrthogonalUniVariatePolynomialFactory::getRecurrenceCoefficients(const UnsignedInteger n) const";
}


/* Cache initialization */
void OrthogonalUniVariatePolynomialFactory::initializeCache()
{
  coefficientsCache_.add(Coefficients(1, 1.0));
  recurrenceCoefficientsCache_.add(getRecurrenceCoefficients(0));
}

/* Roots of the polynomial of degree n */
Point OrthogonalUniVariatePolynomialFactory::getRoots(const UnsignedInteger n) const
{
  // As a specialized UniVariatePolynomial, the roots are complex
  const OrthogonalUniVariatePolynomial::ComplexCollection complexRoots(build(n).getRoots());
  // But in fact we know that they are real
  Point roots(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    roots[i] = complexRoots[i].real();
  return roots;
}

/* Nodes and weights of the polynomial of degree n as the eigenvalues of the associated Jacobi matrix and the square
   of the first component of the associated normalized eigenvectors */
Point OrthogonalUniVariatePolynomialFactory::getNodesAndWeights(const UnsignedInteger n,
    Point & weights) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: cannot compute the roots and weights of a constant polynomial.";
  // gauss integration rule
  char jobz('V');
  int ljobz(1);
  Point d(n);
  Point e(n - 1);
  Coefficients recurrenceCoefficientsI(getRecurrenceCoefficients(0));
  Scalar alphaPrec = recurrenceCoefficientsI[0];
  d[0] = -recurrenceCoefficientsI[1] / alphaPrec;
  if (n == 1)
  {
    weights = Point(1, 1.0);
    return d;
  }
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    recurrenceCoefficientsI = getRecurrenceCoefficients(i);
    d[i]     = -recurrenceCoefficientsI[1] / recurrenceCoefficientsI[0];
    e[i - 1] = sqrt(-recurrenceCoefficientsI[2] / (recurrenceCoefficientsI[0] * alphaPrec));
    alphaPrec = recurrenceCoefficientsI[0];
  }
  int ldz(n);
  SquareMatrix z(n);
  Point work(2 * n - 2);
  int info;
  dstev_(&jobz, &ldz, &d[0], &e[0], &z(0, 0), &ldz, &work[0], &info, &ljobz);
  if (info != 0) throw InternalException(HERE) << "Lapack DSTEV: error code=" << info;
  weights = Point(n);
  for (UnsignedInteger i = 0; i < n; ++i) weights[i] = z(0, i) * z(0, i);
  return d;
}

/* Method save() stores the object through the StorageManager */
void OrthogonalUniVariatePolynomialFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "measure_", measure_ );
  adv.saveAttribute( "coefficientsCache_", coefficientsCache_ );
  adv.saveAttribute( "recurrenceCoefficientsCache_", recurrenceCoefficientsCache_ );
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalUniVariatePolynomialFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "measure_", measure_ );
  adv.loadAttribute( "coefficientsCache_", coefficientsCache_ );
  adv.loadAttribute( "recurrenceCoefficientsCache_", recurrenceCoefficientsCache_ );
}

END_NAMESPACE_OPENTURNS
