//                                               -*- C++ -*-
/**
 *  @brief The result of a chaos expansion
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FunctionalChaosResult.hxx"
#include "openturns/Os.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(FunctionalChaosResult)
static const Factory<FunctionalChaosResult> Factory_FunctionalChaosResult;


/* Default constructor */
FunctionalChaosResult::FunctionalChaosResult()
  : MetaModelResult()
  , distribution_()
  , transformation_()
  , inverseTransformation_()
  , composedModel_()
  , orthogonalBasis_()
  , I_(0)
  , alpha_k_(0, 0)
  , Psi_k_(0)
{
  // Nothing to do
}


/* Default constructor */
FunctionalChaosResult::FunctionalChaosResult(const Distribution & distribution,
    const Function & transformation,
    const Function & inverseTransformation,
    const OrthogonalBasis & orthogonalBasis,
    const Indices & I,
    const Sample & alpha_k,
    const FunctionCollection & Psi_k,
    const Point & residuals,
    const Point & relativeErrors)
  : MetaModelResult(Function(), residuals, relativeErrors)
  , distribution_(distribution)
  , transformation_(transformation)
  , inverseTransformation_(inverseTransformation)
  , orthogonalBasis_(orthogonalBasis)
  , I_(I)
  , alpha_k_(alpha_k)
  , Psi_k_(Psi_k)
  , composedMetaModel_(DualLinearCombinationFunction(Psi_k, alpha_k))
{
  if (transformation.getEvaluation().getImplementation()->getClassName() == "IdentityEvaluation")
    metaModel_ = composedMetaModel_;
  else
    metaModel_ = ComposedFunction(composedMetaModel_, transformation);
}


/* Virtual constructor */
FunctionalChaosResult * FunctionalChaosResult::clone() const
{
  return new FunctionalChaosResult(*this);
}


/* String converter */
String FunctionalChaosResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " derived from " << MetaModelResult::__repr__()
         << " distribution=" << distribution_
         << " transformation=" << transformation_
         << " inverseTransformation=" << inverseTransformation_
         << " orthogonalBasis=" << orthogonalBasis_
         << " indices=" << I_
         << " coefficients=" << alpha_k_
         << " reduced basis=" << Psi_k_
         << " residuals=" << residuals_
         << " relativeErrors=" << relativeErrors_
         << " composedMetaModel=" << composedMetaModel_
         << " metaModel=" << metaModel_;
}

String FunctionalChaosResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "meta model=" << metaModel_ << Os::GetEndOfLine() << offset;
  oss << "orthogonal basis=" << orthogonalBasis_;
  return oss;
}

/* Distribution accessor */
Distribution FunctionalChaosResult::getDistribution() const
{
  return distribution_;
}

/* IsoProbabilisticTransformation accessor */
Function FunctionalChaosResult::getTransformation() const
{
  return transformation_;
}

/* InverseIsoProbabilisticTransformation accessor */
Function FunctionalChaosResult::getInverseTransformation() const
{
  return inverseTransformation_;
}

/* Composed model accessor */
Function FunctionalChaosResult::getComposedModel() const
{
  LOGWARN(OSS() << "FunctionalChaosResult.getComposedModel is deprecated");
  return Function();
}

/* Orthogonal basis accessor */
OrthogonalBasis FunctionalChaosResult::getOrthogonalBasis() const
{
  return orthogonalBasis_;
}

/* Indices accessor */
Indices FunctionalChaosResult::getIndices() const
{
  return I_;
}

/* Coefficients accessor */
Sample FunctionalChaosResult::getCoefficients() const
{
  return alpha_k_;
}

/* Reduced basis accessor */
FunctionalChaosResult::FunctionCollection FunctionalChaosResult::getReducedBasis() const
{
  return Psi_k_;
}

/* Composed meta model accessor */
Function FunctionalChaosResult::getComposedMetaModel() const
{
  return composedMetaModel_;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "transformation_", transformation_ );
  adv.saveAttribute( "inverseTransformation_", inverseTransformation_ );
  adv.saveAttribute( "orthogonalBasis_", orthogonalBasis_ );
  adv.saveAttribute( "I_", I_ );
  adv.saveAttribute( "alpha_k_", alpha_k_ );
  adv.saveAttribute( "Psi_k_", Psi_k_ );
  adv.saveAttribute( "composedMetaModel_", composedMetaModel_ );
}


/* Method load() reloads the object from the StorageManager */
void FunctionalChaosResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "transformation_", transformation_ );
  adv.loadAttribute( "inverseTransformation_", inverseTransformation_ );
  adv.loadAttribute( "orthogonalBasis_", orthogonalBasis_ );
  adv.loadAttribute( "I_", I_ );
  adv.loadAttribute( "alpha_k_", alpha_k_ );
  adv.loadAttribute( "Psi_k_", Psi_k_ );
  adv.loadAttribute( "composedMetaModel_", composedMetaModel_ );
}



END_NAMESPACE_OPENTURNS
