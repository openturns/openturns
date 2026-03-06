//                                               -*- C++ -*-
/**
 *  @brief Jacobi polynomial factory
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
#include "openturns/JacobiFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Beta.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JacobiFactory)

static const Factory<JacobiFactory> Factory_JacobiFactory;

/* Default constructor: (1, 1) order Jacobi polynomial associated with the default Beta() = Beta(2, 2, -1, 1) distribution which is equal to the Epanechnikov distribution */
JacobiFactory::JacobiFactory()
  : OrthogonalUniVariatePolynomialFactory(Beta())
  , alpha_(2.0)
  , beta_(2.0)
{
  initializeCache();
}


/* Parameter constructor: (alpha, beta) are the standard Beta shape parameters */
JacobiFactory::JacobiFactory(const Scalar alpha,
                             const Scalar beta)
  : OrthogonalUniVariatePolynomialFactory(Beta(alpha, beta, -1.0, 1.0))
  , alpha_(alpha)
  , beta_(beta)
{
  initializeCache();
}


/* Constructor with arbitrary Beta shape parameters and bounds */
JacobiFactory::JacobiFactory(const Scalar alpha,
                             const Scalar beta,
                             const Scalar a,
                             const Scalar b)
  : OrthogonalUniVariatePolynomialFactory(Beta(alpha, beta, a, b))
  , alpha_(alpha)
  , beta_(beta)
{
  initializeCache();
}


/* Virtual constructor */
JacobiFactory * JacobiFactory::clone() const
{
  return new JacobiFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
JacobiFactory::Coefficients JacobiFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    const Scalar factor = 0.5 * sqrt((alpha_ + beta_ + 1.0) / (alpha_ * beta_));
    recurrenceCoefficients[0] = (alpha_ + beta_) * factor;
    // To avoid spurious -0.0
    if (alpha_ != beta_) recurrenceCoefficients[1] = (beta_ - alpha_) * factor;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  const Scalar nAlphaP1 = n + beta_;
  const Scalar nBetaP1 = n + alpha_;
  const Scalar twoNAlphaBetaP2 = nAlphaP1 + nBetaP1;
  const Scalar factor1 = (twoNAlphaBetaP2 + 1.0) / ((n + 1.0) * nAlphaP1 * nBetaP1 * (nAlphaP1 + alpha_ - 1.0));
  const Scalar factor2 = 0.5 * sqrt((twoNAlphaBetaP2 - 1.0) * factor1);
  recurrenceCoefficients[0] = factor2 * twoNAlphaBetaP2;
  // To avoid spurious -0.0
  if (alpha_ != beta_) recurrenceCoefficients[1] = factor2 * (beta_ - alpha_) * (alpha_ + beta_ - 2.0) / (twoNAlphaBetaP2 - 2.0);
  if (n == 1)
  {
    const Scalar epsilon = alpha_ + beta_ - 1.0;
    // The case where |epsilon| << 1 leads to an indeterminate form 0/0
    // when n==1, which is the only case where such a problem can occur.
    // We use a series expansion, the threshold 1.0e-8 insures that the
    // resulting error is less than machine precision
    // Here we know that alpha>-1, beta>-1 so alpha+beta+1=0 imposes beta<0
    if (std::abs(epsilon) < 1.0e-8)
    {
      recurrenceCoefficients[2] = (1.5 * epsilon / (alpha_ - 2.0) + (alpha_ - 1.0) * (3.0 - 3.125 * epsilon)) * sqrt(2.0 * alpha_ / ((alpha_ - 1.0) * (alpha_ - 2.0) * (alpha_ + 1.0)));
      return recurrenceCoefficients;
    }
  }
  recurrenceCoefficients[2] = -twoNAlphaBetaP2 / (twoNAlphaBetaP2 - 2.0) * sqrt((nAlphaP1 - 1.0) * (nBetaP1 - 1.0) * (nAlphaP1 + alpha_ - 2.0) * n * factor1 / (twoNAlphaBetaP2 - 3.0));
  return recurrenceCoefficients;
}

/* Alpha accessor */
Scalar JacobiFactory::getAlpha() const
{
  return alpha_;
}

/* Beta accessor */
Scalar JacobiFactory::getBeta() const
{
  return beta_;
}

/* String converter */
String JacobiFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " alpha=" << alpha_
         << " beta=" << beta_
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void JacobiFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}


/* Method load() reloads the object from the StorageManager */
void JacobiFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  // Starting from version 1.28 the stored values are the Beta distribution
  // shape parameters instead of the Jacobi polynomial orders.
  // Conversion: r = beta + 1, t = alpha + 1
  if (adv.getStudyVersion() < 102800)
  {
    const Scalar alphaOld = alpha_;
    alpha_ = beta_ + 1.0;
    beta_ = alphaOld + 1.0;
  }
}

END_NAMESPACE_OPENTURNS
