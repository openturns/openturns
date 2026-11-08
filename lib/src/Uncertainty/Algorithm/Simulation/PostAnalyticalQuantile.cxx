//                                               -*- C++ -*-
/**
 *  @brief FORM/SORM quantile estimator
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
#include <cmath>
#include <algorithm>

#include "openturns/PostAnalyticalQuantile.hxx"
#include "openturns/QuantileRootSolver.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/Greater.hxx"
#include "openturns/AbdoRackwitz.hxx"
#include "openturns/FORM.hxx"
#include "openturns/SORM.hxx"
#include "openturns/FORMResult.hxx"
#include "openturns/SORMResult.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PostAnalyticalQuantile)

static const Factory<PostAnalyticalQuantile> Factory_PostAnalyticalQuantile;

namespace
{
/** Empirical inverse at level alpha, using the same convention as the
    fixed-sample quantile estimators. */
Scalar postAnalyticalPilotQuantile(const Sample & sample, const Scalar alpha)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The pilot sample is empty";
  const Sample sortedSample(sample.sort());
  UnsignedInteger index = static_cast<UnsignedInteger>(std::ceil(alpha * size));
  if (index > 0)
    --index;
  if (index >= size)
    index = size - 1;
  return sortedSample(index, 0);
}

/** Robust scale of the pilot sample used as the first bracket step. */
Scalar postAnalyticalPilotScale(const Sample & sample)
{
  const Scalar standardDeviation = sample.computeStandardDeviation()[0];
  const Scalar interquartileRange = sample.computeQuantile(0.75)[0] - sample.computeQuantile(0.25)[0];
  Scalar scale = std::max(standardDeviation, interquartileRange);
  if (!std::isfinite(scale) || !(scale > 0.0))
    scale = 1.0;
  return scale;
}

/** Student t-interval from independent replicate estimates. */
Interval postAnalyticalReplicateInterval(const Point & estimates,
                                         const Scalar beta,
                                         Scalar & estimate,
                                         Scalar & standardError)
{
  const UnsignedInteger size = estimates.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE)
        << "At least two replicates are required for a replicate interval, but replicateCount = " << size;
  Scalar mean = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    mean += estimates[i];
  mean /= size;
  Scalar variance = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar delta = estimates[i] - mean;
    variance += delta * delta;
  }
  variance /= (size - 1);
  standardError = std::sqrt(std::max(0.0, variance) / size);
  const Scalar t = DistFunc::qStudent(1.0 * (size - 1), 0.5 * (1.0 + beta));
  estimate = mean;
  const Scalar halfWidth = t * standardError;
  return Interval(Point(1, mean - halfWidth), Point(1, mean + halfWidth));
}

} // anonymous namespace

/* Default constructor */
PostAnalyticalQuantile::PostAnalyticalQuantile()
  : QuantileSimulationImplementation()
  , nearestPointAlgorithm_(AbdoRackwitz())
{
  // Nothing to do
}

