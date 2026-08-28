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

#include "openturns/SparseExpansion.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Function> FunctionCollection;

CLASSNAMEINIT(SparseExpansion)

static const Factory<SparseExpansion> Factory_SparseExpansion;


/* Default constructor */
SparseExpansion::SparseExpansion()
  : FunctionalChaosAlgorithm()
{
  // Nothing to do
}


/* Constructor */
SparseExpansion::SparseExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution)
  : SparseExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution)
{
  // Nothing to do
}

/* Constructor */
SparseExpansion::SparseExpansion(const Sample & inputSample,
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
  methodName_ = ResourceMap::GetAsString("SparseExpansion-DecompositionMethod");
  // The fitting algorithm
  fitting_ = CorrectedLeaveOneOut();
  // The selection method is set to OMP by default
  selectionMethod_ = "OMP";
}

/* Constructor */
SparseExpansion::SparseExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const String & methodName,
			     const FittingAlgorithm & fitting)
  : SparseExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution, basis, basisSize, methodName, fitting)
{
  // Nothing to do
}

/* Constructor with active functions */
SparseExpansion::SparseExpansion(const Sample & inputSample,
			     const Point & weights,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const Indices & activeFunctions,
			     const String & methodName,
			     const FittingAlgorithm & fitting)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
  , basis_(basis)
  , basisSize_(basisSize)
  , methodName_(methodName)
  , fitting_(fitting)
  , selectionMethod_("OMP")
{
  // The arguments are checked in the base class, excepted the basis and its size
  if (basis.getMeasure().getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the basis must have a measure with the same dimension as the input distribution, here measure dimension=" << basis.getMeasure().getDimension() << " and distribution dimension=" << distribution.getDimension();
  if (basisSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot project on a basis of size zero";
  // Use the provided active functions, ensuring index 0 is always included
  activeFunctions_ = activeFunctions;
  if (!activeFunctions_.contains(0))
    activeFunctions_.add(0);
}

/* Constructor with active functions */
SparseExpansion::SparseExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const Indices & activeFunctions,
			     const String & methodName,
			     const FittingAlgorithm & fitting)
  : SparseExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution, basis, basisSize, activeFunctions, methodName, fitting)
{
  // Nothing to do
}

