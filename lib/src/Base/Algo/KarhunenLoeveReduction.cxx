//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve reduction
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
#include "openturns/KarhunenLoeveReduction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveLifting.hxx"
#include "openturns/KarhunenLoeveProjection.hxx"
#include "openturns/InverseTrendTransform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KarhunenLoeveReduction)

static const Factory<KarhunenLoeveReduction> Factory_KarhunenLoeveReduction;

/* Default constructor */
KarhunenLoeveReduction::KarhunenLoeveReduction()
  : FieldFunctionImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
KarhunenLoeveReduction::KarhunenLoeveReduction(const KarhunenLoeveResult & result)
  : FieldFunctionImplementation(result.getMesh(), result.getModesAsProcessSample().getDimension(), result.getMesh(), result.getModesAsProcessSample().getDimension())
  , connection_(KarhunenLoeveLifting(result), KarhunenLoeveProjection(result))
{
  // Nothing to do
}

/* Parameter constructor */
KarhunenLoeveReduction::KarhunenLoeveReduction(const KarhunenLoeveResult & result,
    const TrendTransform & trend)
  : KarhunenLoeveReduction(result)
{
  setTrend(trend);
}

void KarhunenLoeveReduction::setTrend(const TrendTransform & trend)
{
  connection_ = FieldToFieldConnection(trend, FieldToFieldConnection(connection_, trend.getInverse()));
}

/* Virtual constructor */
KarhunenLoeveReduction * KarhunenLoeveReduction::clone() const
{
  return new KarhunenLoeveReduction(*this);
}

/* Comparison operator */
Bool KarhunenLoeveReduction::operator ==(const KarhunenLoeveReduction & other) const
{
  return connection_ == other.connection_;
}

/* String converter */
String KarhunenLoeveReduction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveReduction::GetClassName()
      << " connection=" << connection_.__repr__();
  return oss;
}

/* String converter */
String KarhunenLoeveReduction::__str__(const String & offset) const
{
  return OSS(false) << connection_.__str__(offset);
}

/* Operator () */
Sample KarhunenLoeveReduction::operator() (const Sample & inFld) const
{
  if (inFld.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected field values of dimension=" << getInputDimension() << ", got dimension=" << inFld.getDimension();
  if (inFld.getSize() != getInputMesh().getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: expected field values of size=" << getInputMesh().getVerticesNumber() << ", got size=" << inFld.getSize();
  callsNumber_.increment();
  return connection_(inFld);
}

/* Get the i-th marginal function */
KarhunenLoeveReduction::Implementation KarhunenLoeveReduction::getMarginal(const UnsignedInteger /*i*/) const
{
  throw NotYetImplementedException(HERE) << "in KarhunenLoeveReduction::getMarginal(UnsignedInteger)";
}

/* Get the function corresponding to indices components */
KarhunenLoeveReduction::Implementation KarhunenLoeveReduction::getMarginal(const Indices & /*indices*/) const
{
  throw NotYetImplementedException(HERE) << "in KarhunenLoeveReduction::getMarginal(Indices)";
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveReduction::save(Advocate & adv) const
{
  FieldFunctionImplementation::save(adv);
  adv.saveAttribute("connection_", connection_);
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveReduction::load(Advocate & adv)
{
  FieldFunctionImplementation::load(adv);
  adv.loadAttribute("connection_", connection_);
}

END_NAMESPACE_OPENTURNS
