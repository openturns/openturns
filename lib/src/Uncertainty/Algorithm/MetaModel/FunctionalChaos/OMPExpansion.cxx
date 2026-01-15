//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a least-squares
 *         approach with orthogonal matching pursuit selection.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/OMPExpansion.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Function> FunctionCollection;

CLASSNAMEINIT(OMPExpansion)

static const Factory<OMPExpansion> Factory_OMPExpansion;


/* Default constructor */
OMPExpansion::OMPExpansion()
  : FunctionalChaosAlgorithm()
{
  // Nothing to do
}


/* Constructor */
OMPExpansion::OMPExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution)
  : OMPExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution)
{
  // Nothing to do
}

/* Constructor */
OMPExpansion::OMPExpansion(const Sample & inputSample,
			     const Point & weights,
			     const Sample & outputSample,
			     const Distribution & distribution)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
{
  // Extract the basis from the adaptive strategy attribute of the base class
  basis_ = getAdaptiveStrategy().getBasis();
  // The basis size is the maximum dimension of the adaptive strategy
  basisSize_ = getAdaptiveStrategy().getMaximumDimension();
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
  // The method name is set to the default one, given by ResourceMap
  methodName_ = ResourceMap::GetAsString("OMPExpansion-DecompositionMethod");
  // The fitting algorithm
  fitting_ = CorrectedLeaveOneOut();
}

/* Constructor */
OMPExpansion::OMPExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const String & methodName,
			     const FittingAlgorithm & fitting)
  : OMPExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution, basis, basisSize, methodName, fitting)
{
  // Nothing to do
}

