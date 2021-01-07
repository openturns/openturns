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
#include "openturns/UniVariatePolynomialImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/FFT.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniVariatePolynomialImplementation)

static const Factory<UniVariatePolynomialImplementation> Factory_UniVariatePolynomialImplementation;


/* Default constructor */
UniVariatePolynomialImplementation::UniVariatePolynomialImplementation()
  : UniVariateFunctionImplementation()
  , coefficients_(1, 0.0)
{
  // Nothing to do
}


/* Constructor from coefficients */
UniVariatePolynomialImplementation::UniVariatePolynomialImplementation(const Coefficients & coefficients)
  : UniVariateFunctionImplementation()
  , coefficients_(coefficients)
{
  compactCoefficients();
}



/* Virtual constructor */
UniVariatePolynomialImplementation * UniVariatePolynomialImplementation::clone() const
{
  return new UniVariatePolynomialImplementation(*this);
}


/* String converter */
String UniVariatePolynomialImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " coefficients=" << coefficients_.__repr__();
}

String UniVariatePolynomialImplementation::__str__(const String & offset) const
{
  return __str__("X", offset);
}

String UniVariatePolynomialImplementation::__str__(const String & variableName,
    const String & ) const
{
  OSS oss(false);
  const UnsignedInteger size = coefficients_.getSize();
  // Specific case for empty polynomial
  if (size == 0) return oss;
  Bool firstTerm = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar aI = coefficients_[i];
    // Only deal with non-zero coefficients
    if (String(OSS(false) << std::abs(aI)) != "0")
    {
      // Special case for the first term: no + sign, no leading blank and no trailing blank for the - sign
      if (firstTerm)
      {
        firstTerm = false;
        // Sign
        if (aI < 0) oss << "-";
        // If the leading term is a constant, print it even if its absolute value is 1
        if (i == 0) oss  << std::abs(aI);
        else
        {
          // Print the coefficient only if its absolute value is not 1
          if (String(OSS(false) << std::abs(aI)) != "1") oss << std::abs(aI) << " * ";
          oss << variableName;
          // Print the exponent only if it is > 1
          if (i > 1) oss << "^" << i;
        }
      } // Leading term
      // For the other coefficients
      else
      {
        // Separate the sign from the absolute value by a binay +/- operator
        // Here, i > 0
        if (aI > 0.0) oss << " + ";
        else oss << " - ";
        if (String(OSS(false) << std::abs(aI)) != "1") oss << std::abs(aI) << " * ";
        oss << variableName;
        // Print the exponent only if it is > 1
        if (i > 1) oss << "^" << i;
      } // Non-leading term
    } // Non-null coefficient
  } // Loop over the coefficients
  // Here, if firstTerm is true it is because all the coefficients are zero
  if (firstTerm) oss << "0";
  return oss;
}


/* UniVariatePolynomialImplementation are evaluated as functors */
Scalar UniVariatePolynomialImplementation::operator() (const Scalar x) const
{
  const UnsignedInteger size = coefficients_.getSize();
  Scalar y(coefficients_[size - 1]); /* y represents the value of P(x)*/
  // Evaluation using Horner scheme
  for (UnsignedInteger i = size - 1; i > 0; --i) y = y * x + coefficients_[i - 1];

  return y;
}

Complex UniVariatePolynomialImplementation::operator() (const Complex z) const
{
  const UnsignedInteger size = coefficients_.getSize();
  Complex y(coefficients_[size - 1]); /* y represents the value of P(x)*/
  // Evaluation using Horner scheme
  for (UnsignedInteger i = size - 1; i > 0; --i) y = y * z + coefficients_[i - 1];

  return y;
}


/* UniVariatePolynomialImplementation derivative */
Scalar UniVariatePolynomialImplementation::gradient(const Scalar x) const
{
  const UnsignedInteger size = coefficients_.getSize();
  if (size == 1) return 0.0;
  Scalar y((size - 1 ) * coefficients_[size - 1]); /* y represents the value of P'(x)*/
  // Evaluation using Horner scheme
  for (UnsignedInteger i = size - 1; i > 1; --i) y = y * x + (i - 1) * coefficients_[i - 1];

  return y;
}


