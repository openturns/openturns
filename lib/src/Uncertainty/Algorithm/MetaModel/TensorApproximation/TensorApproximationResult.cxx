//                                               -*- C++ -*-
/**
 *  @brief The result of a tensor approximation
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
#include "openturns/TensorApproximationResult.hxx"
#include "openturns/Os.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/CanonicalTensorGradient.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(TensorApproximationResult)
static const Factory<TensorApproximationResult> Factory_TensorApproximationResult;

static const Factory<PersistentCollection<CanonicalTensorEvaluation> > Factory_PersistentCollection_CanonicalTensorEvaluation;


/* Default constructor */
TensorApproximationResult::TensorApproximationResult()
  : MetaModelResult()
  , distribution_()
  , transformation_()
  , inverseTransformation_()
  , composedModel_()
{
  // Nothing to do
}


/* Default constructor */
TensorApproximationResult::TensorApproximationResult(
  const Distribution & distribution,
  const Function & transformation,
  const Function & inverseTransformation,
  const Function & composedModel,
  const Collection<CanonicalTensorEvaluation> & tensorCollection,
  const Point & residuals,
  const Point & relativeErrors)
  : MetaModelResult(Function(), Function(), residuals, relativeErrors)
  , distribution_(distribution)
  , transformation_(transformation)
  , inverseTransformation_(inverseTransformation)
  , composedModel_(composedModel)
  , tensorCollection_(tensorCollection)
  , composedMetaModel_()
{
  FunctionCollection marginals;
  const UnsignedInteger outputDimension = tensorCollection_.getSize();// for now
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    Function tensorFunction;
    tensorFunction.setEvaluation(tensorCollection_[outputIndex].clone());
    tensorFunction.setGradient(CanonicalTensorGradient(tensorCollection_[outputIndex]));
    tensorFunction.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), tensorFunction.getEvaluation()));
    marginals.add(tensorFunction);
  }
  composedMetaModel_ = AggregatedFunction(marginals);
  metaModel_ = ComposedFunction(composedMetaModel_, transformation);
}


/* Virtual constructor */
TensorApproximationResult * TensorApproximationResult::clone() const
{
  return new TensorApproximationResult(*this);
}


/* String converter */
String TensorApproximationResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " derived from " << MetaModelResult::__repr__()
         << " distribution=" << distribution_
         << " transformation=" << transformation_
         << " inverseTransformation=" << inverseTransformation_
         << " composedModel=" << composedModel_
         << " relativeErrors=" << relativeErrors_
         << " composedMetaModel=" << composedMetaModel_
         << " metaModel=" << metaModel_;
}

String TensorApproximationResult::__str__(const String & ) const
{
  OSS oss(false);
  oss << "meta model=" << metaModel_;
//   oss << Os::GetEndOfLine() << offset << "orthogonal basis=" << orthogonalBasis_;
  return oss;
}

/* Distribution accessor */
Distribution TensorApproximationResult::getDistribution() const
{
  return distribution_;
}

/* IsoProbabilisticTransformation accessor */
Function TensorApproximationResult::getTransformation() const
{
  return transformation_;
}

/* InverseIsoProbabilisticTransformation accessor */
Function TensorApproximationResult::getInverseTransformation() const
{
  return inverseTransformation_;
}

/* Composed model accessor */
Function TensorApproximationResult::getComposedModel() const
{
  return composedModel_;
}

/* Composed meta model accessor */
Function TensorApproximationResult::getComposedMetaModel() const
{
  return composedMetaModel_;
}

CanonicalTensorEvaluation TensorApproximationResult::getTensor(const UnsignedInteger marginalIndex) const
{
  return tensorCollection_[marginalIndex];
}

/* Method save() stores the object through the StorageManager */
void TensorApproximationResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("transformation_", transformation_);
  adv.saveAttribute("inverseTransformation_", inverseTransformation_);
  adv.saveAttribute("composedModel_", composedModel_);
  adv.saveAttribute("tensorCollection_", tensorCollection_);
  adv.saveAttribute("composedMetaModel_", composedMetaModel_);
}


/* Method load() reloads the object from the StorageManager */
void TensorApproximationResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("transformation_", transformation_);
  adv.loadAttribute("inverseTransformation_", inverseTransformation_);
  adv.loadAttribute("composedModel_", composedModel_);
  adv.loadAttribute("tensorCollection_", tensorCollection_);
  adv.loadAttribute("composedMetaModel_", composedMetaModel_);
}



END_NAMESPACE_OPENTURNS
