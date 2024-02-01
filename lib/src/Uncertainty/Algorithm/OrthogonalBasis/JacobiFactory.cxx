//                                               -*- C++ -*-
/**
 *  @brief Jacobi polynomial factory
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

/* Default constructor: (1, 1) order Jacobi polynomial associated with the default Beta() = Beta(2, 4, -1, 1) distribution which is equal to the Epanechnikov distribution */
JacobiFactory::JacobiFactory()
  : OrthogonalUniVariatePolynomialFactory(Beta())
  , alpha_(1.0)
  , beta_(1.0)
{
  initializeCache();
}


/* Parameter constructor: (alpha, beta) is the order of the Jacobi polynomial, associated with the Beta(beta + 1, alpha + beta + 2, -1, 1) distribution in the ANALYSIS parameter set or to the Beta(alpha, beta, -1, 1) distribution in the PROBABILITY parameter set */
JacobiFactory::JacobiFactory(const Scalar alpha,
                             const Scalar beta,
                             const ParameterSet parameterization)
  : OrthogonalUniVariatePolynomialFactory((parameterization == ANALYSIS ? Beta(beta + 1.0, alpha + 1.0, -1.0, 1.0) : Beta(alpha, beta - alpha, -1.0, 1.0)))
  , alpha_(0.0)
  , beta_(0.0)
{
  if (parameterization == ANALYSIS)
  {
    alpha_ = alpha;
    beta_ = beta;
  }
  else
  {
    alpha_ = beta - alpha - 1.0;
    beta_ = alpha - 1.0;
  }
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
    const Scalar factor = 0.5 * sqrt((alpha_ + beta_ + 3.0) / ((alpha_ + 1.0) * (beta_ + 1.0)));
    recurrenceCoefficients[0] = (alpha_ + beta_ + 2.0) * factor;
    // To avoid spurious -0.0
    if (alpha_ != beta_) recurrenceCoefficients[1] = (alpha_ - beta_) * factor;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  const Scalar nAlphaP1 = n + alpha_ + 1;
  const Scalar nBetaP1 = n + beta_ + 1;
  const Scalar twoNAlphaBetaP2 = nAlphaP1 + nBetaP1;
  const Scalar factor1 = (twoNAlphaBetaP2 + 1) / ((n + 1) * nAlphaP1 * nBetaP1 * (nAlphaP1 + beta_));
  const Scalar factor2 = 0.5 * sqrt((twoNAlphaBetaP2 - 1) * factor1);
  recurrenceCoefficients[0] = factor2 * twoNAlphaBetaP2;
  // To avoid spurious -0.0
  if (alpha_ != beta_) recurrenceCoefficients[1] = factor2 * (alpha_ - beta_) * (alpha_ + beta_) / (twoNAlphaBetaP2 - 2);
  if (n == 1)
  {
    const Scalar epsilon = alpha_ + beta_ + 1.0;
    // The case where |epsilon| << 1 leads to an indeterminate form 0/0
    // when n==1, which is the only case where such a problem can occur.
    // We use a series expansion, the threshold 1.0e-8 insures that the
    // resulting error is less than machine precision
    // Here we know that alpha>-1, beta>-1 so alpha+beta+1=0 imposes beta<0
    if (std::abs(epsilon) < 1.0e-8)
    {
      recurrenceCoefficients[2] = (1.5 * epsilon / (-1.0 + beta_) + beta_ * (3.0 - 3.125 * epsilon)) * sqrt(2.0 * (1.0 + beta_) / (beta_ * (-1.0 + beta_) * (2.0 + beta_)));
      return recurrenceCoefficients;
    }
  }
  recurrenceCoefficients[2] = -twoNAlphaBetaP2 / (twoNAlphaBetaP2 - 2) * sqrt((nAlphaP1 - 1) * (nBetaP1 - 1) * (nAlphaP1 + beta_ - 1) * n * factor1 / (twoNAlphaBetaP2 - 3));
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
}

END_NAMESPACE_OPENTURNS