Scalar UniVariatePolynomialImplementation::hessian(const Scalar x) const
{
  return derivate().gradient(x);
}

/* Compute the derivative of the polynomial */
UniVariatePolynomialImplementation UniVariatePolynomialImplementation::derivate() const
{
  const UnsignedInteger size = coefficients_.getSize();
  if (size <= 1) return UniVariatePolynomialImplementation();
  Coefficients derivativeCoefficients(size - 1);
  for (UnsignedInteger i = 0; i < size - 1; ++i)
    derivativeCoefficients[i] = coefficients_[i + 1] * (i + 1);
  return derivativeCoefficients;
}

/* Multiply the polynomial P by a Scalar */
UniVariatePolynomialImplementation UniVariatePolynomialImplementation::operator * (const Scalar scal) const
{
  if (scal == 0.0) return UniVariatePolynomialImplementation();
  return UniVariatePolynomialImplementation(coefficients_ * scal);
} // end method operator*

/* Multiply the polynomial P by an UniVariatePolynomialImplementation */
UniVariatePolynomialImplementation UniVariatePolynomialImplementation::operator * (const UniVariatePolynomialImplementation & uniVariatePolynomial) const
{
  // Special cases for constant polynomials
  const UnsignedInteger leftDegree = getDegree();
  if (leftDegree == 0) return uniVariatePolynomial.operator * (coefficients_[0]);
  Coefficients factorCoefficients(uniVariatePolynomial.getCoefficients());
  const UnsignedInteger rightDegree = uniVariatePolynomial.getDegree();
  if (rightDegree == 0) return operator*(factorCoefficients[0]);
  // General case
  const UnsignedInteger resultDimension = leftDegree + rightDegree + 1;
  Coefficients resultCoefficients(resultDimension);
  // For small total degree, use the elementary algorithm
  // We use a static local variable to avoid a systematic call to ResourceMap
  const UnsignedInteger smallDegree = ResourceMap::GetAsUnsignedInteger("UniVariatePolynomial-SmallDegree");
  if (resultDimension < smallDegree)
  {
    for (UnsignedInteger i = 0; i < resultDimension; ++i)
    {
      Scalar coefficientValue = 0.0;
      const UnsignedInteger jMin = i >= rightDegree ? (i - rightDegree) : 0;
      const UnsignedInteger jMax = i >= leftDegree ? leftDegree : i;
      for (UnsignedInteger j = jMin; j <= jMax; j++)
        coefficientValue += coefficients_[j] * factorCoefficients[i - j];
      resultCoefficients[i] = coefficientValue;
    }
  } // small degree
  else
  {
    const FFT fft;
    const UnsignedInteger powerOfTwo = SpecFunc::NextPowerOfTwo(resultDimension);
    ComplexCollection leftCoefficients(powerOfTwo);
    ComplexCollection rightCoefficients(powerOfTwo);
    for (UnsignedInteger i = 0; i <= leftDegree; ++i) leftCoefficients[i] = coefficients_[i];
    for (UnsignedInteger i = 0; i <= rightDegree; ++i) rightCoefficients[i] = factorCoefficients[i];
    leftCoefficients = fft.transform(leftCoefficients);
    rightCoefficients = fft.transform(rightCoefficients);
    for (UnsignedInteger i = 0; i < powerOfTwo; ++i) leftCoefficients[i] *= rightCoefficients[i];
    rightCoefficients = fft.inverseTransform(leftCoefficients);
    for (UnsignedInteger i = 0; i < resultDimension; ++i) resultCoefficients[i] = rightCoefficients[i].real();
  }
  return UniVariatePolynomialImplementation(resultCoefficients);
} // end method operator*

