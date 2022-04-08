//                                               -*- C++ -*-
/**
 *  @brief This is a 1D polynomial
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
#include "openturns/UniVariatePolynomial.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(UniVariatePolynomial)

/* Default constructor */
UniVariatePolynomial::UniVariatePolynomial()
  : TypedInterfaceObject<UniVariatePolynomialImplementation>(new UniVariatePolynomialImplementation())
{
  // Nothing to do
}


/* Constructor from coefficients */
UniVariatePolynomial::UniVariatePolynomial(const Coefficients & coefficients)
  : TypedInterfaceObject<UniVariatePolynomialImplementation>(new UniVariatePolynomialImplementation(coefficients))
{
  // Nothing to do
}


/* Constructor from implementation */
UniVariatePolynomial::UniVariatePolynomial(const UniVariatePolynomialImplementation & implementation)
  : TypedInterfaceObject<UniVariatePolynomialImplementation>(implementation.clone())
{
  // Nothing to do
}


/* Constructor from implementation pointer */
UniVariatePolynomial::UniVariatePolynomial(const Implementation & p_implementation)
  : TypedInterfaceObject<UniVariatePolynomialImplementation>(p_implementation)
{
  // Nothing to do
}


/* Constructor from implementation pointer */
UniVariatePolynomial::UniVariatePolynomial(UniVariatePolynomialImplementation * p_implementation)
  : TypedInterfaceObject<UniVariatePolynomialImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String UniVariatePolynomial::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " coefficients=" << getImplementation()->getCoefficients().__repr__();
}

String UniVariatePolynomial::__str__(const String & offset) const
{
  return OSS(false) << getImplementation()->__str__("X", offset);
}

String UniVariatePolynomial::__str__(const String & variableName,
                                     const String & offset) const
{
  return OSS() << getImplementation()->__str__(variableName, offset);
}


/* UniVariatePolynomial are evaluated as functors */
Scalar UniVariatePolynomial::operator() (const Scalar x) const
{
  return getImplementation()->operator()(x);
}

Complex UniVariatePolynomial::operator() (const Complex z) const
{
  return getImplementation()->operator()(z);
}


/* UniVariatePolynomialImplementation derivative */
Scalar UniVariatePolynomial::gradient(const Scalar x) const
{
  return getImplementation()->gradient(x);
}


Scalar UniVariatePolynomial::hessian(const Scalar x) const
{
  return getImplementation()->hessian(x);
}


/* Compute the derivative of the polynomial */
UniVariatePolynomial UniVariatePolynomial::derivate() const
{
  return getImplementation()->derivate();
}

/* Multiply the polynomial P by a Scalar */
UniVariatePolynomial UniVariatePolynomial::operator* (const Scalar scal) const
{
  return getImplementation()->operator*(scal);
} // end method operator*

/* Multiply the polynomial P by an UniVariatePolynomial */
UniVariatePolynomial UniVariatePolynomial::operator* (const UniVariatePolynomial & uniVariatePolynomial) const
{
  return getImplementation()->operator*(*(uniVariatePolynomial.getImplementation()));
} // end method operator*

/* Multiply the polynomial by (x to the power deg) */
UniVariatePolynomial UniVariatePolynomial::incrementDegree(const UnsignedInteger deg) const
{
  return getImplementation()->incrementDegree(deg);
}// end incrementDegree


/* Realize the summation of two polynomials of any degree ex:P=P1+P2 */
UniVariatePolynomial UniVariatePolynomial::operator + (const UniVariatePolynomial & uniVariatePolynomial) const
{
  return getImplementation()->operator+(*(uniVariatePolynomial.getImplementation()));
} // end summation of P1 & P2


/* Realize the subtraction of two polynomials of any degree ex:P=P1-P2 */
UniVariatePolynomial UniVariatePolynomial::operator - (const UniVariatePolynomial & uniVariatePolynomial) const
{
  return getImplementation()->operator-(*(uniVariatePolynomial.getImplementation()));
} // end subtraction of P1 & P2 */

/* Coefficients accessor */
void UniVariatePolynomial::setCoefficients(const Coefficients & coefficients)
{
  copyOnWrite();
  getImplementation()->setCoefficients(coefficients);
}

UniVariatePolynomial::Coefficients UniVariatePolynomial::getCoefficients() const
{
  return getImplementation()->getCoefficients();
}


/* Method to draw the graph of the polynomial between given bounds */
Graph UniVariatePolynomial::draw(const Scalar xMin,
                                 const Scalar xMax,
                                 const UnsignedInteger pointNumber) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber);
}


/* Get the degree of the polynomial */
UnsignedInteger UniVariatePolynomial::getDegree() const
{
  return getImplementation()->getDegree();
}


/* Root of the polynomial */
UniVariatePolynomial::ComplexCollection UniVariatePolynomial::getRoots() const
{
  return getImplementation()->getRoots();
}

END_NAMESPACE_OPENTURNS
