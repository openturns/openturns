//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions
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
#include <cstdlib>
#include <map>

#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/FixedStrategy.hxx"
#include "openturns/CleaningStrategy.hxx"
#include "openturns/FixedExperiment.hxx"
#include "openturns/LeastSquaresStrategy.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/LeastSquaresMetaModelSelectionFactory.hxx"
#include "openturns/LARS.hxx"
#include "openturns/KFold.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/HypothesisTest.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Function> FunctionCollection;

CLASSNAMEINIT(FunctionalChaosAlgorithm)

static const Factory<FunctionalChaosAlgorithm> Factory_FunctionalChaosAlgorithm;


/* Default constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm()
  : MetaModelAlgorithm()
  , adaptiveStrategy_(FixedStrategy(OrthogonalProductPolynomialFactory(), 0))
  , projectionStrategy_(LeastSquaresStrategy())
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Nothing to do
}


/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Function & model,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy,
    const ProjectionStrategy & projectionStrategy)
  : MetaModelAlgorithm( distribution, model )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(projectionStrategy)
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Nothing to do
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Sample & inputSample,
    const Sample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy,
    const ProjectionStrategy & projectionStrategy)
  : MetaModelAlgorithm(distribution, DatabaseFunction(inputSample, outputSample))
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(projectionStrategy)
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  // Overwrite the content of the projection strategy with the given data
  projectionStrategy_.setMeasure(UserDefined(inputSample));
  projectionStrategy_.setExperiment(FixedExperiment(inputSample));
  projectionStrategy_.setWeights(Point(inputSample.getSize(), 1.0 / inputSample.getSize()));
  projectionStrategy_.setInputSample(inputSample);
  projectionStrategy_.setOutputSample(outputSample);
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy,
    const ProjectionStrategy & projectionStrategy)
  : MetaModelAlgorithm(distribution, DatabaseFunction(inputSample, outputSample))
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(projectionStrategy)
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  // Overwrite the content of the projection strategy with the given data
  projectionStrategy_.setMeasure(UserDefined(inputSample));
  projectionStrategy_.setExperiment(FixedExperiment(inputSample));
  projectionStrategy_.setWeights(weights);
  projectionStrategy_.setInputSample(inputSample);
  projectionStrategy_.setOutputSample(outputSample);
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Function & model,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy)
  : MetaModelAlgorithm( distribution, model )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(LeastSquaresStrategy())
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Nothing to do
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Sample & inputSample,
    const Sample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy)
  : MetaModelAlgorithm(distribution, DatabaseFunction(inputSample, outputSample))
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(LeastSquaresStrategy(inputSample, outputSample))
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
}

Distribution FunctionalChaosAlgorithm::BuildDistribution(const Sample & inputSample)
{
  // Recover the distribution, taking into account that we look for performance
  // so we avoid to rebuild expensive distributions as much as possible
  const UnsignedInteger inputDimension = inputSample.getDimension();
  // For the dependence structure, we use the Spearman independence test to decide between an independent and a Normal copula.
  Bool isIndependent = true;
  for (UnsignedInteger j = 0; j < inputDimension && isIndependent; ++ j)
  {
    const Sample marginalJ = inputSample.getMarginal(j);
    for (UnsignedInteger i = j + 1; i < inputDimension && isIndependent; ++ i)
    {
      TestResult testResult(HypothesisTest::Spearman(inputSample.getMarginal(i), marginalJ));
      isIndependent = isIndependent && testResult.getBinaryQualityMeasure();
    }
  }
  Collection< Distribution > marginals(inputDimension);
  // The strategy for the marginals is to find the best continuous 1-d parametric model else fallback to a kernel smoothing
  KernelSmoothing ks;
  Collection< DistributionFactory > factories(DistributionFactory::GetContinuousUniVariateFactories());
  const Description inputDescription(inputSample.getDescription());
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    TestResult bestResult;
    // Here we remove the duplicate entries in the marginal sample as we are suppose to have a continuous distribution. The duplicates are mostly due to truncation in the file export.
    const Sample marginalSample(inputSample.getMarginal(i).sortUnique());
    Collection<Distribution> possibleDistributions(0);
    for (UnsignedInteger j = 0; j < factories.getSize(); ++j)
      try
      {
        possibleDistributions.add(factories[j].build(marginalSample));
      }
      catch (...)
      {
        // Just skip the factories incompatible with the current marginal sample
      }
    const Distribution candidate(FittingTest::BestModelKolmogorov(marginalSample, possibleDistributions, bestResult));
    // This threshold is somewhat arbitrary. It is here to avoid expensive kernel smoothing.
    if (bestResult.getPValue() >= ResourceMap::GetAsScalar("FunctionalChaosAlgorithm-PValueThreshold")) marginals[i] = candidate;
    else marginals[i] = ks.build(marginalSample);
    marginals[i].setDescription(Description(1, inputDescription[i]));
  }

  ComposedDistribution distribution(marginals);
  if (!isIndependent)
    distribution.setCopula(NormalCopulaFactory().build(inputSample));
  return distribution;
}


/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Sample & inputSample,
    const Sample & outputSample)
  : MetaModelAlgorithm(Distribution(), DatabaseFunction(inputSample, outputSample))
  , adaptiveStrategy_()
  , projectionStrategy_()
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  // Recover the distribution
  LOGINFO("In FunctionalChaosAlgorithm, identify marginal distribution");
  setDistribution(BuildDistribution(inputSample));
  const UnsignedInteger inputDimension = inputSample.getDimension();
  Collection< OrthogonalUniVariatePolynomialFamily > polynomials(inputDimension);
  for (UnsignedInteger i = 0; i < inputDimension; ++ i)
  {
    polynomials[i] = StandardDistributionPolynomialFactory(getDistribution().getMarginal(i));
  }

  const HyperbolicAnisotropicEnumerateFunction enumerate(inputDimension, ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-QNorm" ));
  OrthogonalProductPolynomialFactory basis(polynomials, enumerate);
  const UnsignedInteger maximumTotalDegree = ResourceMap::GetAsUnsignedInteger( "FunctionalChaosAlgorithm-MaximumTotalDegree" );
  // For small sample size, use sparse regression
  LOGINFO("In FunctionalChaosAlgorithm, select adaptive strategy");
  if (inputSample.getSize() < ResourceMap::GetAsUnsignedInteger( "FunctionalChaosAlgorithm-SmallSampleSize" ))
  {
    projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample, LeastSquaresMetaModelSelectionFactory(LARS(), KFold()));
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm, selected a sparse chaos expansion based on LARS and KFold for a total degree of " << maximumTotalDegree);
  } // Small sample
  else if (inputSample.getSize() < ResourceMap::GetAsUnsignedInteger( "FunctionalChaosAlgorithm-LargeSampleSize" ))
  {
    projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample, LeastSquaresMetaModelSelectionFactory(LARS(), CorrectedLeaveOneOut()));
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm, selected a sparse chaos expansion based on LARS and CorrectedLeaveOneOut for a total degree of " << maximumTotalDegree);
  } // Medium sample
  else
  {
    projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample);
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm, selected a chaos expansion based on FixedStrategy for a total degree of " << maximumTotalDegree);
  } // Large sample
  const UnsignedInteger totalSize = enumerate.getStrataCumulatedCardinal(maximumTotalDegree);
  adaptiveStrategy_ = FixedStrategy(basis, totalSize);
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy)
  : MetaModelAlgorithm(distribution, DatabaseFunction(inputSample, outputSample))
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(LeastSquaresStrategy(inputSample, weights, outputSample))
  , maximumResidual_(ResourceMap::GetAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
}


/* Virtual constructor */
FunctionalChaosAlgorithm * FunctionalChaosAlgorithm::clone() const
{
  return new FunctionalChaosAlgorithm(*this);
}


