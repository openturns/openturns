//                                               -*- C++ -*-
/**
 *  @brief Result for field functional chaos
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

#include "openturns/FieldFunctionalChaosResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldFunctionalChaosResult)

static const Factory<FieldFunctionalChaosResult> Factory_FieldFunctionalChaosResult;
static const Factory<PersistentCollection< KarhunenLoeveResult> > Factory_PersistentCollection_KarhunenLoeveResult;

/* Default constructor */
FieldFunctionalChaosResult::FieldFunctionalChaosResult()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
FieldFunctionalChaosResult::FieldFunctionalChaosResult(const Collection<KarhunenLoeveResult> & inputKLResultCollection,
    const FunctionalChaosResult & fceResult,
    const Collection<KarhunenLoeveResult> & outputKLResultCollection)
  : PersistentObject()
  , inputKLResultCollection_(inputKLResultCollection)
  , fceResult_(fceResult)
  , outputKLResultCollection_(outputKLResultCollection)
{
  // Nothing to do
}

/* Virtual constructor */
FieldFunctionalChaosResult * FieldFunctionalChaosResult::clone() const
{
  return new FieldFunctionalChaosResult(*this);
}

/* String converter */
String FieldFunctionalChaosResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

/* KL decompositions accessors */
Collection<KarhunenLoeveResult> FieldFunctionalChaosResult::getInputKLResultCollection() const
{
  return inputKLResultCollection_;
}

Collection<KarhunenLoeveResult> FieldFunctionalChaosResult::getOutputKLResultCollection() const
{
  return outputKLResultCollection_;
}

/* Chaos result accessor */
FunctionalChaosResult FieldFunctionalChaosResult::getFCEResult() const
{
  return fceResult_;
}

/* Block indices accessors */
Collection<Indices> FieldFunctionalChaosResult::getBlockIndices() const
{
  return blockIndices_;
}

void FieldFunctionalChaosResult::setBlockIndices(const Collection<Indices> & blockIndices)
{
  Indices flat;
  for (UnsignedInteger i = 0; i < blockIndices.getSize(); ++ i)
    flat.add(blockIndices[i]);
  if ((flat.getSize() != inputProcessSample_.getDimension()) || !flat.check(inputProcessSample_.getDimension()))
    throw InvalidArgumentException(HERE) << "Block indices does not match with input dimension";
  blockIndices_ = blockIndices;
}

/* Metamodel accessor */
FieldToPointFunction FieldFunctionalChaosResult::getFieldToPointMetamodel() const
{
  return fieldToPointMetamodel_;
}

void FieldFunctionalChaosResult::setMetamodel(const FieldToPointFunction & metamodel)
{
  fieldToPointMetamodel_ = metamodel;
}

PointToFieldFunction FieldFunctionalChaosResult::getPointToFieldMetamodel() const
{
  return pointToFieldMetamodel_;
}

void FieldFunctionalChaosResult::setMetamodel(const PointToFieldFunction & metamodel)
{
  pointToFieldMetamodel_ = metamodel;
}

FieldFunction FieldFunctionalChaosResult::getFieldMetamodel() const
{
  return fieldMetamodel_;
}

void FieldFunctionalChaosResult::setMetamodel(const FieldFunction & metamodel)
{
  fieldMetamodel_ = metamodel;
}

/* Sample accessors */
void FieldFunctionalChaosResult::setInputProcessSample(const ProcessSample & inputProcessSample)
{
  inputProcessSample_ = inputProcessSample;
}

ProcessSample FieldFunctionalChaosResult::getInputProcessSample() const
{
  return inputProcessSample_;
}

void FieldFunctionalChaosResult::setOutputProcessSample(const ProcessSample & outputProcessSample)
{
  outputProcessSample_ = outputProcessSample;
}

ProcessSample FieldFunctionalChaosResult::getOutputProcessSample() const
{
  return outputProcessSample_;
}

void FieldFunctionalChaosResult::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
}

Sample FieldFunctionalChaosResult::getInputSample() const
{
  return inputSample_;
}

void FieldFunctionalChaosResult::setOutputSample(const Sample & outputSample)
{
  outputSample_ = outputSample;
}

Sample FieldFunctionalChaosResult::getOutputSample() const
{
  return outputSample_;
}

void FieldFunctionalChaosResult::setModesSample(const Sample & modesSample)
{
  modesSample_ = modesSample;
}

Sample FieldFunctionalChaosResult::getModesSample() const
{
  return modesSample_;
}

/* Method save() stores the object through the StorageManager */
void FieldFunctionalChaosResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputKLResultCollection_", inputKLResultCollection_);
  adv.saveAttribute("outputKLResultCollection_", outputKLResultCollection_);
  adv.saveAttribute("fceResult_", fceResult_);
  adv.saveAttribute("blockIndices_", blockIndices_);
  adv.saveAttribute("fieldToPointMetamodel_", fieldToPointMetamodel_);
  adv.saveAttribute("pointToFieldMetamodel_", pointToFieldMetamodel_);
  adv.saveAttribute("fieldMetamodel_", fieldMetamodel_);
  adv.saveAttribute("inputProcessSample_", inputProcessSample_);
  adv.saveAttribute("outputProcessSample_", outputProcessSample_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("modesSample_", modesSample_);
}

/* Method load() reloads the object from the StorageManager */
void FieldFunctionalChaosResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputKLResultCollection_", inputKLResultCollection_);
  adv.loadAttribute("outputKLResultCollection_", outputKLResultCollection_);
  adv.loadAttribute("fceResult_", fceResult_);
  adv.loadAttribute("blockIndices_", blockIndices_);
  adv.loadAttribute("fieldToPointMetamodel_", fieldToPointMetamodel_);
  adv.loadAttribute("pointToFieldMetamodel_", pointToFieldMetamodel_);
  adv.loadAttribute("fieldMetamodel_", fieldMetamodel_);
  adv.loadAttribute("inputProcessSample_", inputProcessSample_);
  adv.loadAttribute("outputProcessSample_", outputProcessSample_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("modesSample_", modesSample_);
}

END_NAMESPACE_OPENTURNS
