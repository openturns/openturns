//                                               -*- C++ -*-
/**
 *  @brief Result of a Meta-model algorithm
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
#include "openturns/MetaModelResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetaModelResult)

static const Factory<MetaModelResult> Factory_MetaModelResult;

/* Default constructor */
MetaModelResult::MetaModelResult()
  : PersistentObject()
{
  // Nothing to do
}

/* Standard constructor */
MetaModelResult::MetaModelResult(const Sample & inputSample,
                                 const Sample & outputSample,
                                 const Function & metaModel)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , metaModel_(metaModel)
{
  if (metaModel.getInputDimension() != inputSample.getDimension())
    throw InvalidArgumentException(HERE) << "The input sample dimension (" << inputSample.getDimension() << ") must match the metamodel input dimension (" << metaModel.getInputDimension() << ")";
  if (metaModel.getOutputDimension() != outputSample.getDimension())
    throw InvalidArgumentException(HERE) << "The output sample dimension (" << outputSample.getDimension() << ") must match the metamodel output dimension (" << metaModel.getOutputDimension() << ")";
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "The input sample size (" << inputSample.getSize() << ") must match the output sample size (" << outputSample.getSize() << ")";
}

/* Virtual constructor */
MetaModelResult * MetaModelResult::clone() const
{
  return new MetaModelResult(*this);
}

/* MetaModel accessor */
void MetaModelResult::setMetaModel(const Function & metaModel)
{
  metaModel_ = metaModel;
}

Function MetaModelResult::getMetaModel() const
{
  return metaModel_;
}

/* Sample accessor */
void MetaModelResult::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
}

Sample MetaModelResult::getInputSample() const
{
  return inputSample_;
}

/* Sample accessor */
void MetaModelResult::setOutputSample(const Sample & outputSample)
{
  outputSample_ = outputSample;
}

Sample MetaModelResult::getOutputSample() const
{
  return outputSample_;
}

/* String converter */
String MetaModelResult::__repr__() const
{
  OSS oss;
  oss << " metaModel=" << metaModel_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MetaModelResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "metaModel_", metaModel_ );
}

/* Method load() reloads the object from the StorageManager */
void MetaModelResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  if (adv.hasAttribute( "inputSample_"))
  {
    adv.loadAttribute( "inputSample_", inputSample_ );
    adv.loadAttribute( "outputSample_", outputSample_ );
  }
  adv.loadAttribute( "metaModel_", metaModel_ );
}

END_NAMESPACE_OPENTURNS