/* String converter */
String FunctionalChaosAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* Maximum residual accessors */
void FunctionalChaosAlgorithm::setMaximumResidual(Scalar residual)
{
  maximumResidual_ = residual;
}

Scalar FunctionalChaosAlgorithm::getMaximumResidual() const
{
  return maximumResidual_;
}

/* Projection strategy accessor */
void FunctionalChaosAlgorithm::setProjectionStrategy(const ProjectionStrategy & projectionStrategy)
{
  projectionStrategy_ = projectionStrategy;
}

ProjectionStrategy FunctionalChaosAlgorithm::getProjectionStrategy() const
{
  return projectionStrategy_;
}

AdaptiveStrategy FunctionalChaosAlgorithm::getAdaptiveStrategy() const
{
  return adaptiveStrategy_;
}

/* Computes the functional chaos */
void FunctionalChaosAlgorithm::run()
{
  const UnsignedInteger outputDimension = model_.getOutputDimension();

  // Get the measure upon which the orthogonal basis is built
  const OrthogonalBasis basis(adaptiveStrategy_.getImplementation()->basis_);
  const Distribution measure(basis.getMeasure());
  // Correct the measure of the projection strategy if no input sample
  const Bool databaseProjection = projectionStrategy_.getInputSample().getSize() > 0;
  if (!databaseProjection) projectionStrategy_.setMeasure(measure);

  // First, compute all the parts that are independent of the marginal output
  // Create the isoprobabilistic transformation
  // We have two distributions here:
  // + The distribution of the input, called distribution_
  // + The distribution defining the inner product in basis, called measure
  // The projection is done on the basis, ie wrt measure_, so we have to
  // introduce an isoprobabilistic transformation that maps distribution_ onto
  // measure
  //
  const DistributionTransformation transformation(distribution_, measure);
  transformation_ = transformation;
  inverseTransformation_ = transformation.inverse();

  // Build the composed model g = f o T^{-1}, which is a function of Z so it can be decomposed upon an orthonormal basis based on Z distribution
  const Bool noTransformation = (measure == distribution_);
  if (noTransformation) composedModel_ = model_;
  else composedModel_ = ComposedFunction(model_, inverseTransformation_);
  // If the input and output databases have already been given to the projection strategy, transport them to the measure space
  const Sample initialInputSample(projectionStrategy_.getInputSample());
  if (databaseProjection && !noTransformation)
  {
    LOGINFO("Transform the input sample in the measure space");
    const Sample transformedSample(transformation_(initialInputSample));
    projectionStrategy_.setInputSample(transformedSample);
  }
  // Second, compute the results for each marginal output and merge
  // these marginal results.
  // As all the components have been projected using the same basis,
  // the representation is a linear combination of selected multivariate
  // polynomials with vector coefficients
  // We build the coefficients of the combination. As some indices may be
  // missing, we have to take care of the different sparsity patterns
  Point residuals(outputDimension);
  Point relativeErrors(outputDimension);
  std::map<UnsignedInteger, Point> coefficientsMap;
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
  {
    LOGINFO(OSS() << "Work on output marginal " << outputIndex << " over " << outputDimension - 1);
    Indices marginalIndices;
    Point marginalAlpha_k;
    Scalar marginalResidual = -1.0;
    Scalar marginalRelativeError = -1.0;
    // Compute the indices, the coefficients, the residual and the relative error of the current marginal output
    runMarginal(outputIndex, marginalIndices, marginalAlpha_k, marginalResidual, marginalRelativeError);
    residuals[outputIndex] = marginalResidual;
    relativeErrors[outputIndex] = marginalRelativeError;
    for (UnsignedInteger j = 0; j < marginalIndices.getSize(); ++j)
    {
      // Deal only with non-zero coefficients
      const Scalar marginalAlpha_kj = marginalAlpha_k[j];
      // To avoid -0.0
      if (std::abs(marginalAlpha_kj) != 0.0)
      {
        // Current index in the decomposition of the current marginal output
        const UnsignedInteger index = marginalIndices[j];
        // If the current index is not in the map, create it
        if (coefficientsMap.find(index) == coefficientsMap.end()) coefficientsMap[index] = Point(outputDimension, 0.0);
        // Add the current scalar coefficient to the corresponding component of the vectorial coefficient
        coefficientsMap[index][outputIndex] = marginalAlpha_kj;
      }
    } // Loop over the marginal indices
  } // Loop over the output dimension
  // At this point, the map contains all the associations (index, vector coefficient). It remains to present these data into the proper form and to build the associated partial basis
  std::map<UnsignedInteger, Point>::iterator iter;
  // Full set of indices
  Indices I_k(0);
  // Full set of vectorial coefficients
  Sample alpha_k(0, outputDimension);
  // Full set of partial basis functions.
  FunctionCollection Psi_k(0);
  for (iter = coefficientsMap.begin(); iter != coefficientsMap.end(); ++iter)
  {
    const UnsignedInteger i = iter->first;
    const Point currentCoefficient(iter->second);
    I_k.add(i);
    alpha_k.add(currentCoefficient);
    // We could reuse the function
    Psi_k.add(basis.build(i));
  }
  // Build the result
  result_ = FunctionalChaosResult(model_, distribution_, transformation_, inverseTransformation_, composedModel_, basis, I_k, alpha_k, Psi_k, residuals, relativeErrors);
}

