//                                               -*- C++ -*-
/**
 *  @brief The result of a chaos expansion
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
  , orthogonalBasis_()
  , I_(0)
  , alpha_k_(0, 0)
  , Psi_k_(0)
{
  // Nothing to do
}


/* Default constructor */
FunctionalChaosResult::FunctionalChaosResult(const Sample & inputSample,
    const Sample & outputSample,
    const Distribution & distribution,
    const Function & transformation,
    const Function & inverseTransformation,
    const OrthogonalBasis & orthogonalBasis,
    const Indices & I,
    const Sample & alpha_k,
    const FunctionCollection & Psi_k,
    const Point & residuals,
    const Point & relativeErrors)
  : MetaModelResult(inputSample, outputSample, Function(), residuals, relativeErrors)
  , distribution_(distribution)
  , transformation_(transformation)
  , inverseTransformation_(inverseTransformation)
  , orthogonalBasis_(orthogonalBasis)
  , I_(I)
  , alpha_k_(alpha_k)
  , Psi_k_(Psi_k)
{
  // The composed meta model will be a dual linear combination
  composedMetaModel_ = DualLinearCombinationFunction(Psi_k, alpha_k);
  if (transformation.getEvaluation().getImplementation()->getClassName() == "IdentityEvaluation")
    metaModel_ = composedMetaModel_;
  else
    metaModel_ = ComposedFunction(composedMetaModel_, transformation);
  metaModel_.setInputDescription(inputSample.getDescription());
  metaModel_.setOutputDescription(outputSample.getDescription());
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

String FunctionalChaosResult::__str__(const String & /*offset*/) const
{
  return __repr_markdown__();
}

String FunctionalChaosResult::__repr_markdown__() const
{
  OSS oss(false);
  const UnsignedInteger indicesSize = I_.getSize();
  const EnumerateFunction enumerateFunction(orthogonalBasis_.getEnumerateFunction());
  const UnsignedInteger outputDimension = metaModel_.getOutputDimension();
  const UnsignedInteger inputDimension = distribution_.getDimension();
  oss << getClassName() << Os::GetEndOfLine()
      << "- input dimension=" << inputDimension << Os::GetEndOfLine()
      << "- output dimension=" << outputDimension << Os::GetEndOfLine()
      << "- distribution dimension=" << distribution_.getDimension() << Os::GetEndOfLine()  
      << "- transformation=" << transformation_.getInputDimension() << " -> " << transformation_.getOutputDimension() << Os::GetEndOfLine()
      << "- inverse transformation=" << inverseTransformation_.getInputDimension() << " -> " << inverseTransformation_.getOutputDimension() << Os::GetEndOfLine()
      << "- orthogonal basis dimension=" << orthogonalBasis_.getMeasure().getDimension() << Os::GetEndOfLine()
      << "- indices size=" << indicesSize << Os::GetEndOfLine()
      << "- relative errors=" << relativeErrors_ << Os::GetEndOfLine()
      << "- residuals=" << residuals_ << Os::GetEndOfLine();
  oss << Os::GetEndOfLine();

  const Scalar ell_threshold = ResourceMap::GetAsUnsignedInteger("FunctionalChaosResult-PrintEllipsisThreshold");
  const UnsignedInteger ell_size = ResourceMap::GetAsUnsignedInteger("FunctionalChaosResult-PrintEllipsisSize");
  const UnsignedInteger columnWidth = ResourceMap::GetAsUnsignedInteger("FunctionalChaosResult-PrintColumnWidth");
  const UnsignedInteger ellipsis = (indicesSize * outputDimension > ell_threshold);
  const Bool isEllipsisEnabled = (ellipsis && outputDimension > 2 * ell_size);
  // Print table header
  oss << "| Index |" << OSS::PadString(" Multi-index ", columnWidth) << "|";
  String intermediateString;
  if (isEllipsisEnabled)
  {
    for (UnsignedInteger j = 0 ; j < ell_size; ++ j)
    {
      intermediateString = OSS() << " Coeff. #" << j;
      oss << OSS::PadString(intermediateString, columnWidth) << "|";
    }
    oss << OSS::PadString(" ... ", columnWidth) << "|";
    for (UnsignedInteger j = outputDimension - ell_size ; j < outputDimension; ++ j)
    {
      intermediateString = OSS() << " Coeff. #" << j;
      oss << OSS::PadString(intermediateString, columnWidth) << "|";
    }
    oss << Os::GetEndOfLine();
  }
  else
  {
      if (outputDimension == 1) 
        oss << OSS::PadString(" Coefficient ", columnWidth) << "|" << Os::GetEndOfLine();
      else
      {
        for (UnsignedInteger j = 0 ; j < outputDimension; ++ j)
        {
          intermediateString = OSS() << " Coeff. #" << j;
          oss << OSS::PadString(intermediateString, columnWidth) << "|";
        }
        oss << Os::GetEndOfLine();
      }
  }
  // Print dashes
  String dashesSeparator = String(columnWidth, '-') + "|";
  oss << "|-------|" << dashesSeparator;
  if (isEllipsisEnabled) oss << OSS::RepeatString(1 + 2 * ell_size, dashesSeparator) << Os::GetEndOfLine();
  else if (outputDimension == 1) oss << dashesSeparator << Os::GetEndOfLine();
  else oss << OSS::RepeatString(outputDimension, dashesSeparator) << Os::GetEndOfLine();
  // Print table content: multi-indices
  for (UnsignedInteger i = 0; i < indicesSize; ++ i)
  {
    if (ellipsis && indicesSize > 2 * ell_size)
    {
      if (i == ell_size)
      {
        oss << "| ...   |";
        const String blankColumn(String(columnWidth, ' ') + "|");
        oss << OSS::RepeatString(1 + ell_size, blankColumn)
            << OSS::PadString(" ... ", columnWidth) << "|"
            << OSS::RepeatString(ell_size, blankColumn) << Os::GetEndOfLine();
        continue;
      }
      else if (i > ell_size && i < indicesSize - ell_size) continue;
    }
    const UnsignedInteger rankIndex = I_[i];
    const Indices multiindex(enumerateFunction(rankIndex));
    
    oss << "|" << std::setw(6) << i << " |";

    intermediateString = OSS() << " " << multiindex;
    oss << OSS::PadString(intermediateString, columnWidth) << "|";

    if (isEllipsisEnabled)
    {
      for (UnsignedInteger j = 0 ; j < ell_size; ++ j)
      {
        intermediateString = OSS() << " " << alpha_k_[i][j];
        oss << OSS::PadString(intermediateString, columnWidth) << "|";
      }
      oss << OSS::PadString(" ... ", columnWidth) << "|";
      for (UnsignedInteger j = outputDimension - ell_size ; j < outputDimension; ++ j)
      {
        intermediateString = OSS() << " " << alpha_k_[i][j];
        oss << OSS::PadString(intermediateString, columnWidth) << "|";
      }
      oss << Os::GetEndOfLine();
    }
    else
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      {
        intermediateString = OSS() << " " << alpha_k_[i][j];
        oss << OSS::PadString(intermediateString, columnWidth) << "|";
      }
      oss << Os::GetEndOfLine();
    } // If isEllipsisEnabled
  } // For i in indices size
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
