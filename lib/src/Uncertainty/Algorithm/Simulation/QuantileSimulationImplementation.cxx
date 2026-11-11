//                                               -*- C++ -*-
/**
 *  @brief Base class for quantile simulation algorithms
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
#include <chrono>
#include <algorithm>

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/TensorProductExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuantileSimulationImplementation)

static const Factory<QuantileSimulationImplementation> Factory_QuantileSimulationImplementation;

namespace
{
Bool isCoverageSatisfied(const QuantileSimulationResultImplementation::CoverageType required,
                         const QuantileSimulationResultImplementation::CoverageType achieved)
{
  if (required == QuantileSimulationResultImplementation::COVERAGE_NONE)
    return true;
  if (required == QuantileSimulationResultImplementation::COVERAGE_EXACT)
    return achieved == QuantileSimulationResultImplementation::COVERAGE_EXACT;
  if (required == QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID)
    return achieved == QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID;
  if (required == QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC)
    return (achieved == QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC)
           || (achieved == QuantileSimulationResultImplementation::COVERAGE_EXACT);
  return false;
}

String coverageDescription(const QuantileSimulationResultImplementation::CoverageType coverage)
{
  switch (coverage)
  {
    case QuantileSimulationResultImplementation::COVERAGE_EXACT:
      return "exact";
    case QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC:
      return "asymptotic";
    case QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID:
      return "anytime-valid";
    case QuantileSimulationResultImplementation::COVERAGE_NONE:
      return "none";
    default:
      break;
  }
  return "unknown";
}
}

/* Default constructor */
QuantileSimulationImplementation::QuantileSimulationImplementation()
  : SimulationAlgorithm()
  , output_(RandomVector(Uniform()))
  , result_()
{
  // Nothing to do
}

/* Standard constructor */
QuantileSimulationImplementation::QuantileSimulationImplementation(const RandomVector & output,
                                                                   const Scalar alpha,
                                                                   const Scalar beta)
  : SimulationAlgorithm()
  , result_()
{
  setOutput(output);
  setAlpha(alpha);
  setConfidenceLevel(beta);
}

/* Virtual constructor */
QuantileSimulationImplementation * QuantileSimulationImplementation::clone() const
{
  return new QuantileSimulationImplementation(*this);
}

/* Output random vector accessor */
void QuantileSimulationImplementation::setOutput(const RandomVector & output)
{
  UnsignedInteger dimension = 0;
  try
  {
    dimension = output.getDimension();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "The output random vector must be scalar, but its dimension cannot be retrieved";
  }
  if (dimension != 1)
    throw InvalidArgumentException(HERE)
        << "The quantile simulation algorithm only supports a scalar output random vector, but the output dimension is "
        << dimension << ". Select a marginal with output.getMarginal(i), or define a scalar function h and "
        << "use CompositeRandomVector(h, output).";
  output_ = output;
}

RandomVector QuantileSimulationImplementation::getOutput() const
{
  return output_;
}

/* Quantile level accessor */
void QuantileSimulationImplementation::setAlpha(const Scalar alpha)
{
  if (!(alpha > 0.0 && alpha < 1.0))
    throw InvalidArgumentException(HERE)
        << "The quantile level must be in (0, 1), but alpha = " << alpha;
  alpha_ = alpha;
}

Scalar QuantileSimulationImplementation::getAlpha() const
{
  return alpha_;
}

/* Confidence level accessor */
void QuantileSimulationImplementation::setConfidenceLevel(const Scalar beta)
{
  if (!(beta > 0.0 && beta < 1.0))
    throw InvalidArgumentException(HERE)
        << "The confidence level must be in (0, 1), but beta = " << beta;
  beta_ = beta;
}

Scalar QuantileSimulationImplementation::getConfidenceLevel() const
{
  return beta_;
}

/* Sample size accessor */
void QuantileSimulationImplementation::setSampleSize(const UnsignedInteger sampleSize)
{
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << "The sample size must be positive";
  sampleSize_ = sampleSize;
  const UnsignedInteger blockSize = std::max<UnsignedInteger>(1, getBlockSize());
  setMaximumOuterSampling((sampleSize + blockSize - 1) / blockSize);
}

UnsignedInteger QuantileSimulationImplementation::getSampleSize() const
{
  return sampleSize_;
}

/* Estimator type accessor */
void QuantileSimulationImplementation::setEstimatorType(const EstimatorType type)
{
  if ((type != QuantileSimulationResultImplementation::EMPIRICAL_INVERSE)
      && (type != QuantileSimulationResultImplementation::INTERPOLATED))
    throw InvalidArgumentException(HERE) << "Unknown estimator type";
  estimatorType_ = type;
}

QuantileSimulationImplementation::EstimatorType QuantileSimulationImplementation::getEstimatorType() const
{
  return estimatorType_;
}

/* Required coverage accessor */
void QuantileSimulationImplementation::setRequiredCoverage(const CoverageType coverage)
{
  if ((coverage != QuantileSimulationResultImplementation::COVERAGE_EXACT)
      && (coverage != QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC)
      && (coverage != QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID)
      && (coverage != QuantileSimulationResultImplementation::COVERAGE_NONE))
    throw InvalidArgumentException(HERE) << "Unknown coverage type";
  requiredCoverage_ = coverage;
}

