//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions
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
#include <cstdlib>
#include <map>

#include "openturns/FunctionalChaosAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalMathFunctionImplementation.hxx"
#include "openturns/DatabaseNumericalMathEvaluationImplementation.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"
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

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Distribution> DistributionCollection;
typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;

CLASSNAMEINIT(FunctionalChaosAlgorithm);

static const Factory<FunctionalChaosAlgorithm> Factory_FunctionalChaosAlgorithm;


/* Default constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm()
  : MetaModelAlgorithm()
  , adaptiveStrategy_(FixedStrategy(OrthogonalProductPolynomialFactory(), 0))
  , projectionStrategy_(LeastSquaresStrategy())
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Nothing to do
}


/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalMathFunction & model,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy,
    const ProjectionStrategy & projectionStrategy)
  : MetaModelAlgorithm( distribution, model )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(projectionStrategy)
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Nothing to do
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy,
    const ProjectionStrategy & projectionStrategy)
  : MetaModelAlgorithm( distribution, NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())) )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(projectionStrategy)
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  // Overwrite the content of the projection strategy with the given data
  projectionStrategy_.getImplementation()->inputSample_ = inputSample;
  projectionStrategy_.getImplementation()->measure_ = UserDefined(inputSample);
  projectionStrategy_.getImplementation()->weights_ = NumericalPoint(inputSample.getSize(), 1.0 / inputSample.getSize());
  projectionStrategy_.getImplementation()->weightedExperiment_ = FixedExperiment(inputSample);
  projectionStrategy_.getImplementation()->outputSample_ = outputSample;
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalSample & inputSample,
    const NumericalPoint & weights,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy,
    const ProjectionStrategy & projectionStrategy)
  : MetaModelAlgorithm( distribution, NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())) )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(projectionStrategy)
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  // Overwrite the content of the projection strategy with the given data
  projectionStrategy_.getImplementation()->inputSample_ = inputSample;
  projectionStrategy_.getImplementation()->measure_ = UserDefined(inputSample);
  projectionStrategy_.getImplementation()->weights_ = weights;
  projectionStrategy_.getImplementation()->weightedExperiment_ = FixedExperiment(inputSample, weights);
  projectionStrategy_.getImplementation()->outputSample_ = outputSample;
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalMathFunction & model,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy)
  : MetaModelAlgorithm( distribution, model )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(LeastSquaresStrategy())
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Nothing to do
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy)
  : MetaModelAlgorithm( distribution, NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())) )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(LeastSquaresStrategy(inputSample, outputSample))
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalSample & inputSample,
    const NumericalSample & outputSample)
  : MetaModelAlgorithm( Distribution(), NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())) )
  , adaptiveStrategy_()
  , projectionStrategy_()
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  // Recover the distribution, taking into account that we look for performance
  // so we avoid to rebuild expansive distributions as much as possible
  const UnsignedInteger inputDimension(inputSample.getDimension());
  Collection< Distribution > marginals(inputDimension);
  Collection< OrthogonalUniVariatePolynomialFamily > polynomials(inputDimension);
  // The strategy is to test first a Uniform distribution, then a Normal distribution, then a kernel smoothing for the marginals
  KernelSmoothing ks;
  Collection< DistributionFactory > factories(DistributionFactory::GetContinuousUniVariateFactories());
  LOGINFO("In FunctionalChaosAlgorithm, identify marginal distributions");
  const Description inputDescription(inputSample.getDescription());
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    TestResult bestResult;
    // Here we remove the duplicate entries in the marginal sample using the automatic compaction of the support in the UserDefined class
    const NumericalSample marginalSample(UserDefined(inputSample.getMarginal(i)).getSupport());
    const Distribution candidate(FittingTest::BestModelKolmogorov(marginalSample, factories, bestResult));
    // This threshold is somewhat arbitrary. It is here to avoid expansive kernel smoothing.
    if (bestResult.getPValue() > ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-PValueThreshold")) marginals[i] = candidate;
    else marginals[i] = ks.build(marginalSample.getMarginal(i));
    marginals[i].setDescription(Description(1, inputDescription[i]));
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm constructor, selected distribution for marginal " << i << "=" << marginals[i]);
    polynomials[i] = StandardDistributionPolynomialFactory(marginals[i]);
  }
  // For the dependence structure, we test first the independent copula, then the normal copula, but not a non-parametric copula as the penalty on the meta-model evaluation speed is most of the time prohibitive
  setDistribution(ComposedDistribution(marginals, NormalCopulaFactory().build(inputSample)));
  const HyperbolicAnisotropicEnumerateFunction enumerate(inputDimension, ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-QNorm" ));
  OrthogonalProductPolynomialFactory basis(polynomials, enumerate);
  const UnsignedInteger maximumTotalDegree(ResourceMap::GetAsUnsignedInteger( "FunctionalChaosAlgorithm-MaximumTotalDegree" ));
  // For small sample size, use sparse regression
  LOGINFO("In FunctionalChaosAlgorithm, select adaptive strategy");
  if (inputSample.getSize() < ResourceMap::GetAsUnsignedInteger( "FunctionalChaosAlgorithm-SmallSampleSize" ))
  {
    projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample, LeastSquaresMetaModelSelectionFactory(LARS(), KFold()));
    // projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample, LeastSquaresMetaModelSelectionFactory(LAR(), CorrectedLeaveOneOut()));
    adaptiveStrategy_ = FixedStrategy(basis, enumerate.getStrataCumulatedCardinal(maximumTotalDegree));
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm, selected a sparse chaos expansion based on LAR and KFold for a total degree of " << maximumTotalDegree);
  } // Small sample
  else if (inputSample.getSize() < ResourceMap::GetAsUnsignedInteger( "FunctionalChaosAlgorithm-LargeSampleSize" ))
  {
    projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample);
    const UnsignedInteger totalSize(enumerate.getStrataCumulatedCardinal(maximumTotalDegree));
    const UnsignedInteger basisSize(std::min(totalSize, maximumTotalDegree * maximumTotalDegree * inputSample.getDimension()));
    CleaningStrategy cleaning(basis, totalSize);
    cleaning.setMaximumSize(basisSize);
    adaptiveStrategy_ = cleaning;
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm, selected a constrained chaos expansion based on CleaningStrategy for a total degree of " << maximumTotalDegree << " and a maximum number of terms of " << basisSize);
  } // Medium sample
  else
  {
    projectionStrategy_ = LeastSquaresStrategy(inputSample, outputSample);
    const UnsignedInteger totalSize(enumerate.getStrataCumulatedCardinal(maximumTotalDegree));
    adaptiveStrategy_ = FixedStrategy(basis, totalSize);
    LOGINFO(OSS() << "In FunctionalChaosAlgorithm, selected a chaos expansion based on FixedStrategy for a total degree of " << maximumTotalDegree);
  } // Large sample
}

/* Constructor */
FunctionalChaosAlgorithm::FunctionalChaosAlgorithm(const NumericalSample & inputSample,
    const NumericalPoint & weights,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const AdaptiveStrategy & adaptiveStrategy)
  : MetaModelAlgorithm( distribution, NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())) )
  , adaptiveStrategy_(adaptiveStrategy)
  , projectionStrategy_(LeastSquaresStrategy(inputSample, weights, outputSample))
  , maximumResidual_(ResourceMap::GetAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual" ))
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
void FunctionalChaosAlgorithm::setMaximumResidual(NumericalScalar residual)
{
  maximumResidual_ = residual;
}

NumericalScalar FunctionalChaosAlgorithm::getMaximumResidual() const
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
  const UnsignedInteger outputDimension(model_.getOutputDimension());
  // First, compute all the parts that are independent of the marginal output
  // Create the isoprobabilistic transformation
  // If we call Z a random vector which probability distribution is measure and X a random vector which probability distribution is distribution_, and T the isoprobabilistic transformation, we have:
  // Z = T(X)
  // X = T^{-1}(Z)
  // The transformation is built using the isoprobabilistic transformations of both X and Z
  // First, get the measure upon which the orthogonal basis is built
  const OrthogonalBasis basis(adaptiveStrategy_.getImplementation()->basis_);
  const Distribution measure(basis.getMeasure());
  // Correct the measure of the projection strategy if no input sample
  if (projectionStrategy_.getImplementation()->inputSample_.getSize() == 0)
    projectionStrategy_.setMeasure(measure);
  // Check that it is a product measure
  LOGINFO("Build the iso-probabilistic transformation");
  if (!measure.hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: cannot use FunctionalChaosAlgorithm with an orthogonal basis not based on a product measure";
  // Has the distribution an independent copula? Simply use marginal transformations
  if (distribution_.hasIndependentCopula())
  {
    const UnsignedInteger dimension(distribution_.getDimension());
    // We use empty collections to avoid the construction of default distributions
    DistributionCollection marginalX(0);
    DistributionCollection marginalZ(0);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      marginalX.add(distribution_.getMarginal(i));
      marginalZ.add(measure.getMarginal(i));
    }
    // The distributions have an independent copula, they can be converted one into another by marginal transformation. T is such that T(X) = Z
    const MarginalTransformationEvaluation evaluationT(MarginalTransformationEvaluation(marginalX, marginalZ));
    const MarginalTransformationGradient gradientT(evaluationT);
    const MarginalTransformationHessian hessianT(evaluationT);
    transformation_ = NumericalMathFunction(evaluationT.clone(), gradientT.clone(), hessianT.clone());
    const MarginalTransformationEvaluation evaluationTinv(MarginalTransformationEvaluation(marginalZ, marginalX));
    const MarginalTransformationGradient gradientTinv(evaluationTinv);
    const MarginalTransformationHessian hessianTinv(evaluationTinv);
    inverseTransformation_ = NumericalMathFunction(evaluationTinv.clone(), gradientTinv.clone(), hessianTinv.clone());
  }
  else
  {
    // Is the input distribution mapped into a normal standard space?
    // We know that the standard space associated with Z is a normal space, as Z has an independent copula. We can check that X has the same standard space if its standard distribution has also an independent copula
    if (distribution_.getStandardDistribution().hasIndependentCopula())
    {
      // The distributions share the same standard space, it is thus possible to transform one into the other by composition between their isoprobabilistic transformations. T = T^{-1}_Z o T_X and T^{-1} = T^{-1}_X o T_Z
      const NumericalMathFunction TX(distribution_.getIsoProbabilisticTransformation());
      const NumericalMathFunction TinvX(distribution_.getInverseIsoProbabilisticTransformation());
      const NumericalMathFunction TZ(measure.getIsoProbabilisticTransformation());
      const NumericalMathFunction TinvZ(measure.getInverseIsoProbabilisticTransformation());
      transformation_ = NumericalMathFunction(TinvZ, TX);
      inverseTransformation_ = NumericalMathFunction(TinvX, TZ);
    }
    // The standard space is not normal, use Rosenblatt transformation instead of the native iso-probabilistic transformation.
    else
    {
      // The distributions share the same standard space if we use a Rosenblatt transforation, it is thus possible to transform one into the other by composition between their isoprobabilistic transformations. T = T^{-1}_Z o T_X and T^{-1} = T^{-1}_X o T_Z
      const NumericalMathFunction TX(NumericalMathFunctionImplementation(RosenblattEvaluation(distribution_.getImplementation()).clone()));
      const NumericalMathFunction TinvX(NumericalMathFunctionImplementation(InverseRosenblattEvaluation(distribution_.getImplementation()).clone()));
      const NumericalMathFunction TZ(measure.getIsoProbabilisticTransformation());
      const NumericalMathFunction TinvZ(measure.getInverseIsoProbabilisticTransformation());
      transformation_ = NumericalMathFunction(TinvZ, TX);
      inverseTransformation_ = NumericalMathFunction(TinvX, TZ);
    }
  } // Non-independent input copula
  // Build the composed model g = f o T^{-1}, which is a function of Z so it can be decomposed upon an orthonormal basis based on Z distribution
  LOGINFO("Transform the input sample in the measure space if needed");
  composedModel_ = NumericalMathFunction(model_, inverseTransformation_);
  // If the input and output databases have already been given to the projection strategy, transport them to the measure space
  NumericalSample initialInputSample(projectionStrategy_.getImplementation()->inputSample_);
  if (projectionStrategy_.getImplementation()->inputSample_.getSize() > 0)
    projectionStrategy_.getImplementation()->inputSample_ = transformation_(initialInputSample);
  // Second, compute the results for each marginal output and merge
  // these marginal results.
  // As all the components have been projected using the same basis,
  // the representation is a linear combination of selected multivariate
  // polynomials with vector coefficients
  // We build the coefficients of the combination. As some indices may be
  // missing, we have to take care of the different sparsity patterns
  NumericalPoint residuals(outputDimension);
  NumericalPoint relativeErrors(outputDimension);
  std::map<UnsignedInteger, NumericalPoint> coefficientsMap;
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
  {
    LOGINFO(OSS() << "Work on output marginal " << outputIndex << " over " << outputDimension - 1);
    Indices marginalIndices;
    NumericalPoint marginalAlpha_k;
    NumericalScalar marginalResidual;
    NumericalScalar marginalRelativeError;
    // Compute the indices, the coefficients, the residual and the relative error of the current marginal output
    runMarginal(outputIndex, marginalIndices, marginalAlpha_k, marginalResidual, marginalRelativeError);
    residuals[outputIndex] = marginalResidual;
    relativeErrors[outputIndex] = marginalRelativeError;
    for (UnsignedInteger j = 0; j < marginalIndices.getSize(); ++j)
    {
      // Deal only with non-zero coefficients
      const NumericalScalar marginalAlpha_kj(marginalAlpha_k[j]);
      if (marginalAlpha_kj != 0.0)
      {
        // Current index in the decomposition of the current marginal output
        const UnsignedInteger index(marginalIndices[j]);
        // If the current index is not in the map, create it
        if (coefficientsMap.find(index) == coefficientsMap.end()) coefficientsMap[index] = NumericalPoint(outputDimension, 0.0);
        // Add the current scalar coefficient to the corresponding component of the vectorial coefficient
        coefficientsMap[index][outputIndex] = marginalAlpha_kj;
      }
    } // Loop over the marginal indices
  } // Loop over the output dimension
  // At this point, the map contains all the associations (index, vector coefficient). It remains to present these data into the proper form and to build the associated partial basis
  std::map<UnsignedInteger, NumericalPoint>::iterator iter;
  // Full set of indices
  Indices I_k(0);
  // Full set of vectorial coefficients
  NumericalSample alpha_k(0, outputDimension);
  // Full set of partial basis functions.
  NumericalMathFunctionCollection Psi_k(0);
  for (iter = coefficientsMap.begin(); iter != coefficientsMap.end(); ++iter)
  {
    const UnsignedInteger i(iter->first);
    const NumericalPoint currentcoefficient(iter->second);
    I_k.add(i);
    alpha_k.add(currentcoefficient);
    Psi_k.add(basis.build(i));
  }
  // Build the result
  result_ = FunctionalChaosResult(model_, distribution_, transformation_, inverseTransformation_, composedModel_, basis, I_k, alpha_k, Psi_k, residuals, relativeErrors);
  // Restore the initial input sample
  // If it was not empty, it was given by the user
  if (initialInputSample.getSize() > 0) projectionStrategy_.getImplementation()->inputSample_ = initialInputSample;
  // else it has been produced in the measure space, convert it into the physical space
  else projectionStrategy_.getImplementation()->inputSample_ = inverseTransformation_(projectionStrategy_.getImplementation()->inputSample_);
}