/* Constructor */
OMPExpansion::OMPExpansion(const Sample & inputSample,
			     const Point & weights,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const String & methodName,
			     const FittingAlgorithm & fitting)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
  , basis_(basis)
  , basisSize_(basisSize)
  , methodName_(methodName)
  , fitting_(fitting)
{
  // The arguments are checked in the base class, excepted the basis and its size
  if (basis.getMeasure().getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the basis must have a measure with the same dimension as the input distribution, here measure dimension=" << basis.getMeasure().getDimension() << " and distribution dimension=" << distribution.getDimension();
  if (basisSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot project on a basis of size zero";
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
}


/* Virtual constructor */
OMPExpansion * OMPExpansion::clone() const
{
  return new OMPExpansion(*this);
}


/* Computes the functional chaos */
void OMPExpansion::run()
{
  // First, compute all the parts that are independent of the marginal output
  // Create the isoprobabilistic transformation
  // We have two distributions here:
  // + The distribution of the input, called distribution_
  // + The distribution defining the inner product in basis, called measure
  // The projection is done on the basis, ie wrt measure_, so we have to
  // introduce an isoprobabilistic transformation that maps distribution_ onto
  // measure
  //
  // Check if we have to transform the input sample in order to adapt it
  // to the basis
  FunctionCollection functions;
  if (designProxy_.getSampleSize() == 0)
  {
    const Distribution measure(basis_.getMeasure());
    Sample transformedInputSample;
    if (distribution_ == measure)
    {
      transformation_ = IdentityFunction(distribution_.getDimension());
      inverseTransformation_ = IdentityFunction(distribution_.getDimension());
      transformedInputSample = inputSample_;
    }
    else
    {
      transformation_ = DistributionTransformation(distribution_, basis_.getMeasure());
      inverseTransformation_ = DistributionTransformation(basis_.getMeasure(), distribution_);
      transformedInputSample = transformation_(inputSample_);
    }
    functions = FunctionCollection(basisSize_);
    const UnsignedInteger sampleSize = transformedInputSample.getSize();
    MatrixImplementation designMatrix(sampleSize, basisSize_);
    UnsignedInteger shift = 0;
    // Build the functions and the design matrix on the fly
    for (UnsignedInteger i = 0; i < basisSize_; ++i)
      {
	functions[i] = basis_.build(i);
	const Point column(functions[i](transformedInputSample).asPoint());
	std::copy(column.begin(), column.end(), designMatrix.begin() + shift);
	shift += sampleSize;
      }
    //    designProxy_ = DesignProxy(designMatrix);//transformedInputSample, functions);
    designProxy_ = DesignProxy(transformedInputSample, functions);
  } // Design proxy and transformation initialization
  else
    functions = designProxy_.getBasis();
  // Here we have several options for the offline computations:
  // 1) Do nothing, let the DesignProxy be filled on the fly
  // 2) Compute the full design matrix, overwriting the cache size
  // 3) Compute the partial design matrix up to the cache size
  // Note: the cache size is given by ResourceMap::GetAsUnsignedInteger("DesignProxy-DefaultCacheSize")
  // For now (23/06/2025) we do nothing but we reuse the design proxy between output marginal approximations
  // Initialize the active functions with the constant function
  LeastSquaresMethod leastSquaresMethod = LeastSquaresMethod::Build(methodName_, designProxy_, weights_, Indices(1, 0));
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const UnsignedInteger sampleSize = inputSample_.getSize();
  Point crossValidationErrorHistory(0);
  // Parameters of the early exit criteria
  const Scalar alpha = std::max(1.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor"));
  const Scalar errorThreshold = std::max(0.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-ErrorThreshold"));
  const Scalar maximumError = std::max(0.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-MaximumError"));
  // No more iterations than the number of functions and the number of training points
  const UnsignedInteger maximumNumberOfIterations = std::min(basisSize_ - 1, sampleSize - 1);
  // Second, compute the results for each marginal output and merge
  // these marginal results.
  // As all the components have been projected using the same basis,
  // the representation is a linear combination of selected multivariate
  // polynomials with vector coefficients
  // We build the coefficients of the combination. As some indices may be
  // missing, we have to take care of the different sparsity patterns
  std::map<UnsignedInteger, Point> coefficientsMap;
  const Scalar smallCoefficient = ResourceMap::GetAsScalar("DualLinearCombinationEvaluation-SmallCoefficient");
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
  {
    // The constant function is dealt with as an initialization of the whole selection loop
    Indices marginalActiveFunctions(1, 0);
    Indices flagActiveFunctions(basisSize_, 0);
    flagActiveFunctions[0] = 1;
    const Sample marginalOutputSample(outputSample_.getMarginal(outputIndex));
    const Point rhs(marginalOutputSample.asPoint());
    // std::cerr << "marginalOutputSample=" << marginalOutputSample << std::endl;
    // std::cerr << "rhs=" << rhs << std::endl;

    Scalar crossValidationError = fitting_.run(leastSquaresMethod, marginalOutputSample);
    crossValidationErrorHistory.add(crossValidationError);
    // The initial residual is the centered marginal output sample
    Point coefficients(marginalOutputSample.computeMean());
    Point residuals(rhs - Point(sampleSize, coefficients[0]));
    // Store the best score obtained so far
    Scalar bestCrossValidationError = crossValidationError;
    Point bestCoefficients(coefficients);
    Indices bestSelection(marginalActiveFunctions);
    // Loop over the basis functions
    for (UnsignedInteger i = 0; i < maximumNumberOfIterations; ++i)
      {
	// Find candidate with maximum absolute correlation with the residual
	Scalar maximumAbsoluteCorrelation = -1.0;
	UnsignedInteger bestBasisFunctionIndex = 0;
        for (UnsignedInteger k = 1; k < basisSize_; ++k)
	  {
	    // Skip the functions already selected
            if (flagActiveFunctions[k] == 1) continue;
            const Matrix currentBasisFunctionValues(designProxy_.computeDesign({k}));
	    //std::cerr << "currentBasisFunctionValues=" << currentBasisFunctionValues << std::endl;
	    //std::cerr << "residuals=" << residuals << std::endl;
            const Scalar currentAbsoluteCorrelation = std::abs((currentBasisFunctionValues.transpose() * residuals)[0]) / sampleSize;
            if (currentAbsoluteCorrelation > maximumAbsoluteCorrelation)
	      {
                bestBasisFunctionIndex = k;
                maximumAbsoluteCorrelation = currentAbsoluteCorrelation;
	      }
	  } // Best candidate found
	LOGINFO(OSS() << "Best candidate at iteration " << i << " over " << maximumNumberOfIterations - 1 << " is " << bestBasisFunctionIndex);
	flagActiveFunctions[bestBasisFunctionIndex] = 1;
	// Update the LS method
	leastSquaresMethod.update({bestBasisFunctionIndex}, marginalActiveFunctions, Indices());
	// Add the best candidate to the active set
	marginalActiveFunctions.add(bestBasisFunctionIndex);
	// Update the coefficients
	coefficients = leastSquaresMethod.solve(rhs);
	// Compute corrected leave-out score
	crossValidationError = fitting_.run(leastSquaresMethod, marginalOutputSample);
	crossValidationErrorHistory.add(crossValidationError);
	// Update the best candidate if needed
	if (crossValidationError < bestCrossValidationError)
	  {
	    // std::cerr << "Update the best candidate" << std::endl;
	    // std::cerr << "bestCrossValidationError=" << bestCrossValidationError << ", bestCoefficients=" << bestCoefficients << ", bestSelection=" << bestSelection << std::endl;
	    bestCrossValidationError = crossValidationError;
	    bestCoefficients = coefficients;
	    bestSelection = marginalActiveFunctions;
	  }
	// Early exit tests
	else
	  {
	    // First test: the current cross validation error is larger than the current
	    // best error up to a margin
	    if (!(crossValidationError <= alpha * bestCrossValidationError))
	      {
		LOGINFO(OSS() << "Cross-validation error=" << crossValidationError << " larger than " << alpha << "*" << bestCrossValidationError << "=" << alpha * bestCrossValidationError);
		break;
	      }
	    // Second test: the cross validation error is larger than an absolute
	    // maximum and is not decreasing
	    if (crossValidationError > maximumError)
	      {
		LOGINFO(OSS() << "Cross-validation error=" << crossValidationError << " larger than " << maximumError);
		break;
	      }
	  } // No update of the best cross-validation error
	// Third test: the best cross-validation error is below a given absolute threshold
	if (!(bestCrossValidationError >= errorThreshold))
	  {
	    LOGINFO(OSS() << "Best cross-validation error=" << bestCrossValidationError << " smaller than threshold=" << errorThreshold);
	    break;
	  }
	// Update the residuals
	const Matrix designMatrix(leastSquaresMethod.computeWeightedDesign());
	// Orthogonal matching pursuit
	residuals = rhs - designMatrix * coefficients;
      } // Loop over the basis functions
    // Store the best selection for the current output index
    for (UnsignedInteger j = 0; j < bestSelection.getSize(); ++j)
      {
	// Deal only with non-zero coefficients
	const Scalar marginalAlpha_kj = bestCoefficients[j];
	// To avoid -0.0
	if (std::abs(marginalAlpha_kj) > smallCoefficient)
	  {
	    // Current index in the decomposition of the current marginal output
	    const UnsignedInteger index = bestSelection[j];
	    // If the current index is not in the map, create it
	    if (coefficientsMap.find(index) == coefficientsMap.end()) coefficientsMap[index] = Point(outputDimension, 0.0);
	    // Add the current scalar coefficient to the corresponding component of the vectorial coefficient
	    coefficientsMap[index][outputIndex] = marginalAlpha_kj;
	  } // Nonzero coefficient
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
    Psi_k.add(functions[i]);
  }
  // Build the result
  result_ = FunctionalChaosResult(inputSample_, outputSample_, distribution_, transformation_,
                                  inverseTransformation_, basis_, I_k, alpha_k, Psi_k);
  result_.setIsLeastSquares(true);
  result_.setInvolvesModelSelection(true);
}

/* Method to get/set the active functions */
Indices OMPExpansion::getActiveFunctions() const
{
  return activeFunctions_;
}

void OMPExpansion::setActiveFunctions(const Indices & activeFunctions)
{
  if (!activeFunctions.check(basisSize_)) throw InvalidArgumentException(HERE) << "Error: the active functions must have indices less than " << basisSize_;
  activeFunctions_ = activeFunctions;
}

/* Method to get/set the fitting algorithm */
FittingAlgorithm OMPExpansion::getFittingAlgorithm() const
{
  return fitting_;
}

void OMPExpansion::setFittingAlgorithm(const FittingAlgorithm & fitting)
{
  fitting_ = fitting;
}

/* String converter */
String OMPExpansion::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " basis=" << basis_
         << " basisSize=" << basisSize_
         << " activeFunctions=" << activeFunctions_
         << " designProxy=" << designProxy_
         << " transformation=" << transformation_
         << " inverseTransformation=" << inverseTransformation_;
}


/* String converter */
String OMPExpansion::__str__(const String & ) const
{
  return OSS() << getClassName() << "("
         << "basis=" << basis_
         << ", basisSize=" << basisSize_
         << ", activeFunctions=" << activeFunctions_
         << ", methodName=" << methodName_
         << ")";
}

/* Method save() stores the object through the StorageManager */
void OMPExpansion::save(Advocate & adv) const
{
  FunctionalChaosAlgorithm::save(adv);
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "basisSize_", basisSize_ );
  adv.saveAttribute( "activeFunctions_", activeFunctions_ );
  adv.saveAttribute( "methodName_", methodName_ );
  adv.saveAttribute( "transformation_", transformation_ );
  adv.saveAttribute( "inverseTransformation_", inverseTransformation_ );
}


/* Method load() reloads the object from the StorageManager */
void OMPExpansion::load(Advocate & adv)
{
  FunctionalChaosAlgorithm::load(adv);
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "basisSize_", basisSize_ );
  adv.loadAttribute( "activeFunctions_", activeFunctions_ );
  adv.loadAttribute( "methodName_", methodName_ );
  adv.loadAttribute( "transformation_", transformation_ );
  adv.loadAttribute( "inverseTransformation_", inverseTransformation_ );
}



END_NAMESPACE_OPENTURNS