/* Marginal computation */
void FunctionalChaosAlgorithm::runMarginal(const UnsignedInteger marginalIndex,
    Indices & indices,
    Point & coefficients,
    Scalar & residual,
    Scalar & relativeError)
{
  // Initialize the projection basis Phi_k_p_ and I_p_
  LOGINFO("Compute the initial basis");
  adaptiveStrategy_.computeInitialBasis();
  do
  {
    LOGINFO("Compute the coefficients");
    projectionStrategy_.computeCoefficients(composedModel_, adaptiveStrategy_.getImplementation()->Psi_, adaptiveStrategy_.getImplementation()->I_p_, adaptiveStrategy_.getImplementation()->addedPsi_k_ranks_, adaptiveStrategy_.getImplementation()->conservedPsi_k_ranks_, adaptiveStrategy_.getImplementation()->removedPsi_k_ranks_, marginalIndex);
    // The basis is adapted under the following conditions:
    // + the current residual is small enough
    // + the adaptive strategy has no more vector to propose
    if (projectionStrategy_.getResidual() < maximumResidual_)
    {
      LOGINFO("Stop on small residual");
      indices = adaptiveStrategy_.getImplementation()->I_p_;
      coefficients = projectionStrategy_.getCoefficients();
      residual = projectionStrategy_.getResidual();
      relativeError = projectionStrategy_.getRelativeError();
      return;
    }
    LOGINFO("Adapt the basis");
    adaptiveStrategy_.updateBasis(projectionStrategy_.getImplementation()->alpha_k_p_, projectionStrategy_.getImplementation()->residual_p_, projectionStrategy_.getImplementation()->relativeError_p_);
  }
  // Check if there is still something to do
  while ((adaptiveStrategy_.getImplementation()->addedPsi_k_ranks_.getSize() > 0) || (adaptiveStrategy_.getImplementation()->removedPsi_k_ranks_.getSize() > 0));

  LOGINFO("No more basis adaptation");
  indices = adaptiveStrategy_.getImplementation()->I_p_;
  coefficients = projectionStrategy_.getCoefficients();
  residual = projectionStrategy_.getResidual();
  relativeError = projectionStrategy_.getRelativeError();
}


/* Get the functional chaos result */
FunctionalChaosResult FunctionalChaosAlgorithm::getResult() const
{
  return result_;
}


Sample FunctionalChaosAlgorithm::getInputSample() const
{
  return projectionStrategy_.getInputSample();
}


Sample FunctionalChaosAlgorithm::getOutputSample() const
{
  return projectionStrategy_.getOutputSample();
}


/* Method save() stores the object through the StorageManager */
void FunctionalChaosAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "maximumResidual_", maximumResidual_ );
  adv.saveAttribute( "result_", result_ );
}


/* Method load() reloads the object from the StorageManager */
void FunctionalChaosAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "maximumResidual_", maximumResidual_ );
  adv.loadAttribute( "result_", result_ );
}



END_NAMESPACE_OPENTURNS

