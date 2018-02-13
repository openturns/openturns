//                                               -*- C++ -*-
/**
 *  @brief Field to point function allowing to perform the projection of a
 *         field over a Karhunen-Loeve basis
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/KarhunenLoeveProjection.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KarhunenLoeveProjection)

static const Factory<KarhunenLoeveProjection> Factory_KarhunenLoeveProjection;

/* Default constructor */
KarhunenLoeveProjection::KarhunenLoeveProjection()
  : FieldToPointFunctionImplementation()
  , result_()
{
  // Nothing to do
}

/* Parameter constructor */
KarhunenLoeveProjection::KarhunenLoeveProjection(const KarhunenLoeveResult & result)
  : FieldToPointFunctionImplementation(result.getModesAsProcessSample().getMesh().getDimension(), result.getModesAsProcessSample().getDimension(), result.getEigenValues().getSize())
  , result_(result)
{
  // Set the description
  const UnsignedInteger size = result_.getModesAsProcessSample().getSize();
  if (size == 0) setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  else setInputDescription(result_.getModesAsProcessSample()[0].getDescription());
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "xi"));
}

/* Virtual constructor */
KarhunenLoeveProjection * KarhunenLoeveProjection::clone() const
{
  return new KarhunenLoeveProjection(*this);
}

/* Comparison operator */
Bool KarhunenLoeveProjection::operator ==(const KarhunenLoeveProjection & other) const
{
  return true;
}

/* String converter */
String KarhunenLoeveProjection::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveProjection::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " number of calls=" << callsNumber_;
  return oss;
}

/* String converter */
String KarhunenLoeveProjection::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}

/* Get the i-th marginal function */
KarhunenLoeveProjection::Implementation KarhunenLoeveProjection::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
KarhunenLoeveProjection::Implementation KarhunenLoeveProjection::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  throw NotYetImplementedException(HERE) << "In KarhunenLoeveProjection::getMarginal(const Indices & indices) const";
}

/* Operator () */
Point KarhunenLoeveProjection::operator() (const Field & inFld) const
{
  const Point outPoint(result_.project(inFld));
  ++callsNumber_;
  return outPoint;
}

/* Operator () */
Sample KarhunenLoeveProjection::operator() (const FieldSample & inPS) const
{
  const Sample outSample(result_.project(inPS));
  callsNumber_ += outSample.getSize();
  return outSample;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveProjection::save(Advocate & adv) const
{
  FieldToPointFunctionImplementation::save(adv);
  adv.saveAttribute( "result_", result_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveProjection::load(Advocate & adv)
{
  FieldToPointFunctionImplementation::load(adv);
  adv.loadAttribute( "result_", result_ );
}

END_NAMESPACE_OPENTURNS
