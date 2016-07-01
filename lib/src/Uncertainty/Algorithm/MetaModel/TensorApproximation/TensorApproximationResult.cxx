//                                               -*- C++ -*-
/**
 *  @brief The result of a tensor approximation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/TensorApproximationResult.hxx"
#include "openturns/Os.hxx"
#include "openturns/OSS.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RankOneTensorEvaluation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(TensorApproximationResult);
static const Factory<TensorApproximationResult> Factory_TensorApproximationResult;


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
    const NumericalMathFunction & transformation,
    const NumericalMathFunction & inverseTransformation,
    const NumericalMathFunction & composedModel,
    const Collection<CanonicalTensor> & tensor,
    const NumericalPoint & residuals,
    const NumericalPoint & relativeErrors)
  : MetaModelResult(NumericalMathFunction(), NumericalMathFunction(), residuals, relativeErrors)
  , distribution_(distribution)
  , transformation_(transformation)
  , inverseTransformation_(inverseTransformation)
  , composedModel_(composedModel)
  , tensor_(tensor)
  , composedMetaModel_()
{
  NumericalMathFunctionCollection marginals;
//   const UnsignedInteger dimension = distribution.getDimension();
  const UnsignedInteger outputDimension = tensor_.getSize();// for now
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    // build the metamodel
    UnsignedInteger rank = tensor_[outputIndex].getRank();
    NumericalMathFunctionCollection prodColl(rank);
    NumericalPoint rk(rank);
    for (UnsignedInteger k = 0; k < rank; ++ k)
    {
      Pointer<NumericalMathEvaluationImplementation> p_evaluation = new RankOneTensorEvaluation(tensor_[outputIndex].rank1tensors_[k]);
      prodColl[k] = NumericalMathFunction(p_evaluation);
      rk[k] = tensor_[outputIndex].rank1tensors_[k].radius_;
    }

    NumericalMathFunction combination(prodColl, rk);
    marginals.add(combination);
  }
  composedMetaModel_ = NumericalMathFunction(marginals);
  metaModel_ = NumericalMathFunction(composedMetaModel_, transformation);
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

String TensorApproximationResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "meta model=" << metaModel_ << Os::GetEndOfLine();
//   oss << offset << "orthogonal basis=" << orthogonalBasis_;
  return oss;
}

/* Distribution accessor */
Distribution TensorApproximationResult::getDistribution() const
{
  return distribution_;
}

/* IsoProbabilisticTransformation accessor */
NumericalMathFunction TensorApproximationResult::getTransformation() const
{
  return transformation_;
}

/* InverseIsoProbabilisticTransformation accessor */
NumericalMathFunction TensorApproximationResult::getInverseTransformation() const
{
  return inverseTransformation_;
}

/* Composed model accessor */
NumericalMathFunction TensorApproximationResult::getComposedModel() const
{
  return composedModel_;
}

/* Composed meta model accessor */
NumericalMathFunction TensorApproximationResult::getComposedMetaModel() const
{
  return composedMetaModel_;
}

/* Method save() stores the object through the StorageManager */
void TensorApproximationResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "transformation_", transformation_ );
  adv.saveAttribute( "inverseTransformation_", inverseTransformation_ );
  adv.saveAttribute( "composedModel_", composedModel_ );
  adv.saveAttribute( "composedMetaModel_", composedMetaModel_ );
}


/* Method load() reloads the object from the StorageManager */
void TensorApproximationResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "transformation_", transformation_ );
  adv.loadAttribute( "inverseTransformation_", inverseTransformation_ );
  adv.loadAttribute( "composedModel_", composedModel_ );
  adv.loadAttribute( "composedMetaModel_", composedMetaModel_ );
}



END_NAMESPACE_OPENTURNS
