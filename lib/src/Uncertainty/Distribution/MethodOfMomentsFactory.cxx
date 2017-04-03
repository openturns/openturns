//                                               -*- C++ -*-
/**
 *  @brief Estimation by method of moments
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <iomanip>
#include <fstream>
#include "openturns/MethodOfMomentsFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TNC.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MethodOfMomentsFactory);

static const Factory<MethodOfMomentsFactory> Factory_MethodOfMomentsFactory;

/* Default constructor */
MethodOfMomentsFactory::MethodOfMomentsFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
MethodOfMomentsFactory::MethodOfMomentsFactory(const Distribution & distribution)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
  , solver_(new TNC())
{
  // Initialize optimization solver parameter using the ResourceMap
  solver_.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("MethodOfMomentsFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("MethodOfMomentsFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar("MethodOfMomentsFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsNumericalScalar("MethodOfMomentsFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar("MethodOfMomentsFactory-MaximumConstraintError"));
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
String MethodOfMomentsFactory::__str__(const String & offset) const
{
  return this->getClassName();
}

class MethodOfMomentsEvaluation : public EvaluationImplementation
{
public:
  MethodOfMomentsEvaluation(const NumericalPoint & refMoments,
                                        const Distribution & distribution,
                                        const NumericalPoint & knownParameterValues,
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
    return 1;
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(getInputDimension(), "theta");
  }

  Description getOutputDescription() const
  {
    return Description(1, "lh");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  NumericalPoint operator() (const NumericalPoint & parameter) const
  {
    UnsignedInteger parameterDimension = distribution_.getParameterDimension();
    NumericalPoint effectiveParameter(parameterDimension);
    // set unknown values
    UnsignedInteger unknownParameterSize = unknownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < unknownParameterSize; ++ j)
    {
      effectiveParameter[unknownParameterIndices_[j]] = parameter[j];
    }
    // set known values
    UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
    {
      effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
    }
    Distribution distribution(distribution_);
    try
    {
      distribution.setParameter(effectiveParameter);
    }
    catch (Exception &)
    {
      return NumericalPoint(1, SpecFunc::MaxNumericalScalar);
    }

    // compute moments of conditioned distribution
    NumericalPoint moments(parameterDimension);
    moments[0] = distribution.getMean()[0];
    for (UnsignedInteger j = 1; j < parameterDimension; ++ j)
    {
      moments[j] = distribution.getCenteredMoment(j + 1)[0];
    }

    // compute sum of deltas between centered homogenized moments
    NumericalScalar result = 0.0;
    for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
    {
      const NumericalScalar sign = moments[j] < 0.0 ? -1.0 : 1.0;
      const NumericalScalar slack = refSign_[j] * std::pow(std::abs(refMoments_[j]), 1.0 / (j + 1.0)) - sign * std::pow(std::abs(moments[j]), 1.0 / (j + 1.0));
      result += slack * slack;
    }
    const NumericalScalar sigma2 = distribution.getCovariance()(0, 0);
    return NumericalPoint(1, result / sigma2);
  }

private:
  NumericalPoint refMoments_;
  NumericalPoint refSign_;
  Distribution distribution_;
  NumericalPoint knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};

NumericalPoint MethodOfMomentsFactory::buildParameter(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();

  if (!knownParameterIndices_.check(effectiveParameterSize))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Error: known values size must match indices";

  NumericalPoint refMoments(effectiveParameterSize);
  refMoments[0] =  sample.computeMean()[0];
  for (UnsignedInteger j = 1; j < effectiveParameterSize; ++ j)
  {
    refMoments[j] = sample.computeCenteredMoment(j + 1)[0];
  }

  // Define NumericalMathEvaluation using the MethodOfMomentsEvaluation wrapper
  MethodOfMomentsEvaluation methodOfMomentsWrapper(refMoments, distribution_, knownParameterValues_, knownParameterIndices_);
  Function momentsObjective(methodOfMomentsWrapper.clone());

  // Define optimisation problem
  OptimizationProblem problem(problem_);
  problem.setObjective(momentsObjective);
  OptimizationAlgorithm solver(solver_);
  if (solver.getStartingPoint().getDimension() != momentsObjective.getInputDimension())
  {
    NumericalPoint effectiveParameter(distribution_.getParameter());
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << momentsObjective.getInputDimension() << ". Switching to the default parameter value=" << effectiveParameter);

    // extract unknown values
    NumericalPoint parameter;
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices_.contains(j))
        parameter.add(effectiveParameter[j]);
    }
    solver.setStartingPoint(parameter);
  }
  solver.setProblem(problem);
  solver.run();
  NumericalPoint effectiveParameter(effectiveParameterSize);
  // set unknown values
  NumericalPoint parameter(solver.getResult().getOptimalPoint());
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
  {
    effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
  }
  return effectiveParameter;
}


DistributionFactoryImplementation::Implementation MethodOfMomentsFactory::build(const Sample & sample) const
{
  Distribution result(distribution_);
  result.setParameter(buildParameter(sample));
  result.setDescription(sample.getDescription());
  return result.getImplementation();
}


void MethodOfMomentsFactory::setOptimizationProblem(const OptimizationProblem& problem)
{
  problem_ = problem;
}

OptimizationProblem MethodOfMomentsFactory::getOptimizationProblem() const
{
  return problem_;
}

void MethodOfMomentsFactory::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  solver_ = solver;
}

OptimizationAlgorithm MethodOfMomentsFactory::getOptimizationAlgorithm() const
{
  return solver_;
}


void MethodOfMomentsFactory::setOptimizationSolver(const OptimizationAlgorithm& solver)
{
  Log::Warn(OSS() << "MethodOfMomentsFactory::setOptimizationSolver is deprecated");
  setOptimizationAlgorithm(solver);
}

OptimizationAlgorithm MethodOfMomentsFactory::getOptimizationSolver() const
{
  Log::Warn(OSS() << "MethodOfMomentsFactory::getOptimizationSolver is deprecated");
  return getOptimizationAlgorithm();
}

void MethodOfMomentsFactory::setKnownParameter(const NumericalPoint & values,
    const Indices & indices)
{
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize()) throw InvalidArgumentException(HERE);
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices MethodOfMomentsFactory::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

NumericalPoint MethodOfMomentsFactory::getKnownParameterValues() const
{
  return knownParameterValues_;
}


/* Method save() stores the object through the StorageManager */
void MethodOfMomentsFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("knownParameterValues_", knownParameterValues_);
  adv.saveAttribute("knownParameterIndices_", knownParameterIndices_);
}

/* Method load() reloads the object from the StorageManager */
void MethodOfMomentsFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("knownParameterValues_", knownParameterValues_);
  adv.loadAttribute("knownParameterIndices_", knownParameterIndices_);
}

END_NAMESPACE_OPENTURNS
