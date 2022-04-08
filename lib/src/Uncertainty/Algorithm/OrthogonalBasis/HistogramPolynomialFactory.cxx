//                                               -*- C++ -*-
/**
 *  @brief Histogram polynomial factory, specialization of the adaptive
 *         Stieltjes algorithm with highly accurate and efficient dot product
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
#include "openturns/HistogramPolynomialFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Histogram.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HistogramPolynomialFactory)

static const Factory<HistogramPolynomialFactory> Factory_HistogramPolynomialFactory;

/* Default constructor, associated with the default Histogram distribution */
HistogramPolynomialFactory::HistogramPolynomialFactory()
  : OrthogonalUniVariatePolynomialFactory(Histogram())
  , p_histogram_(dynamic_cast<const Histogram*>(&(*measure_.getImplementation())))
  , legendre_()
  , monicRecurrenceCoefficients_(1, Coefficients(3))
  , monicSquaredNorms_(1)
{
  // Here we initialize the monic coefficients cache
  monicRecurrenceCoefficients_[0][0] = 1.0;
  monicRecurrenceCoefficients_[0][1] = -measure_.getMean()[0];
  monicRecurrenceCoefficients_[0][2] = 0.0;
  monicSquaredNorms_[0] = 1.0;
  initializeCache();
}


/* Parameter constructor */
HistogramPolynomialFactory::HistogramPolynomialFactory(const Scalar first,
    const Point & width,
    const Point & height)
  : OrthogonalUniVariatePolynomialFactory(Histogram(first, width, height))
  , p_histogram_(dynamic_cast<const Histogram*>(&(*measure_.getImplementation())))
  , legendre_()
  , monicRecurrenceCoefficients_(1, Coefficients(3))
  , monicSquaredNorms_(1)
{
  // Here we initialize the monic coefficients cache
  monicRecurrenceCoefficients_[0][0] = 1.0;
  monicRecurrenceCoefficients_[0][1] = -measure_.getMean()[0];
  monicRecurrenceCoefficients_[0][2] = 0.0;
  monicSquaredNorms_[0] = 1.0;
  initializeCache();
}

/* Virtual constructor */
HistogramPolynomialFactory * HistogramPolynomialFactory::clone() const
{
  return new HistogramPolynomialFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x), P-1(x)=0, P0(x)=1
   We have:
   a0n = 1/sqrt(\beta_{n+1})
   a1n = -\alpha_n/sqrt(\beta_{n+1})
   a2n = -\sqrt{\beta_n/\beta_{n+1}}
   where \alpha_n and \beta_n are the recurrence coefficients of the monic orthogonal coefficients
   Qn+1(x) = (x - \alpha_n) * Qn(x) - beta_n * Qn-1(x), Q-1(x)=0, Q0(x)=1
   Rn = <Qn,Qn> for n >= 0
   \alpha_n = <xQn,Qn> / Rn
   \beta_n  = Rn / Rn-1 for n >= 1, \beta_0 = 0
   Remark: here we considere probability measures D, so R0=1, \alpha_0=E[D]=\mu, Q1(x)=x-\mu, \beta_1=R1=Var[D]
   Due to the convention of coefficients for orthonormal polynomials, the coefficients of the monic
   polynomials are stored as [1, -\alpha_n, -\beta_n]
 */
