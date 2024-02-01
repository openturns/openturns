//                                               -*- C++ -*-
/**
 *  @brief Estimation by matching quantiles
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
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include "openturns/QuantileMatchingFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/LeastSquaresProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuantileMatchingFactory)

static const Factory<QuantileMatchingFactory> Factory_QuantileMatchingFactory;

/* Default constructor */
QuantileMatchingFactory::QuantileMatchingFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
QuantileMatchingFactory::QuantileMatchingFactory(const Distribution & distribution,
    const Point & probabilities,
    const Interval & optimizationBounds)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
  , probabilities_(probabilities)
  , optimizationBounds_(optimizationBounds)
{
  setProbabilities(probabilities);
  LeastSquaresProblem problem(SymbolicFunction("x", "x^2"));
  if (optimizationBounds.getDimension())
    problem.setBounds(Interval(1));
  solver_ = OptimizationAlgorithm::Build(problem);

  // Initialize optimization solver parameter using the ResourceMap
  solver_.setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("QuantileMatchingFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("QuantileMatchingFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("QuantileMatchingFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("QuantileMatchingFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("QuantileMatchingFactory-MaximumConstraintError"));
}

/* Virtual constructor */
QuantileMatchingFactory * QuantileMatchingFactory::clone() const
{
  return new QuantileMatchingFactory(*this);
}

/* String converter */
String QuantileMatchingFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName()
      << " distribution=" << distribution_
      << " solver=" << solver_;
  return oss;
}

/* String converter */
String QuantileMatchingFactory::__str__(const String & ) const
{
  return this->getClassName();
}

class QuantileMatchingEvaluation : public EvaluationImplementation
{
public:
  QuantileMatchingEvaluation(const Point & refQuantiles,
                             const Distribution & distribution,
                             const Point & probabilities,
                             const Point & knownParameterValues,
                             const Indices & knownParameterIndices)
    : EvaluationImplementation()
    , refQuantiles_(refQuantiles)
    , distribution_(distribution)
    , probabilities_(probabilities)
    , knownParameterValues_(knownParameterValues)
    , knownParameterIndices_(knownParameterIndices)
  {
    // build the unknown indices
    const UnsignedInteger parameterDimension = distribution.getParameter().getSize();
    for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
    {
      if (!knownParameterIndices_.contains(j))
        unknownParameterIndices_.add(j);
    }
  }

  QuantileMatchingEvaluation * clone() const
  {
    return new QuantileMatchingEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return probabilities_.getSize();
  }

  UnsignedInteger getOutputDimension() const
  {
    return getInputDimension();
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(getInputDimension(), "theta");
  }

  Description getOutputDescription() const
  {
    return Description(getOutputDimension(), "r");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Point operator() (const Point & parameter) const
  {
    const UnsignedInteger parameterDimension = distribution_.getParameterDimension();
    Point effectiveParameter(parameterDimension);
    // set unknown values
    const UnsignedInteger unknownParameterSize = unknownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < unknownParameterSize; ++ j)
      effectiveParameter[unknownParameterIndices_[j]] = parameter[j];

    // set known values
    const UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
      effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
    Distribution distribution(distribution_);
    try
    {
      distribution.setParameter(effectiveParameter);
    }
    catch (const Exception &)
    {
      return Point(getOutputDimension(), SpecFunc::MaxScalar);
    }

    // compute deltas between quantiles
    const UnsignedInteger estimatedParameterSize = probabilities_.getSize();
    Point result(estimatedParameterSize);
    for (UnsignedInteger j = 0; j < estimatedParameterSize; ++ j)
      result[j] = refQuantiles_[j] - distribution.computeScalarQuantile(probabilities_[j]);
    return result;
  }

private:
  Point refQuantiles_;
  Distribution distribution_;
  Point probabilities_;
  Point knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};


Distribution QuantileMatchingFactory::build(const Point & parameter) const
{
  Distribution result(distribution_);
  Point parameter2(parameter);
  // set known values
  const UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
  {
    parameter2[knownParameterIndices_[j]] = knownParameterValues_[j];
  }
  result.setParameter(parameter2);
  return result;
}


Distribution QuantileMatchingFactory::build() const
{
  return build(distribution_.getParameter());
}

Distribution QuantileMatchingFactory::build(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const UnsignedInteger estimatedParameterSize = probabilities_.getSize();

  Point refQuantiles(estimatedParameterSize);
  for (UnsignedInteger j = 0; j < estimatedParameterSize; ++ j)
    refQuantiles[j] = sample.computeQuantile(probabilities_[j])[0];

  Distribution result(buildFromQuantiles(refQuantiles));
  result.setDescription(sample.getDescription());
  return result;
}

