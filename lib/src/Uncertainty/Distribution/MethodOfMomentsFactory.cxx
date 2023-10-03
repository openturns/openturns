//                                               -*- C++ -*-
/**
 *  @brief Estimation by method of moments
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
#include "openturns/MethodOfMomentsFactory.hxx"
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

CLASSNAMEINIT(MethodOfMomentsFactory)

static const Factory<MethodOfMomentsFactory> Factory_MethodOfMomentsFactory;

/* Default constructor */
MethodOfMomentsFactory::MethodOfMomentsFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
MethodOfMomentsFactory::MethodOfMomentsFactory(const Distribution & distribution,
    const Interval & optimizationBounds)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
  , optimizationBounds_(optimizationBounds)
{
  LeastSquaresProblem problem(SymbolicFunction("x", "x^2"));
  if (optimizationBounds.getDimension())
    problem.setBounds(Interval(1));
  solver_ = OptimizationAlgorithm::Build(problem);

  // Initialize optimization solver parameter using the ResourceMap
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("MethodOfMomentsFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("MethodOfMomentsFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("MethodOfMomentsFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("MethodOfMomentsFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("MethodOfMomentsFactory-MaximumConstraintError"));
}

/* Virtual constructor */
MethodOfMomentsFactory * MethodOfMomentsFactory::clone() const
{
  return new MethodOfMomentsFactory(*this);
}

/* String converter */
String MethodOfMomentsFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName()
      << " distribution=" << distribution_
      << " solver=" << solver_;
  return oss;
}

/* String converter */
String MethodOfMomentsFactory::__str__(const String & ) const
{
  return this->getClassName();
}

class MethodOfMomentsEvaluation : public EvaluationImplementation
{
public:
  MethodOfMomentsEvaluation(const Point & refMoments,
                            const Distribution & distribution,
                            const Point & knownParameterValues,
                            const Indices & knownParameterIndices)
    : EvaluationImplementation()
    , refMoments_(refMoments)
    , refSign_(refMoments.getSize())
    , distribution_(distribution)
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

    for (UnsignedInteger j = 0; j < refMoments.getSize(); ++ j)
    {
      refSign_[j] = refMoments_[j] < 0.0 ? -1.0 : 1.0;
    }
  }

  MethodOfMomentsEvaluation * clone() const
  {
    return new MethodOfMomentsEvaluation(*this);
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
    UnsignedInteger unknownParameterSize = unknownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < unknownParameterSize; ++ j)
      effectiveParameter[unknownParameterIndices_[j]] = parameter[j];

    // set known values
    UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
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

    // compute moments of conditioned distribution
    Point moments(parameterDimension);
    moments[0] = distribution.getMean()[0];
    for (UnsignedInteger j = 1; j < parameterDimension; ++ j)
      moments[j] = distribution.getCentralMoment(j + 1)[0];

    // compute sum of deltas between centered homogenized moments
    Point result(parameterDimension);
    for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
    {
      const Scalar sign = moments[j] < 0.0 ? -1.0 : 1.0;
      result[j] = refSign_[j] * std::pow(std::abs(refMoments_[j]), 1.0 / (j + 1.0)) - sign * std::pow(std::abs(moments[j]), 1.0 / (j + 1.0));
    }
    const Scalar sigma2 = distribution.getCovariance()(0, 0);
    if (!(sigma2 > 0.0))
      return Point(getOutputDimension(), SpecFunc::MaxScalar);
    return result / sigma2;
  }

private:
  Point refMoments_;
  Point refSign_;
  Distribution distribution_;
  Point knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};


Distribution MethodOfMomentsFactory::build(const Point & parameter) const
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


Distribution MethodOfMomentsFactory::build() const
{
  return build(distribution_.getParameter());
}

Distribution MethodOfMomentsFactory::build(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();

  if (!knownParameterIndices_.check(effectiveParameterSize))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Error: known values size must match indices";

  Point refMoments(effectiveParameterSize);
  refMoments[0] =  sample.computeMean()[0];
  for (UnsignedInteger j = 1; j < effectiveParameterSize; ++ j)
    refMoments[j] = sample.computeCentralMoment(j + 1)[0];

  Distribution result(buildFromMoments(refMoments));
  result.setDescription(sample.getDescription());
  return result;
}

/** Build a distribution from its moments */
Distribution MethodOfMomentsFactory::buildFromMoments(const Point & moments) const
{
  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (moments.getSize() < effectiveParameterSize)
    throw InvalidArgumentException(HERE) << "Expected " << effectiveParameterSize << " moments to build distribution";

  // Define evaluation
  MethodOfMomentsEvaluation methodOfMomentsWrapper(moments, distribution_, knownParameterValues_, knownParameterIndices_);
  Function momentsObjective(methodOfMomentsWrapper.clone());

  // Define optimization problem
  LeastSquaresProblem problem(momentsObjective);
  problem.setBounds(optimizationBounds_);
  OptimizationAlgorithm solver(solver_);
  if (solver.getStartingPoint().getDimension() != momentsObjective.getInputDimension())
  {
    Point effectiveParameter(distribution_.getParameter());
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << momentsObjective.getInputDimension() << ". Switching to the default parameter value=" << effectiveParameter);

    // extract unknown values
    Point parameter;
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices_.contains(j))
        parameter.add(effectiveParameter[j]);
    }
    solver.setStartingPoint(parameter);
  }
  // clip starting point
  if (optimizationBounds_.getDimension() && !optimizationBounds_.contains(solver.getStartingPoint()))
  {
    Point startingPoint(solver.getStartingPoint());
    const Point lb(optimizationBounds_.getLowerBound());
    const Point ub(optimizationBounds_.getUpperBound());
    for (UnsignedInteger j = 0; j < startingPoint.getDimension(); ++ j)
    {
      startingPoint[j] = std::min(startingPoint[j], ub[j]);
      startingPoint[j] = std::max(startingPoint[j], lb[j]);
    }
  }

  solver.setProblem(problem);
  solver.setVerbose(Log::HasInfo());
  try
  {
    solver.run();
  }
  catch (const Exception & exc)
  {
    throw NotDefinedException(HERE) << "Cannot estimate distribution from moments: " << exc.what();
  }

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
  UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
    effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];

  Distribution result(distribution_);
  result.setParameter(effectiveParameter);
  return result;
}

void MethodOfMomentsFactory::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  solver_ = solver;
}

OptimizationAlgorithm MethodOfMomentsFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

/* Accessor to optimization bounds */
void MethodOfMomentsFactory::setOptimizationBounds(const Interval & optimizationBounds)
{
  optimizationBounds_ = optimizationBounds;
}

Interval MethodOfMomentsFactory::getOptimizationBounds() const
{
  return optimizationBounds_;
}

void MethodOfMomentsFactory::setKnownParameter(const Point & values,
    const Indices & indices)
{
  if (values.getSize() != indices.getSize())
    throw InvalidArgumentException(HERE) << "Indices and values size must match";
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices MethodOfMomentsFactory::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

Point MethodOfMomentsFactory::getKnownParameterValues() const
{
  return knownParameterValues_;
}


/* Method save() stores the object through the StorageManager */
void MethodOfMomentsFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("knownParameterValues_", knownParameterValues_);
  adv.saveAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
}

/* Method load() reloads the object from the StorageManager */
void MethodOfMomentsFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("knownParameterValues_", knownParameterValues_);
  adv.loadAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
}

END_NAMESPACE_OPENTURNS
