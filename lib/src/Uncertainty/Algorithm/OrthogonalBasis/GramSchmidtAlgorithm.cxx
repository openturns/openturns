//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
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

#include "GramSchmidtAlgorithm.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "SquareMatrix.hxx"
#include "Log.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GramSchmidtAlgorithm);

static const Factory<GramSchmidtAlgorithm> RegisteredFactory;

/* Default constructor */
GramSchmidtAlgorithm::GramSchmidtAlgorithm()
  : OrthonormalizationAlgorithmImplementation()
  , standardMoments_(1, 1.0)
  , coefficientsCache_(1, Coefficients(1, 1.0))
  , referenceFamily_()
  , useCanonicalBasis_(true)
{
  // Nothing to do
}


/* Parameter constructor */
GramSchmidtAlgorithm::GramSchmidtAlgorithm(const Distribution & measure)
  : OrthonormalizationAlgorithmImplementation(measure)
  , standardMoments_(1, 1.0)
  , coefficientsCache_(1, Coefficients(1, 1.0))
  , referenceFamily_()
  , useCanonicalBasis_(true)
{
  // Nothing to do
}


/* Parameter constructor */
GramSchmidtAlgorithm::GramSchmidtAlgorithm(const Distribution & measure,
    const OrthogonalUniVariatePolynomialFamily & referenceFamily)
  : OrthonormalizationAlgorithmImplementation(measure)
  , standardMoments_(1, 1.0)
  , coefficientsCache_(1, Coefficients(1, 1.0))
  , referenceFamily_(referenceFamily)
  , useCanonicalBasis_(false)
{
  // Nothing to do
}


