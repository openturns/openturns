//                                               -*- C++ -*-
/**
 *  @brief Functional chaos on field data
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

#include "openturns/FieldToPointFunctionalChaosAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
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
#include "openturns/JointDistribution.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToPointFunctionalChaosAlgorithm)

static const Factory<FieldToPointFunctionalChaosAlgorithm> Factory_FieldToPointFunctionalChaosAlgorithm;


/* Default constructor */
FieldToPointFunctionalChaosAlgorithm::FieldToPointFunctionalChaosAlgorithm()
  : FieldFunctionalChaosAlgorithm()
{
  // Nothing to do
}

/* Constructor with parameters */
FieldToPointFunctionalChaosAlgorithm::FieldToPointFunctionalChaosAlgorithm(const ProcessSample & inputProcessSample,
    const Sample & outputSample)
  : FieldFunctionalChaosAlgorithm()
  , inputProcessSample_(inputProcessSample)
  , outputSample_(outputSample)
{
  for (UnsignedInteger i = 0; i < inputProcessSample_.getDimension(); ++ i)
    blockIndices_.add(Indices(1, i));
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

/* Centered sample flag accessor */
void FieldToPointFunctionalChaosAlgorithm::setCenteredSample(const Bool centeredSample)
{
  centeredSample_ = centeredSample;
}

Bool FieldToPointFunctionalChaosAlgorithm::getCenteredSample() const
{
  return centeredSample_;
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
  JointDistribution::DistributionCollection marginals(dimension);
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

  JointDistribution distribution(marginals);
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

void FieldToPointFunctionalChaosAlgorithm::setBlockIndices(const Collection<Indices> & blockIndices)
{
  Indices flat;
  for (UnsignedInteger i = 0; i < blockIndices.getSize(); ++ i)
    flat.add(blockIndices[i]);
  if ((flat.getSize() != inputProcessSample_.getDimension()) || !flat.check(inputProcessSample_.getDimension()))
    throw InvalidArgumentException(HERE) << "Block indices does not match with input dimension";
  blockIndices_ = blockIndices;
}

/* Response surface computation */
void FieldToPointFunctionalChaosAlgorithm::run()
{
  ProcessSample inputProcessSample(inputProcessSample_);
  InverseTrendTransform inverseTransform;
  if (!centeredSample_)
  {
    inverseTransform = InverseTrendTransform(P1LagrangeEvaluation(inputProcessSample_.computeMean()), inputProcessSample_.getMesh());
    inputProcessSample = inverseTransform(inputProcessSample_);
  }

  // decompose input process
  Collection<KarhunenLoeveResult> klResultCollection(blockIndices_.getSize());
  decompose(inputProcessSample, klResultCollection);

  // the global input projection stacks projections of each block of variables
  const UnsignedInteger size = inputProcessSample_.getSize();
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
  result_.setMetaModel(metamodel);
  result_.setInputProcessSample(inputProcessSample_);
  result_.setInputBlockIndices(blockIndices_);
  result_.setOutputSample(outputSample_);
  result_.setModesSample(modesSample);
}

/* Method save() stores the object through the StorageManager */
void FieldToPointFunctionalChaosAlgorithm::save(Advocate & adv) const
{
  FieldFunctionalChaosAlgorithm::save(adv);
  adv.saveAttribute("inputProcessSample_", inputProcessSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("centeredSample_", centeredSample_);
}

/* Method load() reloads the object from the StorageManager */
void FieldToPointFunctionalChaosAlgorithm::load(Advocate & adv)
{
  FieldFunctionalChaosAlgorithm::load(adv);
  adv.loadAttribute("inputProcessSample_", inputProcessSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("centeredSample_", centeredSample_);
}

END_NAMESPACE_OPENTURNS
