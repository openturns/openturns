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

#include <limits>
#include "openturns/FieldToPointFunctionalChaosAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveSVDAlgorithm.hxx"
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/KarhunenLoeveProjection.hxx"
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/FixedStrategy.hxx"
#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/LARS.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/LeastSquaresStrategy.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"
#include "openturns/LeastSquaresMetaModelSelectionFactory.hxx"
#include "openturns/FieldToPointConnection.hxx"
#include "openturns/NormalityTest.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/EmpiricalBernsteinCopula.hxx"
#include "openturns/BlockIndependentDistribution.hxx"
#include "openturns/HypothesisTest.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToPointFunctionalChaosAlgorithm)

static const Factory<FieldToPointFunctionalChaosAlgorithm> Factory_FieldToPointFunctionalChaosAlgorithm;


/* Default constructor */
FieldToPointFunctionalChaosAlgorithm::FieldToPointFunctionalChaosAlgorithm()
  : PersistentObject()
  , nbModes_(std::numeric_limits<UnsignedInteger>::max())
{
  // Nothing to do
}

/* Constructor with parameters */
FieldToPointFunctionalChaosAlgorithm::FieldToPointFunctionalChaosAlgorithm(const ProcessSample & inputProcessSample,
    const Sample & outputSample)
  : PersistentObject()
  , inputProcessSample_(inputProcessSample)
  , outputSample_(outputSample)
  , nbModes_(std::numeric_limits<UnsignedInteger>::max())
{
  for (UnsignedInteger i = 0; i < inputProcessSample_.getDimension(); ++ i)
    blockIndices_.add(Indices(1, i));

  setRecompress(ResourceMap::GetAsBool("FieldToPointFunctionalChaosAlgorithm-DefaultRecompress"));
}

/* Virtual constructor */
FieldToPointFunctionalChaosAlgorithm * FieldToPointFunctionalChaosAlgorithm::clone() const
{
  return new FieldToPointFunctionalChaosAlgorithm(*this);
}


ProcessSample FieldToPointFunctionalChaosAlgorithm::getInputProcessSample() const
{
  return inputProcessSample_;
}


Sample FieldToPointFunctionalChaosAlgorithm::getOutputSample() const
{
  return outputSample_;
}

/* Block indices accessors */
Collection<Indices> FieldToPointFunctionalChaosAlgorithm::getBlockIndices() const
{
  return blockIndices_;
}

void FieldToPointFunctionalChaosAlgorithm::setBlockIndices(const Collection<Indices> & blockIndices)
{
  Indices flat;
  for (UnsignedInteger i = 0; i < blockIndices.getSize(); ++ i)
    flat.add(blockIndices[i]);
  if ((flat.getSize() != inputProcessSample_.getDimension()) || !flat.check(inputProcessSample_.getDimension()))
    throw InvalidArgumentException(HERE) << "Block indices does not match with input dimension";
  blockIndices_ = blockIndices;
}


/* Threshold accessors */
Scalar FieldToPointFunctionalChaosAlgorithm::getThreshold() const
{
  return threshold_;
}

void FieldToPointFunctionalChaosAlgorithm::setThreshold(const Scalar threshold)
{
  if (!(threshold <= 1.0 && threshold >= 0.0))
    throw InvalidArgumentException(HERE) << "threshold must be between 0.0 and 1.0 but is " << threshold;

  threshold_ = threshold;
}

/* Number of modes accessors */
UnsignedInteger FieldToPointFunctionalChaosAlgorithm::getNbModes() const
{
  return nbModes_;
}

void FieldToPointFunctionalChaosAlgorithm::setNbModes(const UnsignedInteger nbModes)
{
  nbModes_ = nbModes;
}

/* Recompression flag accessors */
Bool FieldToPointFunctionalChaosAlgorithm::getRecompress() const
{
  return recompress_;
}

void FieldToPointFunctionalChaosAlgorithm::setRecompress(const Bool recompress)
{
  recompress_ = recompress;
}

/* Centered sample flag accessor */
void FieldToPointFunctionalChaosAlgorithm::setCenteredSample(const Bool centeredSample)
{
  centeredSample_ = centeredSample;
}