/** Build a distribution from its moments */
Distribution QuantileMatchingFactory::buildFromQuantiles(const Point & quantiles) const
{
  const UnsignedInteger parameterDimension = distribution_.getParameterDimension();
  if (quantiles.getSize() + knownParameterValues_.getSize() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Expected " << parameterDimension - knownParameterValues_.getSize()
                                         << " quantiles to estimate the distribution";

  if (!quantiles.isIncreasing())
    throw InvalidArgumentException(HERE) << "Provided quantiles are not increasing";

  if (probabilities_.getSize() + knownParameterValues_.getSize() != parameterDimension)
    throw InvalidArgumentException(HERE) << "The total of known parameters size (" << parameterDimension << ") "
                                         << "and probability levels size (" << probabilities_.getSize() << ") match the model parameter dimension ("
                                         << parameterDimension << ")";

  if (optimizationBounds_.getDimension() && (optimizationBounds_.getDimension() != probabilities_.getSize()))
    throw InvalidArgumentException(HERE) << "The bounds dimension must match the probabilities size (" << probabilities_.getSize() << ")";

  // Define evaluation
  const QuantileMatchingEvaluation quantileMatchingWrapper(quantiles, distribution_, probabilities_, knownParameterValues_, knownParameterIndices_);
  Function quantilesObjective(quantileMatchingWrapper.clone());

  // Define optimization problem
  LeastSquaresProblem problem(quantilesObjective);
  problem.setBounds(optimizationBounds_);
  OptimizationAlgorithm solver(solver_);

  // define starting point
  Point effectiveParameter(distribution_.getParameter());
  Point parameter;
  for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
  {
    if (!knownParameterIndices_.contains(j))
      parameter.add(effectiveParameter[j]);
  }

  // clip starting point
  if (optimizationBounds_.getDimension() && !optimizationBounds_.contains(parameter))
  {
    const Point lb(optimizationBounds_.getLowerBound());
    const Point ub(optimizationBounds_.getUpperBound());
    const Interval::BoolCollection finiteLowerBound(optimizationBounds_.getFiniteLowerBound());
    const Interval::BoolCollection finiteUpperBound(optimizationBounds_.getFiniteUpperBound());
    for (UnsignedInteger j = 0; j < parameter.getDimension(); ++ j)
    {
      if (finiteLowerBound[j])
        parameter[j] = std::max(parameter[j], lb[j]);
      if (finiteUpperBound[j])
        parameter[j] = std::min(parameter[j], ub[j]);
    }
  }

  solver.setStartingPoint(parameter);

  solver.setProblem(problem);
  solver.run();

  const OptimizationResult result(solver.getResult());
  const Scalar residual = result.getOptimalValue()[0];
  if (!(residual <= solver.getMaximumConstraintError()))
    throw InternalException(HERE) << "Quantile residual too high (" << residual << "), should be lower than " << solver.getMaximumConstraintError();

  // set unknown values
  parameter = result.getOptimalPoint();
  UnsignedInteger index = 0;
  for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
  {
    if (!knownParameterIndices_.contains(j))
    {
      effectiveParameter[j] = parameter[index];
      ++ index;
    }
  }

  // set known values
  const UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
    effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];

  Distribution distribution(distribution_);
  distribution.setParameter(effectiveParameter);
  return distribution;
}

void QuantileMatchingFactory::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  solver_ = solver;
}

OptimizationAlgorithm QuantileMatchingFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

/* Accessor to optimization bounds */
void QuantileMatchingFactory::setOptimizationBounds(const Interval & optimizationBounds)
{
  optimizationBounds_ = optimizationBounds;
}

Interval QuantileMatchingFactory::getOptimizationBounds() const
{
  return optimizationBounds_;
}

void QuantileMatchingFactory::setKnownParameter(const Point & values,
    const Indices & indices)
{
  if (values.getSize() != indices.getSize())
    throw InvalidArgumentException(HERE) << "Indices and values size must match";
  const UnsignedInteger parameterDimension = distribution_.getParameterDimension();
  if (!indices.check(parameterDimension))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices QuantileMatchingFactory::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

Point QuantileMatchingFactory::getKnownParameterValues() const
{
  return knownParameterValues_;
}

void QuantileMatchingFactory::setProbabilities(const Point & probabilities)
{
  const UnsignedInteger parameterDimension = distribution_.getParameterDimension();
  if (probabilities.getSize() > parameterDimension)
    throw InvalidArgumentException(HERE) << "At most " << parameterDimension << " probability levels must be provided, but " << probabilities.getSize() << " were provided";
  if (!probabilities.isIncreasing())
    throw InvalidArgumentException(HERE) << "Provided probabilities are not increasing";
  for (UnsignedInteger i = 0; i < probabilities.getSize(); ++ i)
    if (!(probabilities[i] >= 0.0) || !(probabilities[i] <= 1.0))
      throw InvalidArgumentException(HERE) << "Probability levels must be in [0, 1], but probabilities[" << i << "] = " << probabilities[i];
  probabilities_ = probabilities;
}

Point QuantileMatchingFactory::getProbabilities() const
{
  return probabilities_;
}

/* Method save() stores the object through the StorageManager */
void QuantileMatchingFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("probabilities_", probabilities_);
  adv.saveAttribute("knownParameterValues_", knownParameterValues_);
  adv.saveAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
}

/* Method load() reloads the object from the StorageManager */
void QuantileMatchingFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("probabilities_", probabilities_);
  adv.loadAttribute("knownParameterValues_", knownParameterValues_);
  adv.loadAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
}

END_NAMESPACE_OPENTURNS
