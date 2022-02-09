//                                               -*- C++ -*-
/**
 *  @brief This is a 1D polynomial
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OrthogonalUniVariatePolynomial.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariatePolynomial)

static const Factory<OrthogonalUniVariatePolynomial> Factory_OrthogonalUniVariatePolynomial;


/* Default constructor */
OrthogonalUniVariatePolynomial::OrthogonalUniVariatePolynomial()
  : UniVariatePolynomialImplementation()
  , recurrenceCoefficients_(0)
{
  coefficients_ = Coefficients(1, 1.0);
}


/* Constructor from recurrence coefficients */
OrthogonalUniVariatePolynomial::OrthogonalUniVariatePolynomial(const Sample & recurrenceCoefficients)
  : UniVariatePolynomialImplementation()
  , recurrenceCoefficients_(recurrenceCoefficients.getImplementation()->getData())
{
  // Build the coefficients using the recurrence coefficients
  coefficients_ = Coefficients(buildCoefficients(recurrenceCoefficients.getSize()));
}


/* Constructor from recurrence coefficients and coefficients */
OrthogonalUniVariatePolynomial::OrthogonalUniVariatePolynomial(const Sample & recurrenceCoefficients,
    const Coefficients & coefficients)
  : UniVariatePolynomialImplementation()
  , recurrenceCoefficients_(recurrenceCoefficients.getImplementation()->getData())
{
  // Set the value of the coefficients, stored in the upper class
  coefficients_ = coefficients;
}


/* Build the coefficients of the polynomial based on the recurrence coefficients */
OrthogonalUniVariatePolynomial::Coefficients OrthogonalUniVariatePolynomial::buildCoefficients(const UnsignedInteger n)
{
  // Constant polynomial equals to 1
  if (n == 0) return Coefficients(1, 1.0);
  // Other cases
  Coefficients coefficientsN(n + 1);
  Coefficients coefficientsNMinus1(buildCoefficients(n - 1));
  // Leading term
  UnsignedInteger index = 3 * n - 3;
  const Scalar aN0 = recurrenceCoefficients_[index];
  ++index;
  const Scalar aN1 = recurrenceCoefficients_[index];
  ++index;
  const Scalar aN2 = recurrenceCoefficients_[index];
  coefficientsN[n] = aN0 * coefficientsNMinus1[n - 1];
  // Constant term, case n = 1
  coefficientsN[0] = aN1 * coefficientsNMinus1[0];
  if (n == 1) return coefficientsN;
  // Constant term, case n >= 2
  Coefficients coefficientsNMinus2(buildCoefficients(n - 2));
  coefficientsN[0] += aN2 * coefficientsNMinus2[0];
  // Leading term
  coefficientsN[n] = aN0 * coefficientsNMinus1[n - 1];
  // Second leading term
  coefficientsN[n - 1] = aN0 * coefficientsNMinus1[n - 2] + aN1 * coefficientsNMinus1[n - 1];
  // Constant term
  coefficientsN[0] = aN1 * coefficientsNMinus1[0] + aN2 * coefficientsNMinus2[0];
  // Remaining terms
  for (UnsignedInteger i = 1; i < n - 1; ++i)
    coefficientsN[i] = aN0 * coefficientsNMinus1[i - 1] + aN1 * coefficientsNMinus1[i] + aN2 * coefficientsNMinus2[i];
  return coefficientsN;
}


/* Virtual constructor */
OrthogonalUniVariatePolynomial * OrthogonalUniVariatePolynomial::clone() const
{
  return new OrthogonalUniVariatePolynomial(*this);
}


