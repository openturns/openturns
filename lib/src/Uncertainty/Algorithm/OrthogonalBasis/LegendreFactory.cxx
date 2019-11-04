//                                               -*- C++ -*-
/**
 *  @brief Legendre polynomial factory
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LegendreFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LegendreFactory)

static const Factory<LegendreFactory> Factory_LegendreFactory;



/* Default constructor */
LegendreFactory::LegendreFactory()
  : OrthogonalUniVariatePolynomialFactory(Uniform())
{
  initializeCache();
}


/* Virtual constructor */
LegendreFactory * LegendreFactory::clone() const
{
  return new LegendreFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
LegendreFactory::Coefficients LegendreFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    recurrenceCoefficients[0] = sqrt(3.0);
    recurrenceCoefficients[1] = 0.0;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  const Scalar factor = sqrt(2.0 * n + 3.0) / (n + 1.0);
  recurrenceCoefficients[0] = sqrt(2.0 * n + 1.0) * factor;
  recurrenceCoefficients[1] = 0.0;
  recurrenceCoefficients[2] = -factor * n / sqrt(2.0 * n - 1.0);
  return recurrenceCoefficients;
}


/* String converter */
String LegendreFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void LegendreFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void LegendreFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
}

END_NAMESPACE_OPENTURNS
