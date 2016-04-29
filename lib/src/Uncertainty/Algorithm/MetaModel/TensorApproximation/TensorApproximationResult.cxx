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
//   , orthogonalBasis_()
//   , I_(0)
//   , alpha_k_(0, 0)
//   , Psi_k_(0)
{
  // Nothing to do
}


/* Default constructor */
TensorApproximationResult::TensorApproximationResult(
//   const NumericalMathFunction & model,
    const Distribution & distribution,
    const NumericalMathFunction & transformation,
    const NumericalMathFunction & inverseTransformation,
    const NumericalMathFunction & composedModel,
    const CanonicalTensor & tensor,
//     const OrthogonalBasis & orthogonalBasis,
//     const Indices & I,
//     const NumericalSample & alpha_k,
//     const NumericalMathFunctionCollection & Psi_k,
    const NumericalPoint & residuals,
    const NumericalPoint & relativeErrors)
  : MetaModelResult(NumericalMathFunction(), NumericalMathFunction(), residuals, relativeErrors)
  , distribution_(distribution)
  , transformation_(transformation)
  , inverseTransformation_(inverseTransformation)
  , composedModel_(composedModel)
  , tensor_(tensor)
//   , orthogonalBasis_(orthogonalBasis)
//   , I_(I)
//   , alpha_k_(alpha_k)
//   , Psi_k_(Psi_k)
  , composedMetaModel_()
{
  NumericalMathFunctionCollection marginals;
//   const UnsignedInteger dimension = distribution.getDimension();
  const UnsignedInteger outputDimension = 1;// for now
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    // build the metamodel
    UnsignedInteger r = 1; //tensor_.getRank();
    NumericalMathFunctionCollection prodColl(r);
    NumericalPoint rk(r);
    for (UnsignedInteger k = 0; k < r; ++ k)
    {
      Pointer<NumericalMathEvaluationImplementation> p_evaluation = new RankOneTensorEvaluation(tensor_.rank1tensors_[k]);
      prodColl[k] = NumericalMathFunction(p_evaluation);
      rk[k] = tensor_.rank1tensors_[k].alpha_;
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
//          << " orthogonalBasis=" << orthogonalBasis_
//          << " indices=" << I_
//          << " coefficients=" << alpha_k_
//          << " reduced basis=" << Psi_k_
//          << " residuals=" << residuals_
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

// /* Orthogonal basis accessor */
// OrthogonalBasis TensorApproximationResult::getOrthogonalBasis() const
// {
//   return orthogonalBasis_;
// }
// 
// /* Indices accessor */
// Indices TensorApproximationResult::getIndices() const
// {
//   return I_;
// }
// 
// /* Coefficients accessor */
// NumericalSample TensorApproximationResult::getCoefficients() const
// {
//   return alpha_k_;
// }
// 
// /* Reduced basis accessor */
// TensorApproximationResult::NumericalMathFunctionCollection TensorApproximationResult::getReducedBasis() const
// {
//   return Psi_k_;
// }

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
//   adv.saveAttribute( "orthogonalBasis_", orthogonalBasis_ );
//   adv.saveAttribute( "I_", I_ );
//   adv.saveAttribute( "alpha_k_", alpha_k_ );
//   adv.saveAttribute( "Psi_k_", Psi_k_ );
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
//   adv.loadAttribute( "orthogonalBasis_", orthogonalBasis_ );
//   adv.loadAttribute( "I_", I_ );
//   adv.loadAttribute( "alpha_k_", alpha_k_ );
//   adv.loadAttribute( "Psi_k_", Psi_k_ );
  adv.loadAttribute( "composedMetaModel_", composedMetaModel_ );
}



END_NAMESPACE_OPENTURNS