/* Constructor */
SparseExpansion::SparseExpansion(const Sample & inputSample,
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
  , selectionMethod_("OMP")
{
  // The arguments are checked in the base class, excepted the basis and its size
  if (basis.getMeasure().getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the basis must have a measure with the same dimension as the input distribution, here measure dimension=" << basis.getMeasure().getDimension() << " and distribution dimension=" << distribution.getDimension();
  if (basisSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot project on a basis of size zero";
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
}


/* Virtual constructor */
SparseExpansion * SparseExpansion::clone() const
{
  return new SparseExpansion(*this);
}


/* Computes the functional chaos */
void SparseExpansion::run()
{
  // First, compute all the parts that are independent of the marginal output
  // Create the isoprobabilistic transformation
  FunctionCollection functions;
  if (designProxy_.getSampleSize() == 0)
  {
    const Distribution measure(basis_.getMeasure());
    const Bool identityTransformation = initializeTransformation(measure);
    Sample transformedInputSample(inputSample_);
    if (!identityTransformation)
      transformedInputSample = transformation_(inputSample_);
    functions = FunctionCollection(basisSize_);
    for (UnsignedInteger i = 0; i < basisSize_; ++i)
      functions[i] = basis_.build(i);
    designProxy_ = DesignProxy(transformedInputSample, functions);
  }
  else
    functions = designProxy_.getBasis();

  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const UnsignedInteger sampleSize = inputSample_.getSize();
  const Scalar alpha = std::max(1.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor"));
  const Scalar errorThreshold = std::max(0.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-ErrorThreshold"));
  const Scalar maximumError = std::max(0.0, ResourceMap::GetAsScalar("LeastSquaresMetaModelSelection-MaximumError"));
  const UnsignedInteger maximumNumberOfIterations = std::min(basisSize_ - 1, sampleSize - 1);
  const Scalar smallCoefficient = ResourceMap::GetAsScalar("DualLinearCombinationEvaluation-SmallCoefficient");
  const UnsignedInteger consecutiveIncreases = std::max(static_cast<UnsignedInteger>(1), ResourceMap::GetAsUnsignedInteger("SparseExpansion-ConsecutiveIncreases"));

  if (selectionMethod_ == "LARS")
    runLARS(functions, outputDimension, sampleSize, maximumNumberOfIterations, alpha, errorThreshold, maximumError, smallCoefficient, consecutiveIncreases);
  else
    runOMP(functions, outputDimension, sampleSize, maximumNumberOfIterations, alpha, errorThreshold, maximumError, smallCoefficient, consecutiveIncreases);
}


void SparseExpansion::runOMP(const Collection<Function> & functions,
                             const UnsignedInteger outputDimension,
                             const UnsignedInteger sampleSize,
                             const UnsignedInteger maximumNumberOfIterations,
                             const Scalar alpha,
                             const Scalar errorThreshold,
                             const Scalar maximumError,
                             const Scalar smallCoefficient,
                             const UnsignedInteger consecutiveIncreases)
{
  LeastSquaresMethod leastSquaresMethod = LeastSquaresMethod::Build(methodName_, designProxy_, weights_, Indices(1, 0));
  std::vector<bool> isCandidate(basisSize_, false);
  for (UnsignedInteger idx = 0; idx < activeFunctions_.getSize(); ++idx)
    isCandidate[activeFunctions_[idx]] = true;

  std::map<UnsignedInteger, Point> coefficientsMap;
  Collection<Indices> allIndicesHistory;
  Collection<Point> allCoefficientsHistory;
  Point allErrorHistory;
  Indices historyCutPoints(1, 0);

  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
  {
    Indices marginalActiveFunctions(1, 0);
    Indices flagActiveFunctions(basisSize_, 0);
    flagActiveFunctions[0] = 1;
    const Sample marginalOutputSample(outputSample_.getMarginal(outputIndex));
    const Point rhs(marginalOutputSample.asPoint());

    Scalar crossValidationError = fitting_.run(leastSquaresMethod, marginalOutputSample);
    // Compute weighted mean: c_0 = sum(w_s * y_s) / sum(w_s)
    Scalar weightedMean = 0.0;
    Scalar weightSum = 0.0;
    for (UnsignedInteger s = 0; s < sampleSize; ++s)
    {
      weightedMean += weights_[s] * rhs[s];
      weightSum += weights_[s];
    }
    weightedMean /= weightSum;
    Point coefficients(1, weightedMean);
    allIndicesHistory.add(marginalActiveFunctions);
    allCoefficientsHistory.add(coefficients);
    allErrorHistory.add(crossValidationError);
    Point weightedResiduals(sampleSize);
    for (UnsignedInteger s = 0; s < sampleSize; ++s)
      weightedResiduals[s] = weights_[s] * (rhs[s] - coefficients[0]);

    Scalar bestCrossValidationError = crossValidationError;
    Point bestCoefficients(coefficients);
    Indices bestSelection(marginalActiveFunctions);
    UnsignedInteger increaseCount = 0;

    for (UnsignedInteger i = 0; i < maximumNumberOfIterations; ++i)
    {
      Scalar maximumAbsoluteCorrelation = -1.0;
      UnsignedInteger bestBasisFunctionIndex = 0;
      for (UnsignedInteger k = 1; k < basisSize_; ++k)
      {
        if (flagActiveFunctions[k] == 1) continue;
        if (!isCandidate[k]) continue;
        const Matrix currentBasisFunctionValues(designProxy_.computeDesign({k}));
        const Scalar currentAbsoluteCorrelation = std::abs((currentBasisFunctionValues.transpose() * weightedResiduals)[0]);
        if (currentAbsoluteCorrelation > maximumAbsoluteCorrelation)
        {
          bestBasisFunctionIndex = k;
          maximumAbsoluteCorrelation = currentAbsoluteCorrelation;
        }
      }
      if (maximumAbsoluteCorrelation < 0.0) break;
      LOGINFO(OSS() << "OMP: best candidate at iteration " << i << " is " << bestBasisFunctionIndex);
      flagActiveFunctions[bestBasisFunctionIndex] = 1;
      leastSquaresMethod.update({bestBasisFunctionIndex}, marginalActiveFunctions, Indices());
      marginalActiveFunctions.add(bestBasisFunctionIndex);
      coefficients = leastSquaresMethod.solve(rhs);
      crossValidationError = fitting_.run(leastSquaresMethod, marginalOutputSample);
      allIndicesHistory.add(marginalActiveFunctions);
      allCoefficientsHistory.add(coefficients);
      allErrorHistory.add(crossValidationError);

      if (crossValidationError < bestCrossValidationError)
      {
        bestCrossValidationError = crossValidationError;
        bestCoefficients = coefficients;
        bestSelection = marginalActiveFunctions;
        increaseCount = 0;
      }
      else
      {
        ++increaseCount;
        if (increaseCount >= consecutiveIncreases)
        {
          LOGINFO(OSS() << "Cross-validation error=" << crossValidationError << " increased " << increaseCount << " consecutive times");
          break;
        }
        if (!(crossValidationError <= alpha * bestCrossValidationError))
        {
          LOGINFO(OSS() << "Cross-validation error=" << crossValidationError << " larger than " << alpha << "*" << bestCrossValidationError);
          break;
        }
        if (crossValidationError > maximumError)
        {
          LOGINFO(OSS() << "Cross-validation error=" << crossValidationError << " larger than " << maximumError);
          break;
        }
      }
      if (!(bestCrossValidationError >= errorThreshold))
      {
        LOGINFO(OSS() << "Best cross-validation error=" << bestCrossValidationError << " smaller than threshold=" << errorThreshold);
        break;
      }
      const Point pred(leastSquaresMethod.computeWeightedDesign() * coefficients);
      for (UnsignedInteger s = 0; s < sampleSize; ++s)
        weightedResiduals[s] = weights_[s] * rhs[s] - std::sqrt(weights_[s]) * pred[s];
    }

    for (UnsignedInteger j = 0; j < bestSelection.getSize(); ++j)
    {
      const Scalar marginalAlpha_kj = bestCoefficients[j];
      if (std::abs(marginalAlpha_kj) > smallCoefficient)
      {
        const UnsignedInteger index = bestSelection[j];
        if (coefficientsMap.find(index) == coefficientsMap.end()) coefficientsMap[index] = Point(outputDimension, 0.0);
        coefficientsMap[index][outputIndex] = marginalAlpha_kj;
      }
    }
    historyCutPoints.add(allIndicesHistory.getSize());
  }

  std::map<UnsignedInteger, Point>::iterator iter;
  Indices I_k(0);
  Sample alpha_k(0, outputDimension);
  FunctionCollection Psi_k(0);
  for (iter = coefficientsMap.begin(); iter != coefficientsMap.end(); ++iter)
  {
    const UnsignedInteger i = iter->first;
    const Point currentCoefficient(iter->second);
    I_k.add(i);
    alpha_k.add(currentCoefficient);
    Psi_k.add(functions[i]);
  }
  result_ = FunctionalChaosResult(inputSample_, outputSample_, distribution_, transformation_,
                                  inverseTransformation_, basis_, I_k, alpha_k, Psi_k);
  result_.setIsLeastSquares(true);
  result_.setInvolvesModelSelection(true);
  result_.setSelectionHistory(allIndicesHistory, allCoefficientsHistory, historyCutPoints);
  result_.setErrorHistory(allErrorHistory, historyCutPoints);
  result_.setUseDomination(useDomination_);
}


void SparseExpansion::runLARS(const Collection<Function> & functions,
                              const UnsignedInteger outputDimension,
                              const UnsignedInteger sampleSize,
                              const UnsignedInteger maximumNumberOfIterations,
                              const Scalar alpha,
                              const Scalar errorThreshold,
                              const Scalar maximumError,
                              const Scalar smallCoefficient,
                              const UnsignedInteger consecutiveIncreases)
{
  // Precompute full weighted design matrix PhiW = sqrt(W) * Phi
  // Create a temporary least squares method with all active indices to get the full weighted design
  LeastSquaresMethod fullMethod = LeastSquaresMethod::Build(methodName_, designProxy_, weights_, activeFunctions_);
  const Matrix PhiW = fullMethod.computeWeightedDesign(true);

  std::vector<bool> isCandidate(basisSize_, false);
  for (UnsignedInteger idx = 0; idx < activeFunctions_.getSize(); ++idx)
    isCandidate[activeFunctions_[idx]] = true;

  LeastSquaresMethod leastSquaresMethod = LeastSquaresMethod::Build(methodName_, designProxy_, weights_, Indices(1, 0));
  std::map<UnsignedInteger, Point> coefficientsMap;
  Collection<Indices> allIndicesHistory;
  Collection<Point> allCoefficientsHistory;
  Point allErrorHistory;
  Indices historyCutPoints(1, 0);

  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
  {
    Indices marginalActiveFunctions(1, 0);
    Indices flagActiveFunctions(basisSize_, 0);
    flagActiveFunctions[0] = 1;
    const Sample marginalOutputSample(outputSample_.getMarginal(outputIndex));
    const Point rhs(marginalOutputSample.asPoint());

    Scalar crossValidationError = fitting_.run(leastSquaresMethod, marginalOutputSample);
    // Compute weighted mean: c_0 = sum(w_s * y_s) / sum(w_s)
    Scalar weightedMean = 0.0;
    Scalar weightSum = 0.0;
    for (UnsignedInteger s = 0; s < sampleSize; ++s)
    {
      weightedMean += weights_[s] * rhs[s];
      weightSum += weights_[s];
    }
    weightedMean /= weightSum;
    Point coefficients(1, weightedMean);
    allIndicesHistory.add(marginalActiveFunctions);
    allCoefficientsHistory.add(coefficients);
    allErrorHistory.add(crossValidationError);

    // LARS prediction (incrementally updated)
    Point mu(sampleSize, coefficients[0]);

    // Store correlations for all basis functions (reused across iterations)
    Point correlations(basisSize_, 0.0);

    Scalar bestCrossValidationError = crossValidationError;
    Point bestCoefficients(coefficients);
    Indices bestSelection(marginalActiveFunctions);
    UnsignedInteger increaseCount = 0;

    // Precompute sqrt(weights) for correlation computations
    Point weightSqrt(sampleSize);
    for (UnsignedInteger s = 0; s < sampleSize; ++s)
      weightSqrt[s] = std::sqrt(weights_[s]);

    for (UnsignedInteger i = 0; i < maximumNumberOfIterations; ++i)
    {
      // Compute residual r = y - mu
      Point residual(sampleSize);
      for (UnsignedInteger s = 0; s < sampleSize; ++s)
        residual[s] = rhs[s] - mu[s];

      // Compute weighted correlations c_j = <phi_j, r>_W = Phi^T W r
      // Using PhiW = sqrt(W) Phi, this is PhiW^T sqrt(W) r
      for (UnsignedInteger k = 0; k < basisSize_; ++k)
        correlations[k] = 0.0;
      for (UnsignedInteger j = 0; j < basisSize_; ++j)
      {
        if (!isCandidate[j]) continue;
        Scalar c = 0.0;
        for (UnsignedInteger s = 0; s < sampleSize; ++s)
          c += PhiW(s, j) * weightSqrt[s] * residual[s];
        correlations[j] = c;
      }

      // Find candidate with maximum absolute correlation
      Scalar cMax = -1.0;
      UnsignedInteger bestBasisFunctionIndex = 0;
      for (UnsignedInteger k = 1; k < basisSize_; ++k)
      {
        if (flagActiveFunctions[k] == 1) continue;
        if (!isCandidate[k]) continue;
        const Scalar cAbs = std::abs(correlations[k]);
        if (cAbs > cMax)
        {
          bestBasisFunctionIndex = k;
          cMax = cAbs;
        }
      }
      if (cMax < 0.0) break;

      LOGINFO(OSS() << "LARS: best candidate at iteration " << i << " is " << bestBasisFunctionIndex << " cMax=" << cMax);
      flagActiveFunctions[bestBasisFunctionIndex] = 1;
      leastSquaresMethod.update({bestBasisFunctionIndex}, marginalActiveFunctions, Indices());
      marginalActiveFunctions.add(bestBasisFunctionIndex);

      const UnsignedInteger predictorsSize = marginalActiveFunctions.getSize();

      // Build sign vector sC for the active set
      Point sC(predictorsSize);
      for (UnsignedInteger j = 0; j < predictorsSize; ++j)
        sC[j] = (correlations[marginalActiveFunctions[j]] < 0.0) ? -1.0 : 1.0;

      // Solve G_A * g_A = sC where G_A = Phi_A^T * W * Phi_A
      const Point ga1(leastSquaresMethod.solveNormalGram(sC));

      // Normalize: cNorm = 1 / sqrt(sC^T * g_A)
      const Scalar sCdotGa1 = sC.dot(ga1);
      if (!(sCdotGa1 > 0.0)) break;
      const Scalar cNorm = 1.0 / std::sqrt(sCdotGa1);
      const Point descentDirectionAk(cNorm * ga1);

      // Compute prediction direction u = Phi_A * descentDirectionAk
      const Matrix psiAk(leastSquaresMethod.computeDesign());
      const Point u(psiAk * descentDirectionAk);

      // Compute correlations of all basis functions with u: d_j = <phi_j, u>_W = Phi^T W u
      // Using PhiW = sqrt(W) Phi, this is PhiW^T sqrt(W) u
      Point d(basisSize_, 0.0);
      for (UnsignedInteger j = 0; j < basisSize_; ++j)
      {
        if (!isCandidate[j]) continue;
        Scalar dj = 0.0;
        for (UnsignedInteger s = 0; s < sampleSize; ++s)
          dj += PhiW(s, j) * weightSqrt[s] * u[s];
        d[j] = dj;
      }

      // Compute step size using Efron's min+ condition
      Scalar step = cMax / cNorm;
      for (UnsignedInteger k = 0; k < basisSize_; ++k)
      {
        if (flagActiveFunctions[k] == 1) continue;
        if (!isCandidate[k]) continue;
        const Scalar cI = correlations[k];
        const Scalar dK = d[k];
        // lhs: inactive correlation catches up from below
        if (std::abs(cNorm - dK) > 0.0)
        {
          const Scalar lhs = (cMax - cI) / (cNorm - dK);
          if (lhs > 0.0) step = std::min(step, lhs);
        }
        // rhs: inactive correlation catches up after sign flip
        if (std::abs(cNorm + dK) > 0.0)
        {
          const Scalar rhsStep = (cMax + cI) / (cNorm + dK);
          if (rhsStep > 0.0) step = std::min(step, rhsStep);
        }
      }

      // Update LARS prediction: mu += step * u
      for (UnsignedInteger s = 0; s < sampleSize; ++s)
        mu[s] += step * u[s];

      // Solve LS for the current active set to get coefficients and cross-validation
      coefficients = leastSquaresMethod.solve(rhs);
      crossValidationError = fitting_.run(leastSquaresMethod, marginalOutputSample);
      allIndicesHistory.add(marginalActiveFunctions);
      allCoefficientsHistory.add(coefficients);
      allErrorHistory.add(crossValidationError);

      if (crossValidationError < bestCrossValidationError)
      {
        bestCrossValidationError = crossValidationError;
        bestCoefficients = coefficients;
        bestSelection = marginalActiveFunctions;
        increaseCount = 0;
      }
      else
      {
        ++increaseCount;
        if (increaseCount >= consecutiveIncreases)
        {
          LOGINFO(OSS() << "LARS: cross-validation error=" << crossValidationError << " increased " << increaseCount << " consecutive times");
          break;
        }
        if (!(crossValidationError <= alpha * bestCrossValidationError))
        {
          LOGINFO(OSS() << "LARS: cross-validation error=" << crossValidationError << " larger than " << alpha << "*" << bestCrossValidationError);
          break;
        }
        if (crossValidationError > maximumError)
        {
          LOGINFO(OSS() << "LARS: cross-validation error=" << crossValidationError << " larger than " << maximumError);
          break;
        }
      }
      if (!(bestCrossValidationError >= errorThreshold))
      {
        LOGINFO(OSS() << "LARS: best cross-validation error=" << bestCrossValidationError << " smaller than threshold=" << errorThreshold);
        break;
      }
    }

    for (UnsignedInteger j = 0; j < bestSelection.getSize(); ++j)
    {
      const Scalar marginalAlpha_kj = bestCoefficients[j];
      if (std::abs(marginalAlpha_kj) > smallCoefficient)
      {
        const UnsignedInteger index = bestSelection[j];
        if (coefficientsMap.find(index) == coefficientsMap.end()) coefficientsMap[index] = Point(outputDimension, 0.0);
        coefficientsMap[index][outputIndex] = marginalAlpha_kj;
      }
    }
    historyCutPoints.add(allIndicesHistory.getSize());
  }

  std::map<UnsignedInteger, Point>::iterator iter;
  Indices I_k(0);
  Sample alpha_k(0, outputDimension);
  FunctionCollection Psi_k(0);
  for (iter = coefficientsMap.begin(); iter != coefficientsMap.end(); ++iter)
  {
    const UnsignedInteger i = iter->first;
    const Point currentCoefficient(iter->second);
    I_k.add(i);
    alpha_k.add(currentCoefficient);
    Psi_k.add(functions[i]);
  }
  result_ = FunctionalChaosResult(inputSample_, outputSample_, distribution_, transformation_,
                                  inverseTransformation_, basis_, I_k, alpha_k, Psi_k);
  result_.setIsLeastSquares(true);
  result_.setInvolvesModelSelection(true);
  result_.setSelectionHistory(allIndicesHistory, allCoefficientsHistory, historyCutPoints);
  result_.setErrorHistory(allErrorHistory, historyCutPoints);
  result_.setUseDomination(useDomination_);
}

/* Method to get/set the active functions */
Indices SparseExpansion::getActiveFunctions() const
{
  return activeFunctions_;
}

void SparseExpansion::setActiveFunctions(const Indices & activeFunctions)
{
  if (!activeFunctions.check(basisSize_)) throw InvalidArgumentException(HERE) << "Error: the active functions must have indices less than " << basisSize_;
  activeFunctions_ = activeFunctions;
  // The constant function (index 0) is always needed for the initial model
  if (!activeFunctions_.contains(0))
    activeFunctions_.add(0);
}

/* Method to get/set the fitting algorithm */
FittingAlgorithm SparseExpansion::getFittingAlgorithm() const
{
  return fitting_;
}

void SparseExpansion::setFittingAlgorithm(const FittingAlgorithm & fitting)
{
  fitting_ = fitting;
}

/* Method to get/set the selection method */
String SparseExpansion::getSelectionMethod() const
{
  return selectionMethod_;
}

void SparseExpansion::setSelectionMethod(const String & method)
{
  if (method != "OMP" && method != "LARS")
    throw InvalidArgumentException(HERE) << "Error: the selection method must be \"OMP\" or \"LARS\", here method=\"" << method << "\"";
  selectionMethod_ = method;
}

/* String converter */
String SparseExpansion::__repr__() const
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
String SparseExpansion::__str__(const String & ) const
{
  return OSS() << getClassName() << "("
         << "basis=" << basis_
         << ", basisSize=" << basisSize_
         << ", activeFunctions=" << activeFunctions_
         << ", methodName=" << methodName_
         << ")";
}

/* Method save() stores the object through the StorageManager */
void SparseExpansion::save(Advocate & adv) const
{
  FunctionalChaosAlgorithm::save(adv);
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "basisSize_", basisSize_ );
  adv.saveAttribute( "activeFunctions_", activeFunctions_ );
  adv.saveAttribute( "methodName_", methodName_ );
  adv.saveAttribute( "fitting_", fitting_ );
  adv.saveAttribute( "selectionMethod_", selectionMethod_ );
}


/* Method load() reloads the object from the StorageManager */
void SparseExpansion::load(Advocate & adv)
{
  FunctionalChaosAlgorithm::load(adv);
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "basisSize_", basisSize_ );
  adv.loadAttribute( "activeFunctions_", activeFunctions_ );
  adv.loadAttribute( "methodName_", methodName_ );
  adv.loadAttribute( "fitting_", fitting_ );
  adv.loadAttribute( "selectionMethod_", selectionMethod_ );
}


END_NAMESPACE_OPENTURNS