/* Virtual constructor */
GramSchmidtAlgorithm * GramSchmidtAlgorithm::clone() const
{
  return new GramSchmidtAlgorithm(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
GramSchmidtAlgorithm::Coefficients GramSchmidtAlgorithm::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  // An orthonormal polynomial of degree n writes P(x) = k_n * x^n + l_n * x^{n-1} + ...
  // Degree n+1
  const Coefficients coefficientsNp1(buildPolynomial(n + 1).getCoefficients());
  const NumericalScalar kNp1(coefficientsNp1[n + 1]);
  const NumericalScalar lNp1(coefficientsNp1[n]);
  // Degree n
  const Coefficients coefficientsN(buildPolynomial(n).getCoefficients());
  const NumericalScalar kN(coefficientsN[n]);
  NumericalScalar lN(0.0);
  NumericalScalar kNm1(0.0);
  if (n > 0)
  {
    // Degree n-1, not used for n = 0
    lN = coefficientsN[n - 1];
    const Coefficients coefficientsNm1(buildPolynomial(n - 1).getCoefficients());
    kNm1 = coefficientsNm1[n - 1];
  }
  Coefficients result(3);
  if ((kN == 0.0) || (kNp1 == 0.0)) throw InternalException(HERE) << "Error: one of the coefficients n or n+1 is zero.";
  result[0] = kNp1 / kN;
  result[1] = (lNp1 - lN * kNp1 / kN) / kN;
  result[2] = -result[0] * kNm1 / kN;
  return result;
}


/* Return the order-th raw moment of the underlying measure */
NumericalScalar GramSchmidtAlgorithm::getStandardMoment(const UnsignedInteger order) const
{
  // We know that the raw moments will be accessed in a particular pattern: the moments not already
  // computed will always be accessed in a successive increasing order
  const UnsignedInteger maxOrder(standardMoments_.getSize());
  if (order > maxOrder) throw InvalidArgumentException(HERE) << "Error: cannot access to the raw moments in arbitrary order.";
  if (order == maxOrder)
    standardMoments_.add(measure_.getStandardMoment(order)[0]);
  return standardMoments_[order];
}

/* Build the kth orthonormal polynomial */
UniVariatePolynomial GramSchmidtAlgorithm::buildPolynomial(const UnsignedInteger k) const
{
  // If the needed polynomial is already in the cache
  if (k < coefficientsCache_.getSize()) return coefficientsCache_[k];
  if (k > coefficientsCache_.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot call buildPolynomial in arbitrary order.";
  UniVariatePolynomial v;
  if (useCanonicalBasis_)
  {
    // Build v = x^k
    Coefficients coefficients(k + 1, 0.0);
    coefficients[k] = 1.0;
    v = UniVariatePolynomial(coefficients);
  }
  else
  {
    v = referenceFamily_.build(k);
  }
  // Modified Gram-Schmidt algorithm
  UniVariatePolynomial q;
  for (UnsignedInteger i = 0; i < k; ++i)
  {
    const UniVariatePolynomial qi(buildPolynomial(i));
    v = v - qi * dotProduct(qi, v);
    const NumericalScalar norm2V = dotProduct(v, v);
    if (norm2V <= 0.0) throw InternalException(HERE) << "Error: the norm of the residual is zero.";
    q = v * (1.0 / sqrt(norm2V));
  }
  coefficientsCache_.add(q.getCoefficients());
  return q;
}


/* Compute the dot product between two general polynomials according to the measure */
NumericalScalar GramSchmidtAlgorithm::dotProduct(const UniVariatePolynomial & p1,
    const UniVariatePolynomial & p2) const
{
  const UniVariatePolynomial q(p1 * p2);
  const Coefficients ai(q.getCoefficients());
  const UnsignedInteger dimension(ai.getDimension());
#define MOMENT
#ifdef MOMENT
  NumericalScalar value(0.0);
  // Use the Kahan compensated summation to reduce roundoff errors
  NumericalScalar e(0.0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar temp(value);
    const NumericalScalar y(ai[i] * getStandardMoment(i) + e);
    value = temp + y;
    e = (temp - value) + y;
  }
  value += e;
  return value;
#else
  measure_.getImplementation()->setIntegrationNodesNumber(std::max(measure_.getImplementation()->getIntegrationNodesNumber(), std::min(ai.getSize(), 50)));
  NumericalPoint weights;
  NumericalPoint nodes(measure_.getImplementation()->getGaussNodesAndWeights(weights));
  const NumericalScalar lowerBound(measure_.getRange().getLowerBound()[0]);
  const NumericalScalar upperBound(measure_.getRange().getUpperBound()[0]);
  const NumericalScalar halfLength(0.5 * (upperBound - lowerBound));
  NumericalScalar newValue(0.0);
  NumericalScalar e = 0.0;
  for (UnsignedInteger i = 0; i < nodes.getSize(); ++i)
  {
    const NumericalScalar temp(newValue);
    const NumericalScalar xi(lowerBound + halfLength * (1.0 + nodes[i]));
    const NumericalScalar y(weights[i] * p1(xi) * p2(xi) * measure_.computePDF(xi));
    newValue = temp + y;
    e = (temp - newValue) + y;
  }
  newValue += e;
  newValue *= halfLength;
  return newValue;
#endif
}


/* Reference univariate orthogonal polynomial family accessor */
void GramSchmidtAlgorithm::setReferenceFamily(const OrthogonalUniVariatePolynomialFamily & family)
{
  (*this) = GramSchmidtAlgorithm(measure_, family);
}


OrthogonalUniVariatePolynomialFamily GramSchmidtAlgorithm::getReferenceFamily() const
{
  return referenceFamily_;
}

/* String converter */
String GramSchmidtAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void GramSchmidtAlgorithm::save(Advocate & adv) const
{
  OrthonormalizationAlgorithmImplementation::save(adv);
  adv.saveAttribute( "referenceFamily_", referenceFamily_ );
  adv.saveAttribute( "useCanonicalBasis_", useCanonicalBasis_ );
}


/* Method load() reloads the object from the StorageManager */
void GramSchmidtAlgorithm::load(Advocate & adv)
{
  OrthonormalizationAlgorithmImplementation::load(adv);
  adv.loadAttribute( "referenceFamily_", referenceFamily_ );
  // Initialize the object using the appropriate constructor
  *this = GramSchmidtAlgorithm(measure_, referenceFamily_);
  adv.loadAttribute( "useCanonicalBasis_", useCanonicalBasis_ );
}

END_NAMESPACE_OPENTURNS