/* Multiply the polynomial by (x to the power deg) */
UniVariatePolynomialImplementation UniVariatePolynomialImplementation::incrementDegree(const UnsignedInteger deg) const
{
  // Special case for the null coefficient
  if ((getDegree() == 0) && (coefficients_[0] == 0.0)) return *this;
  const UnsignedInteger size = coefficients_.getSize();
  // The coefficients are initialized to 0.0
  Coefficients incrementedCoefficients(size + deg);
  // Just shift the coefficients by deg places
  for (UnsignedInteger j = 0; j < size; ++j) incrementedCoefficients[j + deg] = coefficients_[j];
  return UniVariatePolynomialImplementation(incrementedCoefficients);
}// end incrementDegree


/* Realize the summation of two polynomials of any degree ex:P=P1+P2 */
UniVariatePolynomialImplementation UniVariatePolynomialImplementation::operator + (const UniVariatePolynomialImplementation & uniVariatePolynomial) const
{
  Coefficients leftCoefficients(coefficients_);
  Coefficients rightCoefficients(uniVariatePolynomial.getCoefficients());
  const UnsignedInteger lhsSize = leftCoefficients.getSize();
  const UnsignedInteger rhsSize = rightCoefficients.getSize();
  // If the left hand side has a degree greater than the right hand side, add enough zeros to the coefficients in order to equal the degrees
  if (lhsSize > rhsSize) rightCoefficients.add(Point(lhsSize - rhsSize, 0.0));
  // Else the right hand side has a degree greater than the left hand side, add enough zeros to the coefficients in order to equal the degrees
  else leftCoefficients.add(Point(rhsSize - lhsSize, 0.0));
  // Then, we just have to sum-up the degrees
  UniVariatePolynomialImplementation sum(leftCoefficients + rightCoefficients);
  sum.compactCoefficients();
  return sum;
} // end summation of P1 & P2


/* Realize the subtraction of two polynomials of any degree ex:P=P1-P2 */
UniVariatePolynomialImplementation UniVariatePolynomialImplementation::operator - (const UniVariatePolynomialImplementation & uniVariatePolynomial) const
{
  return operator + (uniVariatePolynomial * (-1.0));
} // end subtraction of P1 & P2 */

/* Coefficients accessor */
void UniVariatePolynomialImplementation::setCoefficients(const Coefficients & coefficients)
{
  coefficients_ = coefficients;
  compactCoefficients();
}

UniVariatePolynomialImplementation::Coefficients UniVariatePolynomialImplementation::getCoefficients() const
{
  return coefficients_;
}


/* Get the degree of the polynomial */
UnsignedInteger UniVariatePolynomialImplementation::getDegree() const
{
  return coefficients_.getDimension() - 1;
}


/* Root of the polynomial of degree n as the eigenvalues of the associated  matrix */
UniVariatePolynomialImplementation::ComplexCollection UniVariatePolynomialImplementation::getRoots() const
{
  const UnsignedInteger degree = getDegree();
  if (degree == 0) throw NotDefinedException(HERE) << "Error: cannot compute the roots of a constant polynomial.";
  const Scalar scale = -1.0 / coefficients_[degree];
  SquareMatrix m(degree);
  m(0, degree - 1) = coefficients_[0] * scale;
  for (UnsignedInteger i = 1; i < degree; ++i)
  {
    m(i, i - 1) = 1.0;
    m(i, degree - 1) = coefficients_[i] * scale;
  }
  return m.computeEigenValues();
}

/* remove null leading coefficients. Special case for the null coefficient, which is constant so we don't remove this particular zero. */
void UniVariatePolynomialImplementation::compactCoefficients()
{
  UnsignedInteger degree = coefficients_.getDimension() - 1;
  while ((degree > 0) && (coefficients_[degree] == 0.0))
  {
    coefficients_.erase(degree);
    --degree;
  }
}

/* Method save() stores the object through the StorageManager */
void UniVariatePolynomialImplementation::save(Advocate & adv) const
{
  UniVariateFunctionImplementation::save(adv);
  adv.saveAttribute( "coefficients_", coefficients_ );
}

/* Method load() reloads the object from the StorageManager */
void UniVariatePolynomialImplementation::load(Advocate & adv)
{
  UniVariateFunctionImplementation::load(adv);
  adv.loadAttribute( "coefficients_", coefficients_ );
}


END_NAMESPACE_OPENTURNS
