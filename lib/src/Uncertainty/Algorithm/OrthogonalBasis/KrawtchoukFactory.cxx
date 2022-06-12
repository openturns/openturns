//                                               -*- C++ -*-
/**
 *  @brief Krawtchouk polynomial factory
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
#include "openturns/KrawtchoukFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(KrawtchoukFactory)

static const Factory<KrawtchoukFactory> Factory_KrawtchoukFactory;



/* Default constructor: (1, 0.5) order Krawtchouk polynomial associated with the default Binomial() = Binomial(1, 0.5) distribution which is equal to the Bernoulli(0.5) distribution */
KrawtchoukFactory::KrawtchoukFactory()
  : OrthogonalUniVariatePolynomialFactory(Binomial()),
    n_(1),
    p_(0.5)
{
  initializeCache();
}


/* Parameter constructor: (n, p) is the order of the Krawtchouk polynomial, associated with the Binomial(n, p) distribution */
KrawtchoukFactory::KrawtchoukFactory(const UnsignedInteger n,
                                     const Scalar p)
  : OrthogonalUniVariatePolynomialFactory(Binomial(n, p)),
    n_(n),
    p_(p)
{
  if ((n == 0) || (p <= 0.0) || (p >= 1.0)) throw InvalidArgumentException(HERE) << "Error: must have N>0 and p in ]0, 1[ to build Krawtchouk polynomials.";
  initializeCache();
}

/* Virtual constructor */
KrawtchoukFactory * KrawtchoukFactory::clone() const
{
  return new KrawtchoukFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
KrawtchoukFactory::Coefficients KrawtchoukFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  if (!(n < n_)) throw InvalidArgumentException(HERE) << "Error: cannot build a Krawtchouk polynomial of index greater than its first parameter n. Here, n=" << n << " and you are trying to build the " << n + 1 << "th polynomial.";
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    const Scalar factor1 = sqrt(n_ * p_);
    const Scalar factor2 = 1.0 / sqrt(1.0 - p_);
    recurrenceCoefficients[0] =  factor2 / factor1;
    recurrenceCoefficients[1] = -factor2 * factor1;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  const Scalar factor = 1.0 / sqrt((n + 1) * (n_ - n) * p_ * (1.0 - p_));
  recurrenceCoefficients[0] = factor;
  recurrenceCoefficients[1] = -(p_ * (n_ - n) + n * (1.0 - p_)) * factor;
  recurrenceCoefficients[2] = -sqrt((1.0 - 1.0 / (n + 1.0)) * (1.0 + 1.0 / (n_ - n)));
  return recurrenceCoefficients;
}

/* N accessor */
UnsignedInteger KrawtchoukFactory::getN() const
{
  return n_;
}

/* P accessor */
Scalar KrawtchoukFactory::getP() const
{
  return p_;
}

/* String converter */
String KrawtchoukFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " N=" << n_
         << " p=" << p_
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void KrawtchoukFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "p_", p_ );
}


/* Method load() reloads the object from the StorageManager */
void KrawtchoukFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "p_", p_ );
}

END_NAMESPACE_OPENTURNS
