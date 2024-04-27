//                                               -*- C++ -*-
/**
 *  @brief The result of a chaos expansion
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Curve.hxx"
#include <unordered_map>
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/NormInfEnumerateFunction.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/Basis.hxx"

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
         << " metaModel=" << metaModel_
         << " isLeastSquares=" << isLeastSquares_
         << " involvesModelSelection=" << involvesModelSelection_;
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
  oss << getClassName() << "\n"
      << "- input dimension=" << inputDimension << "\n"
      << "- output dimension=" << outputDimension << "\n"
      << "- distribution dimension=" << distribution_.getDimension() << "\n"
      << "- transformation=" << transformation_.getInputDimension() << " -> " << transformation_.getOutputDimension() << "\n"
      << "- inverse transformation=" << inverseTransformation_.getInputDimension() << " -> " << inverseTransformation_.getOutputDimension() << "\n"
      << "- orthogonal basis dimension=" << orthogonalBasis_.getMeasure().getDimension() << "\n"
      << "- indices size=" << indicesSize << "\n"
      << "- relative errors=" << relativeErrors_ << "\n"
      << "- residuals=" << residuals_ << "\n"
      << "- is least squares=" << isLeastSquares_ << "\n"
      << "- is model selection=" << involvesModelSelection_ << "\n";
  oss << "\n";

  const UnsignedInteger ell_threshold = ResourceMap::GetAsUnsignedInteger("FunctionalChaosResult-PrintEllipsisThreshold");
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
    oss << "\n";
  }
  else
  {
    if (outputDimension == 1)
      oss << OSS::PadString(" Coefficient ", columnWidth) << "|" << "\n";
    else
    {
      for (UnsignedInteger j = 0 ; j < outputDimension; ++ j)
      {
        intermediateString = OSS() << " Coeff. #" << j;
        oss << OSS::PadString(intermediateString, columnWidth) << "|";
      }
      oss << "\n";
    }
  }
  // Print dashes
  String dashesSeparator = String(columnWidth, '-') + "|";
  oss << "|-------|" << dashesSeparator;
  if (isEllipsisEnabled) oss << OSS::RepeatString(1 + 2 * ell_size, dashesSeparator) << "\n";
  else if (outputDimension == 1) oss << dashesSeparator << "\n";
  else oss << OSS::RepeatString(outputDimension, dashesSeparator) << "\n";
  // Print table content: multi-indices
  for (UnsignedInteger i = 0; i < indicesSize; ++ i)
  {
    if (ellipsis && indicesSize > 2 * ell_size)
    {
      if (i == ell_size)
      {
        oss << "| ...   |";
        const String ellipsisColumn(OSS::PadString(" ... ", columnWidth) + "|");
        oss << OSS::RepeatString(1 + outputDimension, ellipsisColumn) << "\n";
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
      oss << "\n";
    }
    else
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      {
        intermediateString = OSS() << " " << alpha_k_[i][j];
        oss << OSS::PadString(intermediateString, columnWidth) << "|";
      }
      oss << "\n";
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

/* Residuals accessor */
Sample FunctionalChaosResult::getSampleResiduals() const
{
  const Sample predictionsSample(metaModel_(inputSample_));
  const Sample residualsSample(outputSample_ - predictionsSample);
  return residualsSample;
}

/* isLeastSquares accessor */
Bool FunctionalChaosResult::isLeastSquares() const
{
  return isLeastSquares_;
}

/* involvesModelSelection accessor */
Bool FunctionalChaosResult::involvesModelSelection() const
{
  return involvesModelSelection_;
}

/* isLeastSquares accessor */
void FunctionalChaosResult::setIsLeastSquares(const Bool isLeastSquares)
{
  isLeastSquares_ = isLeastSquares;
}

/* involvesModelSelection accessor */
void FunctionalChaosResult::setInvolvesModelSelection(const Bool involvesModelSelection)
{
  involvesModelSelection_ = involvesModelSelection;
}