HistogramPolynomialFactory::Coefficients HistogramPolynomialFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  // The cache size is at least 1
  const UnsignedInteger cacheSize = monicRecurrenceCoefficients_.getSize();
  // Get the coefficients from the cache if possible
  if (n < cacheSize - 1)
  {
    const Scalar inverseSqrtBetaNp1 = 1.0 / sqrt(-monicRecurrenceCoefficients_[n + 1][2]);
    Coefficients coefficients(3);
    coefficients[0] = inverseSqrtBetaNp1;
    if (std::abs(monicRecurrenceCoefficients_[n][1]) > 0.0)
      coefficients[1] = monicRecurrenceCoefficients_[n][1] * inverseSqrtBetaNp1;
    coefficients[2] = -sqrt(-monicRecurrenceCoefficients_[n][2]) * inverseSqrtBetaNp1;
    return coefficients;
  }
  // This loop is to go to the first coefficients not in the cache. Here we cannot use cacheSize as
  // the size of the cache is increased by each call to getRecurrenceCoefficients()
  while (n >= monicRecurrenceCoefficients_.getSize()) getRecurrenceCoefficients(n - 1);
  // Here we know that n == cacheSize - 1. In order to compute the recurrence coefficients of the orthonormal polynomial pN
  // we need \alpha_{n-1}, \beta_{n-1} and \beta_n. The first two values are in the cache, we need \beta_n. We compute both
  // \alpha_n and \beta_n by numerical integration.
  Coefficients monicCoefficients(3);
  monicCoefficients[0] = 1.0;
  // Compute the dot products of the monic orthogonal polynomial of degree cacheSize
  // \beta_n = Rn / Rn-1 with Rn-1 = \beta_{n-1}Rn-2 = \beta_{n-1}\beta_{n-2}Rn-3 = ... = \prod_{k=0}^{n-1}\beta_k
  // Compute Rn and <x.Qn, Qn>
  const Point dotProduct(computeDotProduct(OrthogonalUniVariatePolynomial(monicRecurrenceCoefficients_)));
  monicSquaredNorms_.add(dotProduct[0]);
  monicCoefficients[1] = -dotProduct[1] / monicSquaredNorms_[n + 1];
  monicCoefficients[2] = -monicSquaredNorms_[n + 1] / monicSquaredNorms_[n];
  monicRecurrenceCoefficients_.add(monicCoefficients);
  // Now n == cacheSize - 2
  return getRecurrenceCoefficients(n);
}

/* Compute dot products taking into account the singularities of the weights */
Point HistogramPolynomialFactory::computeDotProduct(const OrthogonalUniVariatePolynomial & qN) const
{
  // Here we have integrals of qN(x)^2 and x*qN(x)^2, so a polynomial of maximal degree 2N+1, we need a N+1 Legendre quadrature rule
  const UnsignedInteger n = qN.getDegree();
  Point weights;
  const Point nodes(legendre_.getNodesAndWeights(n + 1, weights));
  const Scalar first = p_histogram_->getFirst();
  const Point width(p_histogram_->getWidth());
  const Point height(p_histogram_->getHeight());
  Scalar a = first;
  Point dotProduct(2);
  for (UnsignedInteger i = 0; i < width.getSize(); ++i)
  {
    const Scalar ab = width[i];
    const Scalar b = a + ab;
    const Scalar c = a + b;
    Point value(2);
    // Here we integrate qN^2 and xqN^2 over [a,b]
    for (UnsignedInteger j = 0; j <= n; ++j)
    {
      const Scalar x = 0.5 * (c + nodes[j] * ab);
      const Scalar qNX = qN(x);
      const Scalar qNX2 = qNX * qNX;
      value[0] += weights[j] * qNX2;
      value[1] += weights[j] * x * qNX2;
    }
    dotProduct += (height[i] * ab) * value;
    a = b;
  }
  return dotProduct;
}

/* First accessor */
Scalar HistogramPolynomialFactory::getFirst() const
{
  return p_histogram_->getFirst();
}

/* Width accessor */
Point HistogramPolynomialFactory::getWidth() const
{
  return p_histogram_->getWidth();
}

/* Height accessor */
Point HistogramPolynomialFactory::getHeight() const
{
  return p_histogram_->getHeight();
}

/* String converter */
String HistogramPolynomialFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_
         << " legendre=" << legendre_;
}


/* Method save() stores the object through the StorageManager */
void HistogramPolynomialFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "legendre_", legendre_ );
}


/* Method load() reloads the object from the StorageManager */
void HistogramPolynomialFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "legendre_", legendre_ );
  p_histogram_ = dynamic_cast<const Histogram*>(&(*measure_.getImplementation()));
}

END_NAMESPACE_OPENTURNS

