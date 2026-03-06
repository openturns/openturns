//                                               -*- C++ -*-
/**
 *  @brief Laguerre polynomial factory
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
#include "openturns/LaguerreFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LaguerreFactory)

static const Factory<LaguerreFactory> Factory_LaguerreFactory;



/* Default constructor, associated with the default Gamma distribution which is equal to the Exponential distribution */
LaguerreFactory::LaguerreFactory()
  : OrthogonalUniVariatePolynomialFactory(Gamma())
  , k_(1.0)
{
  initializeCache();
}


/* Parameter constructor: k is the shape parameter of the Gamma distribution */
LaguerreFactory::LaguerreFactory(const Scalar k)
  : OrthogonalUniVariatePolynomialFactory(Gamma(k, 1.0, 0.0))
  , k_(k)
{
  initializeCache();
}


/* Constructor with arbitrary Gamma parameters */
LaguerreFactory::LaguerreFactory(const Scalar k,
                                 const Scalar lambda,
                                 const Scalar gamma)
  : OrthogonalUniVariatePolynomialFactory(Gamma(k, lambda, gamma))
  , k_(k)
{
  setA(1.0 / lambda);
  setB(gamma);
  initializeCache();
}


/* Virtual constructor */
LaguerreFactory * LaguerreFactory::clone() const
{
  return new LaguerreFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
LaguerreFactory::Coefficients LaguerreFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    const Scalar factor = sqrt(k_);
    recurrenceCoefficients[0] = 1.0 / factor;
    recurrenceCoefficients[1] = -factor;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  const Scalar factor = 1.0 / sqrt((n + 1.0) * (n + k_));
  recurrenceCoefficients[0] = factor;
  recurrenceCoefficients[1] = -(2.0 * n + k_) * factor;
  recurrenceCoefficients[2] = -sqrt((n + k_ - 1.0) * n) * factor;
  return recurrenceCoefficients;
}

/* K accessor */
Scalar LaguerreFactory::getK() const
{
  return k_;
}

/* String converter */
String LaguerreFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " k=" << k_
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void LaguerreFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "k_", k_ );
}


/* Method load() reloads the object from the StorageManager */
void LaguerreFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "k_", k_ );
  // Starting from version 1.28 the stored value is the Gamma distribution
  // shape parameter instead of the Laguerre polynomial order.
  // Conversion: k = alphaOrder + 1
  if (adv.getStudyVersion() < 102800)
  {
    k_ += 1.0;
  }
}

END_NAMESPACE_OPENTURNS