/* Constructor with parameters */
PostAnalyticalQuantile::PostAnalyticalQuantile(const RandomVector & output,
                                               const Scalar alpha,
                                               const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
  , nearestPointAlgorithm_(AbdoRackwitz())
{
  // Nothing to do
}

/* Constructor with parameters and nearest point algorithm */
PostAnalyticalQuantile::PostAnalyticalQuantile(const OptimizationAlgorithm & nearestPointAlgorithm,
                                               const RandomVector & output,
                                               const Scalar alpha,
                                               const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
  , nearestPointAlgorithm_(nearestPointAlgorithm)
{
  // Nothing to do
}

/* Virtual constructor */
PostAnalyticalQuantile * PostAnalyticalQuantile::clone() const
{
  return new PostAnalyticalQuantile(*this);
}

/* Nearest point algorithm accessor */
void PostAnalyticalQuantile::setNearestPointAlgorithm(const OptimizationAlgorithm & nearestPointAlgorithm)
{
  nearestPointAlgorithm_ = nearestPointAlgorithm;
}

OptimizationAlgorithm PostAnalyticalQuantile::getNearestPointAlgorithm() const
{
  return nearestPointAlgorithm_;
}

/* Analytical method accessor */
void PostAnalyticalQuantile::setAnalyticalMethod(const AnalyticalMethod method)
{
  if ((method != FORM_APPROXIMATION) && (method != SORM_APPROXIMATION))
    throw InvalidArgumentException(HERE) << "Unknown analytical method";
  analyticalMethod_ = method;
}

PostAnalyticalQuantile::AnalyticalMethod PostAnalyticalQuantile::getAnalyticalMethod() const
{
  return analyticalMethod_;
}

String PostAnalyticalQuantile::getAnalyticalMethodDescription() const
{
  switch (analyticalMethod_)
  {
    case FORM_APPROXIMATION:
      return "FORM";
    case SORM_APPROXIMATION:
      return "SORM";
    default:
      break;
  }
  return "Unknown";
}

/* SORM formula accessor */
void PostAnalyticalQuantile::setSORMFormula(const SORMFormula formula)
{
  if ((formula != BREITUNG) && (formula != HOHENBICHLER) && (formula != TVEDT))
    throw InvalidArgumentException(HERE) << "Unknown SORM formula";
  sormFormula_ = formula;
}

PostAnalyticalQuantile::SORMFormula PostAnalyticalQuantile::getSORMFormula() const
{
  return sormFormula_;
}

String PostAnalyticalQuantile::getSORMFormulaDescription() const
{
  switch (sormFormula_)
  {
    case BREITUNG:
      return "Breitung";
    case HOHENBICHLER:
      return "Hohenbichler";
    case TVEDT:
      return "Tvedt";
    default:
      break;
  }
  return "Unknown";
}

/* Importance-sampling refinement accessor */
void PostAnalyticalQuantile::setRefinementEnabled(const Bool refinementEnabled)
{
  refinementEnabled_ = refinementEnabled;
}

Bool PostAnalyticalQuantile::getRefinementEnabled() const
{
  return refinementEnabled_;
}

/* Refinement replicate count accessor */
void PostAnalyticalQuantile::setReplicateCount(const UnsignedInteger replicateCount)
{
  if (replicateCount == 0)
    throw InvalidArgumentException(HERE)
        << "The replicate count must be positive";
  replicateCount_ = replicateCount;
}

UnsignedInteger PostAnalyticalQuantile::getReplicateCount() const
{
  return replicateCount_;
}

/* Root-finding tolerance accessor */
void PostAnalyticalQuantile::setRootTolerance(const Scalar tolerance)
{
  if (!std::isfinite(tolerance) || !(tolerance > 0.0))
    throw InvalidArgumentException(HERE)
        << "The root-finding tolerance must be finite and positive, but tolerance = "
        << tolerance;
  rootTolerance_ = tolerance;
}

Scalar PostAnalyticalQuantile::getRootTolerance() const
{
  return rootTolerance_;
}

/* Maximum root-finding iteration accessor */
void PostAnalyticalQuantile::setMaximumRootIteration(const UnsignedInteger maximumIteration)
{
  if (maximumIteration == 0)
    throw InvalidArgumentException(HERE)
        << "The maximum root-finding iteration count must be positive";
  maximumRootIteration_ = maximumIteration;
}

UnsignedInteger PostAnalyticalQuantile::getMaximumRootIteration() const
{
  return maximumRootIteration_;
}

/* Pilot sample size accessor */
void PostAnalyticalQuantile::setPilotSampleSize(const UnsignedInteger pilotSampleSize)
{
  if (pilotSampleSize == 0)
    throw InvalidArgumentException(HERE)
        << "The pilot sample size must be positive";
  pilotSampleSize_ = pilotSampleSize;
}

UnsignedInteger PostAnalyticalQuantile::getPilotSampleSize() const
{
  return pilotSampleSize_;
}

/* Check the inputs without requiring a sampling size */
void PostAnalyticalQuantile::checkInputs() const
{
  UnsignedInteger dimension = 0;
  try
  {
    dimension = output_.getDimension();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE)
        << "The output random vector must be set before running the algorithm";
  }
  if (dimension != 1)
    throw InvalidArgumentException(HERE)
        << "The quantile simulation algorithm only supports a scalar output random vector, but the output dimension is "
        << dimension << ". Select a marginal with output.getMarginal(i), or define a scalar function h and "
        << "use CompositeRandomVector(h, output).";
  if (!(alpha_ > 0.0 && alpha_ < 1.0))
    throw InvalidArgumentException(HERE)
        << "The quantile level must be in (0, 1), but alpha = " << alpha_;
  if (!(beta_ > 0.0 && beta_ < 1.0))
    throw InvalidArgumentException(HERE)
        << "The confidence level must be in (0, 1), but beta = " << beta_;
  if (refinementEnabled_)
  {
    if (sampleSize_ < 2)
      throw InvalidArgumentException(HERE)
          << "The importance-sampling refinement requires a sample size of at least 2, but sampleSize = "
          << sampleSize_;
    if (replicateCount_ < 2)
      throw InvalidArgumentException(HERE)
          << "The importance-sampling refinement requires at least two replicates for an asymptotic interval, but replicateCount = "
          << replicateCount_;
  }
}

/* Run FORM or SORM at the given threshold */
PostAnalyticalQuantile::AnalyticalEvaluation
PostAnalyticalQuantile::runAnalytical(const RandomVector & compositeOutput,
                                      const Scalar threshold) const
{
  const ThresholdEvent event(compositeOutput, Greater(), threshold);
  const Distribution inputDistribution(compositeOutput.getAntecedent().getDistribution());
  OptimizationAlgorithm solver(*(nearestPointAlgorithm_.getImplementation()));
  const UnsignedInteger dimension = inputDistribution.getDimension();
  Bool hasStartingPoint = false;
  try
  {
    hasStartingPoint = solver.getStartingPoint().getDimension() == dimension;
  }
  catch (const Exception &)
  {
    hasStartingPoint = false;
  }
  if (!hasStartingPoint)
    solver.setStartingPoint(inputDistribution.getMean());

  AnalyticalEvaluation evaluation;
  if (analyticalMethod_ == FORM_APPROXIMATION)
  {
    FORM form(solver, event);
    form.run();
    const FORMResult formResult(form.getResult());
    evaluation.probability = formResult.getEventProbability();
    evaluation.formProbability = evaluation.probability;
    evaluation.designPoint = formResult.getStandardSpaceDesignPoint();
    evaluation.reliabilityIndex = formResult.getHasoferReliabilityIndex();
  }
  else
  {
    SORM sorm(solver, event);
    sorm.run();
    const SORMResult sormResult(sorm.getResult());
    switch (sormFormula_)
    {
      case BREITUNG:
        evaluation.probability = sormResult.getEventProbabilityBreitung();
        break;
      case HOHENBICHLER:
        evaluation.probability = sormResult.getEventProbabilityHohenbichler();
        break;
      case TVEDT:
        evaluation.probability = sormResult.getEventProbabilityTvedt();
        break;
      default:
        throw InvalidArgumentException(HERE) << "Unknown SORM formula";
    }
    Scalar formProbability = standardDistribution_.getMarginal(0).computeCDF(Point(1, -sormResult.getHasoferReliabilityIndex()));
    if (sormResult.getIsStandardPointOriginInFailureSpace())
      formProbability = 1.0 - formProbability;
    evaluation.formProbability = formProbability;
    evaluation.designPoint = sormResult.getStandardSpaceDesignPoint();
    evaluation.reliabilityIndex = sormResult.getHasoferReliabilityIndex();
    if (formProbability > 0.0)
    {
      evaluation.hasCurvatureCorrection = true;
      evaluation.curvatureCorrection = evaluation.probability / formProbability;
    }
  }
  return evaluation;
}

/* Weighted empirical probability of a fixed importance sample.  The
   weights are the target-to-proposal density ratios and are not
   normalized: their average is an unbiased estimate of one. */
Scalar PostAnalyticalQuantile::computeImportanceProbability(const Point & values,
                                                            const Point & weights,
                                                            const Scalar threshold) const
{
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
    if (values[i] > threshold)
      sum += weights[i];
  return sum / values.getSize();
}

/* Run the analytical root-finding experiment */
QuantileSimulationResult PostAnalyticalQuantile::runFixedSample()
{
  // Threshold events require a composite random vector.
  RandomVector compositeOutput(output_);
  if (!compositeOutput.isComposite())
    compositeOutput = CompositeRandomVector(IdentityFunction(1), output_);

  // Build the standard-space limit state once.
  const ThresholdEvent referenceEvent(compositeOutput, Greater(), 0.0);
  const StandardEvent standardEvent(referenceEvent);
  standardFunction_ = standardEvent.getImplementation()->getFunction();
  standardDistribution_ = standardEvent.getImplementation()->getAntecedent().getDistribution();

  // Pilot sample used only to start and scale the bracket.
  const Sample pilotSample(output_.getSample(pilotSampleSize_));
  const Scalar start = postAnalyticalPilotQuantile(pilotSample, alpha_);
  const Scalar step = postAnalyticalPilotScale(pilotSample);

  const Scalar target = 1.0 - alpha_;
  UnsignedInteger probabilityEvaluationCount = 0;
  Sample history(0, 2);
  const QuantileRootSolver::FunctionType objective =
      [this, compositeOutput, target, &probabilityEvaluationCount, &history](const Scalar threshold) -> Scalar
  {
    const AnalyticalEvaluation evaluation = runAnalytical(compositeOutput, threshold);
    ++probabilityEvaluationCount;
    history.add(Point({1.0 * probabilityEvaluationCount, threshold}));
    return evaluation.probability - target;
  };

  const QuantileRootSolver solver(rootTolerance_, maximumRootIteration_);
  Scalar lowerBound = 0.0;
  Scalar upperBound = 0.0;
  Scalar lowerValue = 0.0;
  Scalar upperValue = 0.0;
  solver.bracket(objective, start, lowerBound, upperBound, lowerValue, upperValue,
                 step, ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultBracketExpansion"));
  Scalar residual = 0.0;
  Interval bracket;
  UnsignedInteger iterationCount = 0;
  Bool converged = false;
  const Scalar estimate = solver.solveBisection(objective, lowerBound, upperBound,
                                                lowerValue, upperValue, residual,
                                                bracket, iterationCount, converged);
  if (!converged)
    throw NotDefinedException(HERE)
        << "The analytical quantile root did not converge within "
        << maximumRootIteration_ << " iterations";

  // Analytical diagnostics at the returned quantile.
  AnalyticalEvaluation finalEvaluation = runAnalytical(compositeOutput, estimate);
  ++probabilityEvaluationCount;

  Scalar outputEstimate = estimate;
  Scalar standardError = 0.0;
  Interval interval(Point(1, estimate), Point(1, estimate));
  QuantileSimulationResultImplementation::CoverageType coverage = QuantileSimulationResultImplementation::COVERAGE_NONE;
  Scalar refinedProbability = 0.0;
  Scalar refinementStandardError = 0.0;
  UnsignedInteger outerSampling = 1;
  Point replicateEstimates;
  // The analytical probability itself is deterministic, so its effective
  // sample size is one.  The refinement draws replicateCount_ independent
  // samples of sampleSize_ points, and the pilot sample is evaluated in
  // both modes.  Report the evaluations actually performed by the sampling
  // part of the algorithm.
  const UnsignedInteger refinementSampleSize =
      (refinementEnabled_ && (sampleSize_ > 0)) ? replicateCount_ * sampleSize_ : 0;
  const UnsignedInteger effectiveSampleSize = refinementEnabled_ ? refinementSampleSize : 1;

  if (refinementEnabled_)
  {
    // Importance-sampling refinement around the analytical design
    // point.  Each replicate draws its own sample, then solves the
    // weighted empirical root with that fixed sample: the root is
    // deterministic given the replicate, and independent replicates
    // provide the asymptotic interval.
    const Point designPoint(finalEvaluation.designPoint);
    replicateEstimates = Point(replicateCount_);
    Point replicateProbabilities(replicateCount_);
    UnsignedInteger totalIterationCount = 0;
    Scalar unionLower = SpecFunc::MaxScalar;
    Scalar unionUpper = -SpecFunc::MaxScalar;
    Bool allConverged = true;
    for (UnsignedInteger replicate = 0; replicate < replicateCount_; ++replicate)
    {
      const Sample proposalSample(standardDistribution_.getSample(sampleSize_));
      Point values(sampleSize_);
      Point weights(sampleSize_);
      for (UnsignedInteger i = 0; i < sampleSize_; ++i)
      {
        const Point proposalPoint(proposalSample[i]);
        Point point(proposalPoint);
        point += designPoint;
        values[i] = standardFunction_(point)[0];
        // Importance-sampling weight f(x) / f(x - a), where f is the
        // standard distribution density and a is the design point.
        weights[i] = std::exp(standardDistribution_.computeLogPDF(point)
                              - standardDistribution_.computeLogPDF(proposalPoint));
      }
      UnsignedInteger replicateEvaluationCount = 0;
      const QuantileRootSolver::FunctionType replicateObjective =
          [this, &values, &weights, target, &replicateEvaluationCount](const Scalar threshold) -> Scalar
      {
        ++replicateEvaluationCount;
        return computeImportanceProbability(values, weights, threshold) - target;
      };
      const QuantileRootSolver replicateSolver(rootTolerance_, maximumRootIteration_);
      Scalar replicateLower = 0.0;
      Scalar replicateUpper = 0.0;
      Scalar replicateLowerValue = 0.0;
      Scalar replicateUpperValue = 0.0;
      replicateSolver.bracket(replicateObjective, estimate, replicateLower, replicateUpper,
                              replicateLowerValue, replicateUpperValue, step,
                              ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultBracketExpansion"));
      Interval replicateBracket;
      UnsignedInteger replicateIterationCount = 0;
      Bool replicateConverged = false;
      const Scalar replicateEstimate = replicateSolver.solveBisection(replicateObjective,
                                                                       replicateLower, replicateUpper,
                                                                       replicateLowerValue, replicateUpperValue,
                                                                       residual, replicateBracket,
                                                                       replicateIterationCount, replicateConverged);
      replicateEstimates[replicate] = replicateEstimate;
      replicateProbabilities[replicate] = computeImportanceProbability(values, weights, replicateEstimate);
      totalIterationCount += replicateIterationCount;
      probabilityEvaluationCount += replicateEvaluationCount;
      unionLower = std::min(unionLower, replicateBracket.getLowerBound()[0]);
      unionUpper = std::max(unionUpper, replicateBracket.getUpperBound()[0]);
      if (!replicateConverged)
        allConverged = false;
    }
    if (!allConverged)
      throw NotDefinedException(HERE)
          << "At least one importance-sampling replicate did not converge within "
          << maximumRootIteration_ << " iterations";
    outputEstimate = 0.0;
    refinedProbability = 0.0;
    for (UnsignedInteger replicate = 0; replicate < replicateCount_; ++replicate)
    {
      outputEstimate += replicateEstimates[replicate];
      refinedProbability += replicateProbabilities[replicate];
    }
    outputEstimate /= replicateCount_;
    refinedProbability /= replicateCount_;
    interval = postAnalyticalReplicateInterval(replicateEstimates, beta_, outputEstimate, refinementStandardError);
    standardError = refinementStandardError;
    bracket = Interval(Point(1, unionLower), Point(1, unionUpper));
    iterationCount = totalIterationCount;
    coverage = QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC;
    outerSampling = replicateCount_;

    // Refresh the analytical diagnostics at the refined quantile.
    finalEvaluation = runAnalytical(compositeOutput, outputEstimate);
    ++probabilityEvaluationCount;
  }

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(outputEstimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(QuantileSimulationResultImplementation::EMPIRICAL_INVERSE);
  result.setCoverageType(coverage);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(1.0 * effectiveSampleSize);
  result.setSampleSize(refinementSampleSize + pilotSampleSize_);
  result.setBlockLength(0);
  result.setIsFixedSample(true);
  result.setOuterSampling(outerSampling);
  result.setBlockSize(getBlockSize());
  result.setConvergenceHistory(history);
  result.setDirectionCount(0);
  result.setRootBracket(bracket);
  result.setRootIterationCount(iterationCount);
  result.setProbabilityEvaluationCount(probabilityEvaluationCount);
  result.setRootSolverType(QuantileSimulationResultImplementation::BISECTION);
  result.setCommonRandomNumbers(true);
  result.setRandomStreamCapability(false);
  result.setDelegatedToDirectExtraction(false);
  result.setAnalyticalProbability(finalEvaluation.probability);
  result.setDesignPoint(finalEvaluation.designPoint);
  result.setReliabilityIndex(finalEvaluation.reliabilityIndex);
  if (finalEvaluation.hasCurvatureCorrection)
    result.setCurvatureCorrection(finalEvaluation.curvatureCorrection);
  if (refinementEnabled_)
  {
    result.setRefinedProbability(refinedProbability);
    result.setReplicateCount(replicateCount_);
    result.setReplicateEstimates(replicateEstimates);
  }
  result.setRefinementStandardError(refinementStandardError);
  return result;
}

/* String converter */
String PostAnalyticalQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << PostAnalyticalQuantile::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " analyticalMethod=" << getAnalyticalMethodDescription()
      << " sormFormula=" << getSORMFormulaDescription()
      << " refinementEnabled=" << refinementEnabled_
      << " replicateCount=" << replicateCount_
      << " sampleSize=" << sampleSize_
      << " rootTolerance=" << rootTolerance_
      << " maximumRootIteration=" << maximumRootIteration_
      << " pilotSampleSize=" << pilotSampleSize_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void PostAnalyticalQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("nearestPointAlgorithm_", nearestPointAlgorithm_);
  adv.saveAttribute("analyticalMethod_", static_cast<UnsignedInteger>(analyticalMethod_));
  adv.saveAttribute("sormFormula_", static_cast<UnsignedInteger>(sormFormula_));
  adv.saveAttribute("refinementEnabled_", refinementEnabled_);
  adv.saveAttribute("replicateCount_", replicateCount_);
  adv.saveAttribute("rootTolerance_", rootTolerance_);
  adv.saveAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.saveAttribute("pilotSampleSize_", pilotSampleSize_);
}

/* Method load() reloads the object from the StorageManager */
void PostAnalyticalQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("nearestPointAlgorithm_", nearestPointAlgorithm_);
  UnsignedInteger analyticalMethod = static_cast<UnsignedInteger>(analyticalMethod_);
  adv.loadAttribute("analyticalMethod_", analyticalMethod);
  analyticalMethod_ = static_cast<AnalyticalMethod>(analyticalMethod);
  UnsignedInteger sormFormula = static_cast<UnsignedInteger>(sormFormula_);
  adv.loadAttribute("sormFormula_", sormFormula);
  sormFormula_ = static_cast<SORMFormula>(sormFormula);
  adv.loadAttribute("refinementEnabled_", refinementEnabled_);
  adv.loadAttribute("replicateCount_", replicateCount_);
  adv.loadAttribute("rootTolerance_", rootTolerance_);
  adv.loadAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.loadAttribute("pilotSampleSize_", pilotSampleSize_);
}

END_NAMESPACE_OPENTURNS
