//                                               -*- C++ -*-
/**
 *  @brief Result of a Meta-model algorithm
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/MetaModelResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetaModelResult);

static const Factory<MetaModelResult> RegisteredFactory;

/* Default constructor */
MetaModelResult::MetaModelResult()
  : PersistentObject()
  , residuals_(0)
  , relativeErrors_(0)
{
  // Nothing to do
}

/* Standard constructor */
MetaModelResult::MetaModelResult(const NumericalMathFunction & model,
                                 const NumericalMathFunction & metaModel,
                                 const NumericalPoint & residuals,
                                 const NumericalPoint & relativeErrors)
  : PersistentObject()
  , model_(model)
  , metaModel_(metaModel)
  , residuals_(residuals)
  , relativeErrors_(relativeErrors)
{
  // Nothing to do
}

/* Virtual constructor */
MetaModelResult * MetaModelResult::clone() const
{
  return new MetaModelResult(*this);
}

/* Model accessor */
void MetaModelResult::setModel(const NumericalMathFunction & model)
{
  model_ = model;
}

NumericalMathFunction MetaModelResult::getModel() const
{
  return model_;
}

/* MetaModel accessor */
void MetaModelResult::setMetaModel(const NumericalMathFunction & metaModel)
{
  metaModel_ = metaModel;
}

NumericalMathFunction MetaModelResult::getMetaModel() const
{
  return metaModel_;
}

/* Marginal residuals accessor */
void MetaModelResult::setResiduals(const NumericalPoint & residuals)
{
  residuals_ = residuals;
}

NumericalPoint MetaModelResult::getResiduals() const
{
  return residuals_;
}

/* Relative error accessor */
void MetaModelResult::setRelativeErrors(const NumericalPoint & relativeErrors)
{
  relativeErrors_ = relativeErrors;
}

NumericalPoint MetaModelResult::getRelativeErrors() const
{
  return relativeErrors_;
}

/* String converter */
String MetaModelResult::__repr__() const
{
  OSS oss;
  oss << "model=" << model_
      << " metaModel=" << metaModel_
      << " residuals=" << residuals_
      << " relativeErrors=" << relativeErrors_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MetaModelResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "model_", model_ );
  adv.saveAttribute( "metaModel_", metaModel_ );
  adv.saveAttribute( "residuals_", residuals_ );
  adv.saveAttribute( "relativeErrors_", relativeErrors_ );
}

/* Method load() reloads the object from the StorageManager */
void MetaModelResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "model_", model_ );
  adv.loadAttribute( "metaModel_", metaModel_ );
  adv.loadAttribute( "residuals_", residuals_ );
  adv.loadAttribute( "relativeErrors_", relativeErrors_ );
}

END_NAMESPACE_OPENTURNS