/* Marginal computation */
void FunctionalChaosAlgorithm::runMarginal(const UnsignedInteger marginalIndex,
    Indices & indices,
    NumericalPoint & coefficients,
    NumericalScalar & residual,
    NumericalScalar & relativeError)
{
  // Initialize the projection basis Phi_k_p_ and I_p_
  LOGINFO("Compute the initial basis");
  adaptiveStrategy_.computeInitialBasis();
  do
  {
    LOGINFO("Compute the coefficients");
    projectionStrategy_.computeCoefficients(composedModel_, *adaptiveStrategy_.getImplementation()->basis_.getImplementation(), adaptiveStrategy_.getImplementation()->I_p_, adaptiveStrategy_.getImplementation()->addedPsi_k_ranks_, adaptiveStrategy_.getImplementation()->conservedPsi_k_ranks_, adaptiveStrategy_.getImplementation()->removedPsi_k_ranks_, marginalIndex);
    // The basis is adapted under the following conditions:
    // + the current residual is small enough
    // + the adaptive strategy has no more vector to propose
    if (projectionStrategy_.getImplementation()->residual_p_ < maximumResidual_)
    {
      LOGINFO("Stop on small residual");
      indices = adaptiveStrategy_.getImplementation()->I_p_;
      coefficients = projectionStrategy_.getImplementation()->alpha_k_p_;
      residual = projectionStrategy_.getImplementation()->residual_p_;
      relativeError = projectionStrategy_.getImplementation()->relativeError_p_;
      return;
    }
    LOGINFO("Adapt the basis");
    adaptiveStrategy_.updateBasis(projectionStrategy_.getImplementation()->alpha_k_p_, projectionStrategy_.getImplementation()->residual_p_, projectionStrategy_.getImplementation()->relativeError_p_);
  }
  // Check if there is still something to do
  while ((adaptiveStrategy_.getImplementation()->addedPsi_k_ranks_.getSize() > 0) || (adaptiveStrategy_.getImplementation()->removedPsi_k_ranks_.getSize() > 0));

  LOGINFO("No more basis adaptation");
  indices = adaptiveStrategy_.getImplementation()->I_p_;
  coefficients = projectionStrategy_.getImplementation()->alpha_k_p_;
  residual = projectionStrategy_.getImplementation()->residual_p_;
  relativeError = projectionStrategy_.getImplementation()->relativeError_p_;
}


/* Get the functional chaos result */
FunctionalChaosResult FunctionalChaosAlgorithm::getResult() const
{
  return result_;
}


NumericalSample FunctionalChaosAlgorithm::getInputSample() const
{
  return projectionStrategy_.getInputSample();
}


NumericalSample FunctionalChaosAlgorithm::getOutputSample() const
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
