//                                               -*- C++ -*-
/**
 *  @brief Estimation by matching quantiles
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
  // default values for fractiles
  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (!probabilities.getSize() && effectiveParameterSize)
  {
    const Scalar epsilon = ResourceMap::GetAsScalar("QuantileMatchingFactory-QuantileEpsilon");
    if (!(epsilon > 0.0)) throw InternalException(HERE) << "Error: the value of epsilon must be non negative.";
    if (!(epsilon <= 0.5)) throw InternalException(HERE) << "Error: the value of epsilon must be lower or equal to 0.5.";
    probabilities_.resize(effectiveParameterSize);
    for (UnsignedInteger i = 0; i < effectiveParameterSize; ++ i)
    {
      const Scalar rho = i * 1.0 / (effectiveParameterSize - 1.0);
      probabilities_[i] = (1 - rho) * epsilon + rho * (1 - epsilon);
    }
  }

  LeastSquaresProblem problem(SymbolicFunction("x", "x^2"));
  if (optimizationBounds.getDimension())
    problem.setBounds(Interval(1));
  solver_ = OptimizationAlgorithm::Build(problem);

  // Initialize optimization solver parameter using the ResourceMap
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("QuantileMatchingFactory-MaximumEvaluationNumber"));
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
    const UnsignedInteger effectiveParameterSize = distribution.getParameter().getSize();
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
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
    return distribution_.getParameterDimension() - knownParameterValues_.getSize();
  }

  UnsignedInteger getOutputDimension() const
  {
    return distribution_.getParameterDimension();
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
    UnsignedInteger parameterDimension = distribution_.getParameterDimension();
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
    Point result(parameterDimension);
    for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
      result[j] = refQuantiles_[j] - distribution.computeQuantile(probabilities_[j])[0];
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

  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();

  if (!knownParameterIndices_.check(effectiveParameterSize))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Error: known values size must match indices";

  Point refQuantiles(effectiveParameterSize);
  for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    refQuantiles[j] = sample.computeQuantile(probabilities_[j])[0];

  Distribution result(buildFromQuantiles(refQuantiles));
  result.setDescription(sample.getDescription());
  return result;
}

/** Build a distribution from its moments */
Distribution QuantileMatchingFactory::buildFromQuantiles(const Point & quantiles) const
{
  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (quantiles.getSize() < effectiveParameterSize)
    throw InvalidArgumentException(HERE) << "Expected " << effectiveParameterSize << " quantiles to build distribution";

  // Define evaluation
  const QuantileMatchingEvaluation quantileMatchingWrapper(quantiles, distribution_, probabilities_, knownParameterValues_, knownParameterIndices_);
  Function quantilesObjective(quantileMatchingWrapper.clone());

  // Define optimization problem
  LeastSquaresProblem problem(quantilesObjective);
  problem.setBounds(optimizationBounds_);
  OptimizationAlgorithm solver(solver_);
  if (solver.getStartingPoint().getDimension() != quantilesObjective.getInputDimension())
  {
    Point effectiveParameter(distribution_.getParameter());
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << quantilesObjective.getInputDimension() << ". Switching to the default parameter value=" << effectiveParameter);

    // extract unknown values
    Point parameter;
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices_.contains(j))
        parameter.add(effectiveParameter[j]);
    }
    solver.setStartingPoint(parameter);
  }
  solver.setProblem(problem);
  solver.setVerbose(Log::HasInfo());
  solver.run();
  Point effectiveParameter(effectiveParameterSize);
  // set unknown values
  Point parameter(solver.getResult().getOptimalPoint());
  UnsignedInteger index = 0;
  for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
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

  Distribution result(distribution_);
  result.setParameter(effectiveParameter);
  return result;
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
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Indices and values size must match";
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
  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (probabilities_.getSize() != effectiveParameterSize)
    throw InvalidArgumentException(HERE) << "Expected " << effectiveParameterSize << " probabilities";
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