QuantileSimulationImplementation::CoverageType QuantileSimulationImplementation::getRequiredCoverage() const
{
  return requiredCoverage_;
}

/* Performs the actual computation */
void QuantileSimulationImplementation::run()
{
  checkInputs();
  const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  QuantileSimulationResult result(runFixedSample());
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  result.setTimeDuration(std::chrono::duration<Scalar>(t1 - t0).count());
  checkCoverage(result);
  // Publish the result only after the coverage check: a failed run must
  // not replace the result of the previous successful run.
  result_ = result;
}

/* Run a fixed-sample experiment */
QuantileSimulationResult QuantileSimulationImplementation::runFixedSample()
{
  throw NotYetImplementedException(HERE) << "In QuantileSimulationImplementation::runFixedSample()";
}

/* Check the input parameters */
void QuantileSimulationImplementation::checkInputs() const
{
  UnsignedInteger dimension = 0;
  try
  {
    dimension = output_.getDimension();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "The output random vector must be set before running the algorithm";
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
  if (sampleSize_ == 0)
    throw InvalidArgumentException(HERE) << "The sample size must be set before running the algorithm";
}

/* Check the experiment against the output input distribution */
UnsignedInteger QuantileSimulationImplementation::checkExperiment(const WeightedExperiment & experiment) const
{
  UnsignedInteger inputDimension = 0;
  try
  {
    inputDimension = output_.isComposite()
                     ? output_.getAntecedent().getDimension()
                     : output_.getDimension();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "The input dimension of the output random vector cannot be retrieved";
  }
  if (experiment.getDistribution().getDimension() != inputDimension)
    throw InvalidArgumentException(HERE)
        << "The experiment distribution dimension (" << experiment.getDistribution().getDimension()
        << ") does not match the input dimension (" << inputDimension << ")";
  // A stochastic experiment must sample from the input distribution of the
  // output.  Otherwise the estimate describes the experiment distribution
  // and the coverage label would be meaningless.  Fixed designs are
  // diagnostic and may carry arbitrary supplied values;
  // TensorProductExperiment reports a block-independent distribution built
  // from its marginals, whose structural equality with the input
  // distribution is not a reliable test.
  if (experiment.isRandom()
      && (dynamic_cast<const TensorProductExperiment *>(experiment.getImplementation().get()) == nullptr))
  {
    Distribution inputDistribution;
    Bool hasInputDistribution = true;
    try
    {
      inputDistribution = output_.isComposite()
                          ? output_.getAntecedent().getDistribution()
                          : output_.getDistribution();
    }
    catch (const Exception &)
    {
      hasInputDistribution = false;
    }
    if (hasInputDistribution)
    {
      Bool sameDistribution = true;
      try
      {
        sameDistribution = (experiment.getDistribution() == inputDistribution);
      }
      catch (const Exception &)
      {
        // Some distributions do not implement structural equality:
        // keep the dimension check as the fallback.
      }
      if (!sameDistribution)
        throw InvalidArgumentException(HERE)
            << "The experiment distribution does not match the input distribution of the output random vector";
    }
  }
  return inputDimension;
}

/* Check the required coverage against the achieved coverage */
void QuantileSimulationImplementation::checkCoverage(const QuantileSimulationResult & result) const
{
  if (!isCoverageSatisfied(requiredCoverage_, result.getCoverageType()))
    throw InvalidArgumentException(HERE)
        << "The required coverage (" << coverageDescription(requiredCoverage_)
        << ") cannot be satisfied by the achieved coverage ("
        << coverageDescription(result.getCoverageType()) << ")";
}

/* Result accessor */
QuantileSimulationResult QuantileSimulationImplementation::getResult() const
{
  return result_;
}

void QuantileSimulationImplementation::setResult(const QuantileSimulationResult & result)
{
  result_ = result;
}

/* String converter */
String QuantileSimulationImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << QuantileSimulationImplementation::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_
      << " maximumOuterSampling=" << getMaximumOuterSampling()
      << " blockSize=" << getBlockSize();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void QuantileSimulationImplementation::save(Advocate & adv) const
{
  SimulationAlgorithm::save(adv);
  adv.saveAttribute("output_", output_);
  adv.saveAttribute("alpha_", alpha_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("sampleSize_", sampleSize_);
  adv.saveAttribute("estimatorType_", static_cast<UnsignedInteger>(estimatorType_));
  adv.saveAttribute("requiredCoverage_", static_cast<UnsignedInteger>(requiredCoverage_));
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void QuantileSimulationImplementation::load(Advocate & adv)
{
  SimulationAlgorithm::load(adv);
  adv.loadAttribute("output_", output_);
  adv.loadAttribute("alpha_", alpha_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("sampleSize_", sampleSize_);
  UnsignedInteger estimatorType = static_cast<UnsignedInteger>(estimatorType_);
  adv.loadAttribute("estimatorType_", estimatorType);
  estimatorType_ = static_cast<EstimatorType>(estimatorType);
  UnsignedInteger requiredCoverage = static_cast<UnsignedInteger>(requiredCoverage_);
  adv.loadAttribute("requiredCoverage_", requiredCoverage);
  requiredCoverage_ = static_cast<CoverageType>(requiredCoverage);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