Bool FieldToPointFunctionalChaosAlgorithm::getCenteredSample() const
{
  return centeredSample_;
}

/* Result accessor */
FieldFunctionalChaosResult FieldToPointFunctionalChaosAlgorithm::getResult() const
{
  return result_;
}

/* String converter */
String FieldToPointFunctionalChaosAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}


// dedicated function to stack projections
class StackedProjectionFunction : public FieldToPointFunctionImplementation
{
public:
  StackedProjectionFunction(const Collection<FieldToPointFunction> & projectionCollection,
                            const Collection<Indices> & blockIndices)
    : FieldToPointFunctionImplementation()
    , projectionCollection_(projectionCollection)
    , blockIndices_(blockIndices)
  {
    if (!projectionCollection.getSize())
      throw InvalidArgumentException(HERE) << "projectionCollection should not be empty";
    if (projectionCollection.getSize() != blockIndices.getSize())
      throw InvalidArgumentException(HERE) << "projectionCollection size must match blockIndices size";
    inputMesh_ = projectionCollection[0].getInputMesh();
    Description inputDescription;
    Description outputDescription;
    for (UnsignedInteger i = 0; i < projectionCollection.getSize(); ++ i)
    {
      inputDescription.add(projectionCollection[i].getInputDescription());
      outputDescription.add(projectionCollection[i].getOutputDescription());
    }
    inputDimension_ = inputDescription.getSize();
    outputDimension_ = outputDescription.getSize();
    setInputDescription(inputDescription);
    setOutputDescription(outputDescription);
  }

