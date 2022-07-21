//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/OrthonormalizationAlgorithm.hxx"
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthonormalizationAlgorithm
 */

CLASSNAMEINIT(OrthonormalizationAlgorithm)

/* Constructor with parameters */
OrthonormalizationAlgorithm::OrthonormalizationAlgorithm(const OrthonormalizationAlgorithmImplementation & implementation)
  : TypedInterfaceObject<OrthonormalizationAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor with parameters */
OrthonormalizationAlgorithm::OrthonormalizationAlgorithm(Implementation & p_implementation)
  : TypedInterfaceObject<OrthonormalizationAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with parameters */
OrthonormalizationAlgorithm::OrthonormalizationAlgorithm(const Distribution & measure)
  : TypedInterfaceObject<OrthonormalizationAlgorithmImplementation>(new AdaptiveStieltjesAlgorithm(measure))
{
  // Nothing to do
}

/* Constructor with parameters */
OrthonormalizationAlgorithm::OrthonormalizationAlgorithm()
  : TypedInterfaceObject<OrthonormalizationAlgorithmImplementation>(new OrthonormalizationAlgorithmImplementation())
{
  // Nothing to do
}

/* Calculate the coefficients of recurrence a0, a1, a2 such that
   Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
OrthonormalizationAlgorithm::Coefficients OrthonormalizationAlgorithm::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  return getImplementation()->getRecurrenceCoefficients(n);
}

/* Measure accessor */
Distribution OrthonormalizationAlgorithm::getMeasure() const
{
  return getImplementation()->getMeasure();
}

void OrthonormalizationAlgorithm::setMeasure(const Distribution & measure)
{
  copyOnWrite();
  getImplementation()->setMeasure(measure);
}

/* String converter */
String OrthonormalizationAlgorithm::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String OrthonormalizationAlgorithm::__str__(const String & ) const
{
  return __repr__();
}

END_NAMESPACE_OPENTURNS
