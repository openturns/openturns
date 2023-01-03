//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OrthonormalizationAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthonormalizationAlgorithmImplementation
 */

CLASSNAMEINIT(OrthonormalizationAlgorithmImplementation)

static const Factory<OrthonormalizationAlgorithmImplementation> Factory_OrthonormalizationAlgorithmImplementation;


/* Default constructor */
OrthonormalizationAlgorithmImplementation::OrthonormalizationAlgorithmImplementation()
  : PersistentObject(),
    measure_(Uniform())
{
  // Nothing to do
}

/* Parameters constructor */
OrthonormalizationAlgorithmImplementation::OrthonormalizationAlgorithmImplementation(const Distribution & measure)
  : PersistentObject(),
    measure_(measure)
{
  // Nothing to do
}

/* Virtual constructor */
OrthonormalizationAlgorithmImplementation * OrthonormalizationAlgorithmImplementation::clone() const
{
  return new OrthonormalizationAlgorithmImplementation(*this);
}

/* Calculate the coefficients of recurrence a0, a1, a2 such that
   Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
OrthonormalizationAlgorithmImplementation::Coefficients OrthonormalizationAlgorithmImplementation::getRecurrenceCoefficients(const UnsignedInteger ) const
{
  throw NotYetImplementedException(HERE) << "In OrthonormalizationAlgorithmImplementation::getRecurrenceCoefficients(const UnsignedInteger n) const";
}


/* Measure accessor */
void OrthonormalizationAlgorithmImplementation::setMeasure(const Distribution & measure)
{
  measure_ = measure;
}

Distribution OrthonormalizationAlgorithmImplementation::getMeasure() const
{
  return measure_;
}

/* String converter */
String OrthonormalizationAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OrthonormalizationAlgorithmImplementation::GetClassName()
      << " measure=" << measure_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OrthonormalizationAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void OrthonormalizationAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS

