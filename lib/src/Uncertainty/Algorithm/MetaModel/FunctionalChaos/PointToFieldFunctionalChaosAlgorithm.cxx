//                                               -*- C++ -*-
/**
 *  @brief Functional chaos on field data
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

#include "openturns/PointToFieldFunctionalChaosAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveSVDAlgorithm.hxx"
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/KarhunenLoeveProjection.hxx"
#include "openturns/KarhunenLoeveLifting.hxx"
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/LeastSquaresExpansion.hxx"
#include "openturns/PointToFieldConnection.hxx"
#include "openturns/BlockIndependentDistribution.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToFieldFunctionalChaosAlgorithm)

static const Factory<PointToFieldFunctionalChaosAlgorithm> Factory_PointToFieldFunctionalChaosAlgorithm;


/* Default constructor */
PointToFieldFunctionalChaosAlgorithm::PointToFieldFunctionalChaosAlgorithm()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
PointToFieldFunctionalChaosAlgorithm::PointToFieldFunctionalChaosAlgorithm(const Sample & inputSample,
    const ProcessSample & outputProcessSample,
    const Distribution & distribution)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputProcessSample_(outputProcessSample)
  , distribution_(distribution)
{
  if (inputSample.getSize() != outputProcessSample.getSize())
    throw InvalidArgumentException(HERE) << "PointToFieldFunctionalChaosAlgorithm the input sample size (" << inputSample.getSize()
                                         << ") must match the output sample size ("<<outputProcessSample.getSize() << ")";
  if (inputSample.getDimension() != distribution.getDimension())
    throw InvalidArgumentException(HERE) << "PointToFieldFunctionalChaosAlgorithm the input sample dimension (" << inputSample.getDimension()
                                        << ") must match the input distribution dimension (" << distribution.getDimension() << ")";

  // default to one block for all output components
  Indices full(outputProcessSample_.getDimension());
  full.fill();
  blockIndices_.add(full);

  setRecompress(ResourceMap::GetAsBool("PointToFieldFunctionalChaosAlgorithm-DefaultRecompress"));
}

/* Virtual constructor */
PointToFieldFunctionalChaosAlgorithm * PointToFieldFunctionalChaosAlgorithm::clone() const
{
  return new PointToFieldFunctionalChaosAlgorithm(*this);
}

Sample PointToFieldFunctionalChaosAlgorithm::getInputSample() const
{
  return inputSample_;
}

ProcessSample PointToFieldFunctionalChaosAlgorithm::getOutputProcessSample() const
{
  return outputProcessSample_;
}

/* Block indices accessors */
Collection<Indices> PointToFieldFunctionalChaosAlgorithm::getBlockIndices() const
{
  return blockIndices_;
}

void PointToFieldFunctionalChaosAlgorithm::setBlockIndices(const Collection<Indices> & blockIndices)
{
  Indices flat;
  for (UnsignedInteger i = 0; i < blockIndices.getSize(); ++ i)
    flat.add(blockIndices[i]);
  if ((flat.getSize() != outputProcessSample_.getDimension()) || !flat.check(outputProcessSample_.getDimension()))
    throw InvalidArgumentException(HERE) << "Block indices does not match with output dimension";
  blockIndices_ = blockIndices;
}

/* Threshold accessors */
Scalar PointToFieldFunctionalChaosAlgorithm::getThreshold() const
{
  return threshold_;
}

void PointToFieldFunctionalChaosAlgorithm::setThreshold(const Scalar threshold)
{
  if (!(threshold <= 1.0 && threshold >= 0.0))
    throw InvalidArgumentException(HERE) << "threshold must be between 0.0 and 1.0 but is " << threshold;

  threshold_ = threshold;
}

/* Number of modes accessors */
UnsignedInteger PointToFieldFunctionalChaosAlgorithm::getNbModes() const
{
  return nbModes_;
}

void PointToFieldFunctionalChaosAlgorithm::setNbModes(const UnsignedInteger nbModes)
{
  nbModes_ = nbModes;
}

/* Recompression flag accessors */
Bool PointToFieldFunctionalChaosAlgorithm::getRecompress() const
{
  return recompress_;
}

void PointToFieldFunctionalChaosAlgorithm::setRecompress(const Bool recompress)
{
  recompress_ = recompress;
}

/* Result accessor */
FieldFunctionalChaosResult PointToFieldFunctionalChaosAlgorithm::getResult() const
{
  return result_;
}

/* String converter */
String PointToFieldFunctionalChaosAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

// dedicated function to stack liftings
class StackedLiftingFunction : public PointToFieldFunctionImplementation
{
public:
  explicit StackedLiftingFunction(const Collection<PointToFieldFunction> & liftingCollection)
    : PointToFieldFunctionImplementation()
    , liftingCollection_(liftingCollection)
  {
    if (!liftingCollection.getSize())
      throw InvalidArgumentException(HERE) << "liftingCollection should not be empty";

    outputMesh_ = liftingCollection[0].getOutputMesh();
    Description inputDescription;
    Description outputDescription;
    for (UnsignedInteger i = 0; i < liftingCollection.getSize(); ++ i)
    {
      inputDescription.add(liftingCollection[i].getInputDescription());
      outputDescription.add(liftingCollection[i].getOutputDescription());
    }
    
    inputDimension_ = inputDescription.getSize();
    outputDimension_ = outputDescription.getSize();
    setInputDescription(inputDescription);
    setOutputDescription(outputDescription);
  }