/* Conditional expectation accessor */
FunctionalChaosResult FunctionalChaosResult::getConditionalExpectation(const Indices & conditioningIndices) const
{
  // Get marginal input sample and distribution
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  conditioningIndices.check(inputDimension);
  const Sample inputSampleMarginal(inputSample_.getMarginal(conditioningIndices));
  const Distribution inputDistributionMarginal(distribution_.getMarginal(conditioningIndices));

  // Restrict the orthogonal basis
  // TODO: create a new OrthogonalBasis.getMarginal(activeIndices)
  const String basicClassName(orthogonalBasis_.getImplementation()->getClassName());
  if (basicClassName != "OrthogonalProductPolynomialFactory")
    throw InvalidArgumentException(HERE) << "This class can only manage an OrthogonalProductPolynomialFactory "
          << "but current basis is" << basicClassName;
  const OrthogonalProductPolynomialFactory* p_basis = dynamic_cast<const OrthogonalProductPolynomialFactory*>(orthogonalBasis_.getImplementation().get());
  const OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomialCollection(p_basis->getPolynomialFamilyCollection());

  // Restrict the collection of polynomial families
  OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomialMarginalCollection;
  for (UnsignedInteger index = 0; index < inputDimension; ++ index)
    if (conditioningIndices.contains(index))
      polynomialMarginalCollection.add(polynomialCollection[index]);

  // Restrict the enumeration function
  // TODO: create a new EnumerateFunction.getMarginal(activeIndices)
  const EnumerateFunction enumerateFunction(orthogonalBasis_.getEnumerateFunction());
  const EnumerateFunctionImplementation enumerateFunctionImplementation(enumerateFunction.getImplementation());
  const String enumerateName(enumerateFunction.getImplementation()->getClassName());
  EnumerateFunction enumerateFunctionMarginal;
  const UnsignedInteger activeDimension = conditioningIndices.getSize();
  if (enumerateName == "LinearEnumerateFunction")
  {
    enumerateFunctionMarginal = LinearEnumerateFunction(activeDimension);
  }
  else if (enumerateName == "HyperbolicAnisotropicEnumerateFunction")
  {
    const HyperbolicAnisotropicEnumerateFunction* p_rule = dynamic_cast<const HyperbolicAnisotropicEnumerateFunction*>(enumerateFunction.getImplementation().get());
    const Scalar quasiNorm(p_rule->getQ());
    const Point weightVector(p_rule->getWeight());
    Point weightMarginal(activeDimension);
    for (UnsignedInteger i = 0; i < activeDimension; ++i)
      weightMarginal[i] = weightVector[conditioningIndices[i]];
    enumerateFunctionMarginal = HyperbolicAnisotropicEnumerateFunction(weightMarginal, quasiNorm);
  }
  else if (enumerateName == "NormInfEnumerateFunction")
  {
    enumerateFunctionMarginal = NormInfEnumerateFunction(activeDimension);
  }
  else
    throw InvalidArgumentException(HERE) << "Cannot manage an enumerate function with "
      << enumerateName;

  // Create the conditioned orthogonal basis
  const OrthogonalProductPolynomialFactory orthogonalBasisMarginal(polynomialMarginalCollection, enumerateFunctionMarginal);

  // Create the active transformation and its inverse
  const Distribution measureMarginal(orthogonalBasisMarginal.getMeasure());
  const DistributionTransformation transformationMarginal(inputDistributionMarginal, measureMarginal);
  const DistributionTransformation inverseTransformationMarginal(transformationMarginal.inverse());

  // Condition the multi-indices (taking into account for model selection)
  // Get the indices of active multi-indices in the reduced enumeration rule
  Indices listOfActiveReducedIndices(0);  // In the reduced enumeration rule
  Indices listOfActiveIndices(0);  // In the list of coefficients
  const UnsignedInteger numberOfFunctions = Psi_k_.getSize();
  for (UnsignedInteger k = 0; k < numberOfFunctions; ++k)
  {
    const Function basisFunction(Psi_k_[k]);
    const Indices multiIndex(enumerateFunction(I_[k]));
    // See if this function has active marginal indices only
    bool isActive = true;
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      if (!(conditioningIndices.contains(i)) && multiIndex[i] > 0)
      {
        isActive = false;
        break;
      }
    if (isActive)
    {
      listOfActiveIndices.add(k);
      Indices activeMultiIndex(activeDimension);
      UnsignedInteger localIndex = 0;
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        if (conditioningIndices.contains(i))
          activeMultiIndex[localIndex] = multiIndex[i];
      UnsignedInteger activeIndice = enumerateFunctionMarginal.inverse(activeMultiIndex);
      listOfActiveReducedIndices.add(activeIndice);
    }
  }
  const UnsignedInteger reducedActiveBasisDimension = listOfActiveReducedIndices.getSize();

  // Compute active coefficients
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  Sample activeCoefficients(reducedActiveBasisDimension, outputDimension);
  const UnsignedInteger numberOfActiveIndices = listOfActiveIndices.getSize();
  for (UnsignedInteger k = 0; k < numberOfActiveIndices; ++k)
    activeCoefficients[k] = alpha_k_[listOfActiveIndices[k]];

  // Get the conditioned functional basis
  Basis activeReducedBasis(0);
  for (UnsignedInteger k = 0; k < reducedActiveBasisDimension; ++k)
  {
    const UnsignedInteger activeIndice = listOfActiveReducedIndices[k];
    const Function activeFunction(orthogonalBasisMarginal.build(activeIndice));
    activeReducedBasis.add(activeFunction);
  }

  // Create the conditional expectation PCE
  const FunctionalChaosResult conditionalPCE(
      inputSampleMarginal,
      outputSample_,
      inputDistributionMarginal,
      transformationMarginal,
      inverseTransformationMarginal,
      orthogonalBasisMarginal,
      listOfActiveReducedIndices,
      activeCoefficients,
      activeReducedBasis,
      residuals_,
      relativeErrors_
  );
  return conditionalPCE;
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
  adv.saveAttribute( "indicesHistory_", indicesHistory_ );
  adv.saveAttribute( "coefficientsHistory_", coefficientsHistory_ );
  adv.saveAttribute( "errorHistory_", errorHistory_ );
  adv.saveAttribute( "isLeastSquares_", isLeastSquares_ );
  adv.saveAttribute( "involvesModelSelection_", involvesModelSelection_ );
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
  if (adv.hasAttribute("errorHistory_"))
  {
    adv.loadAttribute( "indicesHistory_", indicesHistory_ );
    adv.loadAttribute( "coefficientsHistory_", coefficientsHistory_ );
    adv.loadAttribute( "errorHistory_", errorHistory_ );
  }
  if (adv.hasAttribute("isLeastSquares_"))
  {
    adv.loadAttribute( "isLeastSquares_", isLeastSquares_ );
    adv.loadAttribute( "involvesModelSelection_", involvesModelSelection_ );
  }
}

