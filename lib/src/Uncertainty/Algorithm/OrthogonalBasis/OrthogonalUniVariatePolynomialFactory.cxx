//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D polynomial factories
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
#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariatePolynomialFactory)

static const Factory<OrthogonalUniVariatePolynomialFactory> Factory_OrthogonalUniVariatePolynomialFactory;


/* Default constructor */
OrthogonalUniVariatePolynomialFactory::OrthogonalUniVariatePolynomialFactory()
  : PersistentObject()
  , measure_()
  , recurrenceCoefficientsCache_(0, 3)
  , polynomialsCache_(0)
{
  // Nothing to do. The derived class will have to call initializeCaches().
}


/* Constructor */
OrthogonalUniVariatePolynomialFactory::OrthogonalUniVariatePolynomialFactory(const Distribution & measure)
  : PersistentObject()
  , measure_(measure)
  , recurrenceCoefficientsCache_(0, 3)
  , polynomialsCache_(0)
{
  // The derived class will have to call initializeCaches().
  if (measure.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error, expected a distribution of dimension 1, got dimension=" << measure.getDimension();
  const Interval initialRange(measure.getRange());
  const Scalar ai = initialRange.getLowerBound()[0];
  const Scalar bi = initialRange.getUpperBound()[0];
  const Interval standardRange(measure.getStandardRepresentative().getRange());
  const Scalar as = standardRange.getLowerBound()[0];
  const Scalar bs = standardRange.getUpperBound()[0];
  a_ = (bs - as) / (bi - ai);
  b_ = (as * bi - bs * ai) / (bi - ai);
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
    polynomialsCache_.add(OrthogonalUniVariatePolynomial(buildRecurrenceCoefficientsCollection(i), a_, b_));
  return polynomialsCache_[degree];
}



/* Build the 3 terms recurrence coefficients up to the needed degree */
Sample OrthogonalUniVariatePolynomialFactory::buildRecurrenceCoefficientsCollection(const UnsignedInteger degree) const
{
  Sample recurrenceCoefficients(degree, 3);
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
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    weights[i] = z(0, i) * z(0, i);
    d[i] = (d[i] - b_) / a_;
  }
  return d;
}

/* Affine coefficients accessors */
Scalar OrthogonalUniVariatePolynomialFactory::getA() const
{
  return a_;
}

void OrthogonalUniVariatePolynomialFactory::setA(const Scalar a)
{
  a_ = a;
}

Scalar OrthogonalUniVariatePolynomialFactory::getB() const
{
  return b_;
}

void OrthogonalUniVariatePolynomialFactory::setB(const Scalar b)
{
  b_ = b;
}


/* Method save() stores the object through the StorageManager */
void OrthogonalUniVariatePolynomialFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "measure_", measure_ );

  adv.saveAttribute("recurrenceCoefficientsCache_", recurrenceCoefficientsCache_);
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );  
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalUniVariatePolynomialFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "measure_", measure_ );
  // recurrenceCoefficientsCache_ changed type from PersistentCollection<Coefficients> to Sample in 1.19
  // without backward compatibility, see https://github.com/openturns/openturns/pull/1961
  if (adv.getStudyVersion() >= 102000)
    adv.loadAttribute("recurrenceCoefficientsCache_", recurrenceCoefficientsCache_);
  else
  {
    PersistentCollection<Coefficients> coefficientsColl;
    adv.loadAttribute("recurrenceCoefficientsCache_", coefficientsColl);
    const UnsignedInteger size = coefficientsColl.getSize();
    recurrenceCoefficientsCache_ = Sample(size, 3);
    for (UnsignedInteger i = 0; i < size; ++ i)
      for (UnsignedInteger j = 0; j < 3; ++j)
        recurrenceCoefficientsCache_(i, j) = coefficientsColl[i][j];
  }
  if (adv.hasAttribute("a_"))
    adv.loadAttribute( "a_", a_ );
  if (adv.hasAttribute("b_"))
    adv.loadAttribute( "b_", b_ );
}

END_NAMESPACE_OPENTURNS