/* OrthogonalUniVariatePolynomial are evaluated as functors */
Scalar OrthogonalUniVariatePolynomial::operator() (const Scalar x) const
{
  const UnsignedInteger size = recurrenceCoefficients_.getSize();
  Scalar uN = 1.0;
  // Special case: degree == 0, constant unitary polynomial
  if (size == 0) return uN;
  UnsignedInteger index = size - 1;
  const Scalar aN2 = recurrenceCoefficients_[index];
  --index;
  const Scalar aN1 = recurrenceCoefficients_[index];
  --index;
  const Scalar aN0 = recurrenceCoefficients_[index];
  Scalar uNMinus1 = aN0 * x + aN1;
  // Special case: degree == 1, affine polynomial
  if (size == 3) return uNMinus1;
  Scalar aN2uN = aN2 * uN;
  Scalar uNMinus2 = 0.0;
  // General case, use Clenshaw's algorithm for a stable evaluation of the polynomial
  // The summation must be done in reverse order to get the best stability
  // The three terms recurrence relation is:
  // Pn+1(x) = (a0[n] * x + a1[n]) * Pn(x) + a2[n] * Pn-1(x)
  // with P-1 = 0, P0 = 1
  Scalar aNMinus12, aNMinus11, aNMinus10;
  while (index > 0)
  {
    --index;
    aNMinus12 = recurrenceCoefficients_[index];
    --index;
    aNMinus11 = recurrenceCoefficients_[index];
    --index;
    aNMinus10 = recurrenceCoefficients_[index];
    uNMinus2 = (aNMinus10 * x + aNMinus11) * uNMinus1 + aN2uN;
    aN2uN = aNMinus12 * uNMinus1;
    uNMinus1 = uNMinus2;
  }
  return uNMinus2;
}


Sample OrthogonalUniVariatePolynomial::getRecurrenceCoefficients() const
{
  SampleImplementation result(recurrenceCoefficients_.getSize() / 3, 3);
  result.setData(recurrenceCoefficients_);
  return result;
}


/* Roots of the polynomial of degree n as the eigenvalues of the associated Jacobi matrix */
/* Jn = [alpha_0 sqrt(beta_1) 0 ...
   sqrt(beta_1) alpha_1 sqrt(beta_2) 0 ...
   0 sqrt(beta_2) alpha_2 sqrt(beta_3) 0 ...
   |
   0 ... 0 sqrt(beta_{n-1}) alpha_{n-1}] */
OrthogonalUniVariatePolynomial::ComplexCollection OrthogonalUniVariatePolynomial::getRoots() const
{
  const UnsignedInteger n = getDegree();
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: cannot compute the roots of a constant polynomial.";
  // gauss integration rule
  char jobz('N');
  int ljobz(1);
  Point d(n);
  Point e(n - 1);
  UnsignedInteger index = 0;
  Scalar a0 = recurrenceCoefficients_[index];
  ++index;
  Scalar a1 = recurrenceCoefficients_[index];
  ++index;
  Scalar a2;
  Scalar alphaPrec = a0;
  d[0] = -a1 / a0;
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    ++index;
    a0 = recurrenceCoefficients_[index];
    ++index;
    a1 = recurrenceCoefficients_[index];
    ++index;
    a2 = recurrenceCoefficients_[index];
    d[i]     = -a1 / a0;
    e[i - 1] = sqrt(-a2 / (a0 * alphaPrec));
    alphaPrec = a0;
  }
  int ldz(n);
  SquareMatrix z(n);
  Point work(2 * n - 2);
  int info;
  dstev_(&jobz, &ldz, &d[0], &e[0], &z(0, 0), &ldz, &work[0], &info, &ljobz);
  if (info != 0) throw InternalException(HERE) << "Lapack DSTEV: error code=" << info;
  ComplexCollection result(n);
  for (UnsignedInteger i = 0; i < n; ++i) result[i] = Complex(d[i], 0.0);
  return result;
}

/* Method save() stores the object through the StorageManager */
void OrthogonalUniVariatePolynomial::save(Advocate & adv) const
{
  UniVariatePolynomialImplementation::save(adv);
  adv.saveAttribute( "recurrenceCoefficients_", recurrenceCoefficients_ );
}

/* Method load() reloads the object from the StorageManager */
void OrthogonalUniVariatePolynomial::load(Advocate & adv)
{
  UniVariatePolynomialImplementation::load(adv);
  adv.loadAttribute( "recurrenceCoefficients_", recurrenceCoefficients_ );
}


END_NAMESPACE_OPENTURNS