IndicesCollection FunctionalChaosResult::getIndicesHistory() const
{
  if (metaModel_.getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "getIndicesHistory is only available for 1-d output dimension "
                                           << "but the current output dimension is " << metaModel_.getOutputDimension();
  return IndicesCollection(indicesHistory_);
}

Collection<Point> FunctionalChaosResult::getCoefficientsHistory() const
{
  if (metaModel_.getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "getCoefficientsHistory is only available for 1-d output dimension "
                                           << "but the current output dimension is " << metaModel_.getOutputDimension();
  return coefficientsHistory_;
}

void FunctionalChaosResult::setSelectionHistory(Collection<Indices> & indicesHistory, Collection<Point> & coefficientsHistory)
{
  indicesHistory_ = indicesHistory;
  coefficientsHistory_ = coefficientsHistory;
}

Graph FunctionalChaosResult::drawSelectionHistory() const
{
  if (metaModel_.getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "drawSelectionHistory is only available for 1-d output dimension"
                                           << "but the current output dimension is " << metaModel_.getOutputDimension();
  const UnsignedInteger size = indicesHistory_.getSize();
  if (!size)
    throw InvalidArgumentException(HERE) << "No selection history available";

  // compute union of basis terms
  std::unordered_map <UnsignedInteger, UnsignedInteger> indicesMap;
  UnsignedInteger coefId = 0;
  Indices uniqueBasisIndices;
  for (UnsignedInteger i = 0; i < size; ++ i)
    for (UnsignedInteger j = 0; j < indicesHistory_[i].getSize(); ++ j)
      if (indicesMap.find(indicesHistory_[i][j]) == indicesMap.end())
      {
        indicesMap[indicesHistory_[i][j]] = coefId;
        ++ coefId;
        uniqueBasisIndices.add(indicesHistory_[i][j]);
      }
  Sample valuesY(size + 1, coefId);
  Sample valuesX(size + 1, 1);
  for (UnsignedInteger i = 0; i < size + 1; ++ i)
    valuesX(i, 0) = i;
  for (UnsignedInteger i = 0; i < size; ++ i)
    for (UnsignedInteger j = 0; j < indicesHistory_[i].getSize(); ++ j)
      valuesY(i + 1, indicesMap[indicesHistory_[i][j]]) = coefficientsHistory_[i][j];
  Graph result("Selection history", "iteration", "coefficient", true, "upper right");
  for (UnsignedInteger i = 0; i < coefId; ++ i)
  {
    Curve curve(valuesX, valuesY.getMarginal(i));
    curve.setLegend(OSS() << "Coef. #" << uniqueBasisIndices[i]);
    result.add(curve);
  }
  result.setIntegerXTick(true);
  return result;
}

/* Error history accessor */
void FunctionalChaosResult::setErrorHistory(const Point & errorHistory)
{
  errorHistory_ = errorHistory;
}

Point FunctionalChaosResult::getErrorHistory() const
{
  return errorHistory_;
}

Graph FunctionalChaosResult::drawErrorHistory() const
{
  if (metaModel_.getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "drawErrorHistory is only available for 1-d output dimension"
                                           << "but the current output dimension is " << metaModel_.getOutputDimension();
  const UnsignedInteger size = errorHistory_.getSize();
  if (!size)
    throw InvalidArgumentException(HERE) << "No error history available";

  Sample values(size, 2);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    values(i, 0) = i;
    values(i, 1) = errorHistory_[i];
  }
  Graph result("Error history", "iteration", "error", true, "upper right");
  Curve curve(values);
  curve.setColor("blue");
  result.add(curve);
  result.setIntegerXTick(true);
  return result;
}

END_NAMESPACE_OPENTURNS