  StackedProjectionFunction * clone() const override
  {
    return new StackedProjectionFunction(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return inputDimension_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return outputDimension_;
  }

  Point operator() (const Sample & inS) const override
  {
    Point Y;
    for (UnsignedInteger i = 0; i < projectionCollection_.getSize(); ++ i)
    {
      const Sample x(inS.getMarginal(blockIndices_[i]));
      const Point y(projectionCollection_[i](x));
      Y.add(y);
    }
    return Y;
  }

private:
  UnsignedInteger inputDimension_ = 0;
  UnsignedInteger outputDimension_ = 0;
  Collection<FieldToPointFunction> projectionCollection_;
  Collection<Indices> blockIndices_;
};


/* Build distribution of KL modes */
Distribution FieldToPointFunctionalChaosAlgorithm::BuildDistribution(const Sample & sample)
{
  // marginals: test Gaussian with fallback to histogram
  const UnsignedInteger dimension = sample.getDimension();
  ComposedDistribution::DistributionCollection marginals(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    const Sample marginalJ(sample.getMarginal(j));
    const TestResult testResult(NormalityTest::CramerVonMisesNormal(marginalJ));
    if (testResult.getBinaryQualityMeasure())
      marginals[j] = NormalFactory().build(marginalJ);
    else
      marginals[j] = HistogramFactory().build(marginalJ);
  }

  // dependence structure: test independence with fallback to beta copula
  Bool isIndependent = true;
  for (UnsignedInteger j = 0; j < dimension && isIndependent; ++ j)
  {
    const Sample marginalJ = sample.getMarginal(j);
    for (UnsignedInteger i = j + 1; i < dimension && isIndependent; ++ i)
    {
      TestResult testResult(HypothesisTest::Spearman(sample.getMarginal(i), marginalJ));
      isIndependent = isIndependent && testResult.getBinaryQualityMeasure();
    }
  }

  ComposedDistribution distribution(marginals);
  if (!isIndependent)
  {
    const String copulaType = ResourceMap::GetAsString("FieldToPointFunctionalChaosAlgorithm-CopulaType");
    if (copulaType == "Normal")
      distribution.setCopula(NormalCopulaFactory().build(sample));
    else if (copulaType == "Beta")
      distribution.setCopula(EmpiricalBernsteinCopula(sample, sample.getSize()));
    else
      throw InvalidArgumentException(HERE) << "Unknown copula type: " << copulaType;
  }
  return distribution;
}

/* Response surface computation */
void FieldToPointFunctionalChaosAlgorithm::run()
{
  const UnsignedInteger size = inputProcessSample_.getSize();
  Collection<KarhunenLoeveResult> klResultCollection(blockIndices_.getSize());
  Point eigenValues;

  ProcessSample inputProcessSample(inputProcessSample_);
  InverseTrendTransform inverseTransform;
  if (!centeredSample_)
  {
    inverseTransform = InverseTrendTransform(P1LagrangeEvaluation(inputProcessSample_.computeMean()), inputProcessSample_.getMesh());
    inputProcessSample = inverseTransform(inputProcessSample_);
  }

  for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
  {
    const ProcessSample inputProcessSampleI(inputProcessSample.getMarginal(blockIndices_[i]));
    const Bool centered = true;
    KarhunenLoeveSVDAlgorithm algo(inputProcessSampleI, threshold_, centered);
    algo.setNbModes(nbModes_);
    algo.run();
    eigenValues.add(algo.getResult().getEigenvalues());
    klResultCollection[i] = algo.getResult();
    LOGINFO(OSS() << "block #" << i << " indices=" << blockIndices_[i] << " ev size=" << algo.getResult().getEigenvalues().getDimension());
  }

  if (recompress_)
  {
    const Scalar cumulatedVariance = eigenValues.norm1();
    std::sort(eigenValues.begin(), eigenValues.end(), [](const Scalar a, const Scalar b)
    {
      return a > b;
    });
    const UnsignedInteger nbModesMax = std::min(nbModes_, eigenValues.getSize());
    // Find the cut-off in the eigenvalues
    UnsignedInteger K = 0;
    Scalar selectedVariance = 0.0; // sum of eigenvalues selected after cut-off is applied
    do
    {
      selectedVariance += eigenValues[K];
      ++ K;
    }
    while ((K < nbModesMax) && (selectedVariance < (1.0 - threshold_) * cumulatedVariance));
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
  Collection<Distribution> distributionBlocks(blockIndices_.getSize());
  Collection<FieldToPointFunction> projectionCollection(blockIndices_.getSize());
  for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
  {
    const KarhunenLoeveProjection projectionI(klResultCollection[i]);
    projectionCollection[i] = projectionI;
    const ProcessSample inputProcessSampleI(inputProcessSample.getMarginal(blockIndices_[i]));
    const Sample modesSampleI(projectionI(inputProcessSampleI));
    distributionBlocks[i] = BuildDistribution(modesSampleI);
    distributionBlocks[i].setDescription(Description::BuildDefault(modesSampleI.getDimension(), OSS() << "xsi^" << i << "_"));
    modesSample.stack(modesSampleI);
  }

  // build PCE expansion of projected modes vs output sample
  const BlockIndependentDistribution distribution(distributionBlocks);
  FunctionalChaosAlgorithm fce(modesSample, outputSample_, distribution);
  fce.run();
  const FunctionalChaosResult fceResult(fce.getResult());

  // compose input projection + FCE interpolation
  const FieldToPointFunction projection(StackedProjectionFunction(projectionCollection, blockIndices_));
  FieldToPointFunction metamodel(FieldToPointConnection(fceResult.getMetaModel(), projection));
  if (!centeredSample_)
    metamodel = FieldToPointConnection(metamodel, inverseTransform);

  // populate result
  result_ = FieldFunctionalChaosResult(klResultCollection, fceResult, Collection<KarhunenLoeveResult>());
  result_.setMetamodel(metamodel);
  result_.setInputProcessSample(inputProcessSample_);
  result_.setBlockIndices(blockIndices_);
  result_.setOutputSample(outputSample_);
  result_.setModesSample(modesSample);
}


/* Method save() stores the object through the StorageManager */
void FieldToPointFunctionalChaosAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputProcessSample_", inputProcessSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("blockIndices_", blockIndices_);
  adv.saveAttribute("centeredSample_", centeredSample_);
  adv.saveAttribute("threshold_", threshold_);
  adv.saveAttribute("nbModes_", nbModes_);
  adv.saveAttribute("recompress_", recompress_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void FieldToPointFunctionalChaosAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputProcessSample_", inputProcessSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("blockIndices_", blockIndices_);
  adv.loadAttribute("centeredSample_", centeredSample_);
  adv.loadAttribute("threshold_", threshold_);
  adv.loadAttribute("nbModes_", nbModes_);
  adv.loadAttribute("recompress_", recompress_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
