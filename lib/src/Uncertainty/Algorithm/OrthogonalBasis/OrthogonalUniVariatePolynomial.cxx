//                                               -*- C++ -*-
/**
 *  @brief This is a 1D polynomial
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
#include "openturns/OrthogonalUniVariatePolynomial.hxx"
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
  coefficients_ = buildCoefficients();
}


/* Constructor from recurrence coefficients */
OrthogonalUniVariatePolynomial::OrthogonalUniVariatePolynomial(const Sample & recurrenceCoefficients,
							       const Scalar a,
							       const Scalar b)
  : UniVariatePolynomialImplementation()
  , recurrenceCoefficients_(recurrenceCoefficients.getImplementation()->getData())
  , a_(a)
  , b_(b)
{
  coefficients_ = buildCoefficients();
}


/* Virtual constructor */
OrthogonalUniVariatePolynomial * OrthogonalUniVariatePolynomial::clone() const
{
  return new OrthogonalUniVariatePolynomial(*this);
}


/* Build the monomial coefficients from the recurrence coefficients */
OrthogonalUniVariatePolynomial::Coefficients OrthogonalUniVariatePolynomial::buildCoefficients() const
{
  const UnsignedInteger n = recurrenceCoefficients_.getSize() / 3;
  // Constant polynomial
  if (n == 0) return Coefficients(1, 1.0);
  // P1(z) = a0[0] * z + a1[0], with z = a_ * x + b_
  Coefficients coeffs(2);
  coeffs[0] = recurrenceCoefficients_[0] * b_ + recurrenceCoefficients_[1];
  coeffs[1] = recurrenceCoefficients_[0] * a_;
  if (n == 1) return coeffs;
  // Build iteratively for higher degrees using the recurrence
  Coefficients coeffsMinus1(1, 1.0);
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    const UnsignedInteger offset = 3 * i;
    const Scalar a0 = recurrenceCoefficients_[offset];
    const Scalar a1 = recurrenceCoefficients_[offset + 1];
    const Scalar a2 = recurrenceCoefficients_[offset + 2];
    // Q_{i+1}(x) = a0 * (a * x + b) * Q_i(x) + a1 * Q_i(x) + a2 * Q_{i-1}(x)
    const UnsignedInteger degree = i + 1;
    Coefficients coeffsNext(degree + 1, 0.0);
    const Scalar alpha = a0 * a_;
    for (UnsignedInteger j = 1; j <= degree; ++j)
      coeffsNext[j] = alpha * coeffs[j - 1];
    const Scalar beta = a0 * b_ + a1;
    for (UnsignedInteger j = 0; j <= i; ++j)
      coeffsNext[j] += beta * coeffs[j];
    for (UnsignedInteger j = 0; j <= i - 1; ++j)
      coeffsNext[j] += a2 * coeffsMinus1[j];
    coeffsMinus1 = coeffs;
    coeffs = coeffsNext;
  }
  return coeffs;
}


/* OrthogonalUniVariatePolynomial are evaluated as functors */
Scalar OrthogonalUniVariatePolynomial::operator() (const Scalar x) const
{
  const UnsignedInteger size = recurrenceCoefficients_.getSize();
  Scalar uN = 1.0;
  // Special case: degree == 0, constant unitary polynomial
  if (size == 0) return uN;
  // Apply the affine transformation
  const Scalar z = a_ * x + b_;
  UnsignedInteger index = size - 1;
  const Scalar aN2 = recurrenceCoefficients_[index];
  --index;
  const Scalar aN1 = recurrenceCoefficients_[index];
  --index;
  const Scalar aN0 = recurrenceCoefficients_[index];
  Scalar uNMinus1 = aN0 * z + aN1;
  // Special case: degree == 1, affine polynomial
  if (size == 3) return uNMinus1;
  Scalar aN2uN = aN2 * uN;
  Scalar uNMinus2 = 0.0;
  // General case, use Clenshaw's algorithm for a stable evaluation of the polynomial
  // The summation must be done in reverse order to get the best stability
  // The three terms recurrence relation is:
  // Pn+1(z) = (a0[n] * z + a1[n]) * Pn(z) + a2[n] * Pn-1(z)
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
    uNMinus2 = (aNMinus10 * z + aNMinus11) * uNMinus1 + aN2uN;
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
  for (UnsignedInteger i = 0; i < n; ++i) result[i] = (Complex(d[i], 0.0) - b_) / a_;
  return result;
}

/* Affine coefficients accessors */
Scalar OrthogonalUniVariatePolynomial::getA() const
{
  return a_;
}

void OrthogonalUniVariatePolynomial::setA(const Scalar a)
{
  if (a == 0.0) throw InvalidArgumentException(HERE) << "Error: the affine coefficient a=0 should be nonzero.";
  if (a != a_)
  {
    a_ = a;
    coefficients_ = buildCoefficients();
  }
}
  
Scalar OrthogonalUniVariatePolynomial::getB() const
{
  return b_;
}

void OrthogonalUniVariatePolynomial::setB(const Scalar b)
{
  if (b != b_)
  {
    b_ = b;
    coefficients_ = buildCoefficients();
  }
}

/* Method save() stores the object through the StorageManager */
void OrthogonalUniVariatePolynomial::save(Advocate & adv) const
{
  UniVariatePolynomialImplementation::save(adv);
  adv.saveAttribute( "recurrenceCoefficients_", recurrenceCoefficients_ );
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
}

/* Method load() reloads the object from the StorageManager */
void OrthogonalUniVariatePolynomial::load(Advocate & adv)
{
  UniVariatePolynomialImplementation::load(adv);
  // recurrenceCoefficients_ changed type from PersistentCollection<Coefficients> to PersistentCollection<Scalar> in 1.19
  // without backward compatibility, see https://github.com/openturns/openturns/pull/1961
  if (adv.getStudyVersion() >= 102000)
    adv.loadAttribute("recurrenceCoefficients_", recurrenceCoefficients_);
  else
  {
    PersistentCollection<Coefficients> coefficientsColl;
    adv.loadAttribute("recurrenceCoefficients_", coefficientsColl);
    const UnsignedInteger size = coefficientsColl.getSize();
    recurrenceCoefficients_ = PersistentCollection<Scalar>(3 * size);
    for (UnsignedInteger i = 0; i < size; ++ i)
      for (UnsignedInteger j = 0; j < 3; ++j)
        recurrenceCoefficients_[3 * i + j] = coefficientsColl[i][j];
  }
  // Affine transformation is introduced in version 1.17
  if (adv.hasAttribute("a_")) adv.loadAttribute("a_", a_);
  if (adv.hasAttribute("b_")) adv.loadAttribute("b_", b_);
}


END_NAMESPACE_OPENTURNS
