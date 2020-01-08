//                                               -*- C++ -*-
/**
 *  @brief Meixner polynomial factory
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MeixnerFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NegativeBinomial.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(MeixnerFactory)

static const Factory<MeixnerFactory> Factory_MeixnerFactory;



/* Default constructor, associated with the default Negative Binomial distribution of parameter 1, 1/2. */
MeixnerFactory::MeixnerFactory()
  : OrthogonalUniVariatePolynomialFactory(NegativeBinomial()),
    r_(1.0),
    p_(0.5)
{
  initializeCache();
}


/* Parameter constructor: lambda is the order of the generalized Meixner polynomial, associated with the NegativeBinomial(r, p) distribution */
MeixnerFactory::MeixnerFactory(const Scalar r,
                               const Scalar p)
  : OrthogonalUniVariatePolynomialFactory( NegativeBinomial(r, p) ),
    r_(r),
    p_(p)
{
  if (!(r > 0.0)) throw InvalidArgumentException(HERE) << "Error: must have r>0 to build Meixner polynomials.";
  if ((p <= 0.0) || (p >= 1.0)) throw InvalidArgumentException(HERE) << "Error: p must be in [0, 1] to build Meixner polynomials.";
  initializeCache();
}

/* Virtual constructor */
MeixnerFactory * MeixnerFactory::clone() const
{
  return new MeixnerFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
MeixnerFactory::Coefficients MeixnerFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    const Scalar factor = sqrt(r_ * p_);
    recurrenceCoefficients[0] = (p_ - 1.0) / factor;
    recurrenceCoefficients[1] = factor;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  const Scalar denominator = sqrt(p_ * (n + 1) * (n + r_));
  recurrenceCoefficients[0] = (p_ - 1.0) / denominator;
  recurrenceCoefficients[1] = (p_ * (n + r_) + n) / denominator;
  recurrenceCoefficients[2] = -sqrt(p_ * n * (n + r_ - 1.0)) / denominator;
  return recurrenceCoefficients;
}

/* R accessor */
Scalar MeixnerFactory::getR() const
{
  return r_;
}

/* P accessor */
Scalar MeixnerFactory::getP() const
{
  return p_;
}

/* String converter */
String MeixnerFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " r=" << r_
         << " p=" << p_
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void MeixnerFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "r_", r_ );
  adv.saveAttribute( "p_", p_ );
}


/* Method load() reloads the object from the StorageManager */
void MeixnerFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "r_", r_ );
  adv.loadAttribute( "p_", p_ );
}

END_NAMESPACE_OPENTURNS