  StackedLiftingFunction * clone() const override
  {
    return new StackedLiftingFunction(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return inputDimension_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return outputDimension_;
  }

  Sample operator() (const Point & inP) const override
  {
    Sample Y(getOutputMesh().getVerticesNumber(), 0);
    UnsignedInteger shift = 0;
    for (UnsignedInteger i = 0; i < liftingCollection_.getSize(); ++ i)
    {
      Point inPI(liftingCollection_[i].getInputDimension()); // subset of modes coefficients inP for block I
      std::copy(inP.begin() + shift, inP.begin() + shift + inPI.getDimension(), inPI.begin());
      shift += inPI.getDimension();
      const Sample y(liftingCollection_[i](inPI));
      Y.stack(y);
    }
    return Y;
  }

private:
  UnsignedInteger inputDimension_ = 0;
  UnsignedInteger outputDimension_ = 0;
  Collection<PointToFieldFunction> liftingCollection_;
};

/* Response surface computation */
void PointToFieldFunctionalChaosAlgorithm::run()
{
  const UnsignedInteger size = outputProcessSample_.getSize();
  Collection<KarhunenLoeveResult> klResultCollection(blockIndices_.getSize());
  Point eigenValues;

  const ProcessSample outputProcessSample(outputProcessSample_);

  for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
  {
    const ProcessSample outputProcessSampleI(outputProcessSample.getMarginal(blockIndices_[i]));
    const Bool centered = false;
    KarhunenLoeveSVDAlgorithm algo(outputProcessSampleI, threshold_, centered);
    algo.setNbModes(nbModes_);
    algo.run();
    eigenValues.add(algo.getResult().getEigenvalues());
    klResultCollection[i] = algo.getResult();
    LOGINFO(OSS() << "block #" << i << " indices=" << blockIndices_[i] << " ev size=" << algo.getResult().getEigenvalues().getDimension());
  }

  if (recompress_)
  {
    const Scalar cumulatedVariance = eigenValues.norm1();
    std::sort(eigenValues.begin(), eigenValues.end(), [](const Scalar a, const Scalar b) { return a > b; });
    const UnsignedInteger nbModesMax = std::min(nbModes_, eigenValues.getSize());
    // Find the cut-off in the eigenvalues
    UnsignedInteger K = 0;
    Scalar selectedVariance = 0.0; // sum of eigenvalues selected after cut-off is applied
    do
    {
      selectedVariance += eigenValues[K];
      ++ K;
    }
    while ((K < nbModesMax-1) && (selectedVariance < (1.0 - threshold_) * cumulatedVariance));
    LOGINFO(OSS() << "Selected " << K << " eigenvalues out of " << eigenValues.getSize() << " computed");

    const Scalar lambdaCut = eigenValues[K];
    for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
    {
      UnsignedInteger Ki = 1;// retain at least 1 mode
      Point eigenValuesI(klResultCollection[i].getEigenvalues());
      Scalar selectedVarianceI = eigenValuesI[0];
      const Scalar cumulatedVarianceI = eigenValuesI.norm1();
      while ((Ki < eigenValuesI.getSize()) && (eigenValuesI[Ki] > lambdaCut))
      {
        selectedVarianceI += eigenValuesI[Ki];
        ++ Ki;
      }
      eigenValuesI.resize(Ki);
      Collection<Function> modesI(klResultCollection[i].getModes());
      modesI.resize(Ki);
      CovarianceModel covarianceI(RankMCovarianceModel(eigenValuesI, modesI));
      ProcessSample modesAsProcessSampleI(klResultCollection[i].getModesAsProcessSample());
      modesAsProcessSampleI.erase(Ki, modesAsProcessSampleI.getSize());
      MatrixImplementation projectionMatrixI(*klResultCollection[i].getProjectionMatrix().getImplementation());
      projectionMatrixI.resize(Ki, projectionMatrixI.getNbColumns());

      klResultCollection[i] = KarhunenLoeveResult(covarianceI, klResultCollection[i].getThreshold(), eigenValuesI, modesI, modesAsProcessSampleI, projectionMatrixI, selectedVarianceI / cumulatedVarianceI);
    }
  }

  // the global input projection stacks projections of each block of variables
  Sample modesSample(size, 0);
  Collection<PointToFieldFunction> liftingCollection(blockIndices_.getSize());
  for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
  {
    const KarhunenLoeveProjection projectionI(klResultCollection[i]);
    const KarhunenLoeveLifting liftingI(klResultCollection[i]);
    liftingCollection[i] = liftingI;
    const ProcessSample outputProcessSampleI(outputProcessSample.getMarginal(blockIndices_[i]));
    const Sample modesSampleI(projectionI(outputProcessSampleI));
    modesSample.stack(modesSampleI);
  }

  // build PCE expansion of projected modes vs output sample
  FunctionalChaosResult fceResult;
  const String expansion = ResourceMap::GetAsString("PointToFieldFunctionalChaosAlgorithm-Expansion");
  if (expansion == "LeastSquaresExpansion")
  {
    LeastSquaresExpansion fce(inputSample_, modesSample, distribution_);
    fce.run();
    fceResult = fce.getResult();
  }
  else if (expansion == "FunctionalChaosAlgorithm")
  {
    FunctionalChaosAlgorithm fce(inputSample_, modesSample, distribution_);
    fce.run();
    fceResult = fce.getResult();
  }
  else
    throw InvalidArgumentException(HERE) << "Unknown expansion: " << expansion;

  // compose FCE interpolation + output lifting
  const StackedLiftingFunction stackedLifting(liftingCollection);
  const PointToFieldFunction lifting(stackedLifting);
  PointToFieldFunction metamodel(PointToFieldConnection(lifting, fceResult.getMetaModel()));

  // populate result
  result_ = FieldFunctionalChaosResult(Collection<KarhunenLoeveResult>(), fceResult, klResultCollection);
  result_.setMetaModel(metamodel);
  result_.setInputSample(inputSample_);
  result_.setOutputBlockIndices(blockIndices_);
  result_.setOutputProcessSample(outputProcessSample_);
  result_.setModesSample(modesSample);
}


/* Method save() stores the object through the StorageManager */
void PointToFieldFunctionalChaosAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputProcessSample_", outputProcessSample_);
  adv.saveAttribute("blockIndices_", blockIndices_);
  adv.saveAttribute("threshold_", threshold_);
  adv.saveAttribute("nbModes_", nbModes_);
  adv.saveAttribute("recompress_", recompress_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void PointToFieldFunctionalChaosAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputProcessSample_", outputProcessSample_);
  adv.loadAttribute("blockIndices_", blockIndices_);
  adv.loadAttribute("threshold_", threshold_);
  adv.loadAttribute("nbModes_", nbModes_);
  adv.loadAttribute("recompress_", recompress_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
