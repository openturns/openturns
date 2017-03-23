//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
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
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/Normal.hxx"
#include "openturns/TNC.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalMathGradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumLikelihoodFactory);

static const Factory<MaximumLikelihoodFactory> Factory_MaximumLikelihoodFactory;

/* Default constructor */
MaximumLikelihoodFactory::MaximumLikelihoodFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
MaximumLikelihoodFactory::MaximumLikelihoodFactory(const Distribution & distribution)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
  , solver_(new TNC())
{
  // Initialize optimization solver parameter using the ResourceMap
  solver_.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("MaximumLikelihoodFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumConstraintError"));
}

/* Virtual constructor */
MaximumLikelihoodFactory * MaximumLikelihoodFactory::clone() const
{
  return new MaximumLikelihoodFactory(*this);
}

/* String converter */
String MaximumLikelihoodFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName()
      << " distribution=" << distribution_
      << " solver=" << solver_;
  return oss;
}

/* String converter */
String MaximumLikelihoodFactory::__str__(const String & offset) const
{
  return this->getClassName();
}

class LogLikelihoodEvaluation : public NumericalMathEvaluationImplementation
{
public:
  LogLikelihoodEvaluation(const NumericalSample & sample,
                                        const Distribution & distribution,
                                        const NumericalPoint & knownParameterValues,
                                        const Indices & knownParameterIndices)
    : NumericalMathEvaluationImplementation()
    , sample_(sample)
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
  }

  LogLikelihoodEvaluation * clone() const
  {
    return new LogLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return unknownParameterIndices_.getSize();
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
    NumericalScalar result = 0.0;
    // Define conditinned distribution
    Distribution distribution(distribution_);
    NumericalPoint effectiveParameter(distribution.getParameter());
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
    try
    {
      distribution.setParameter(effectiveParameter);
    }
    catch (Exception &)
    {
      return NumericalPoint(1, SpecFunc::LogMinNumericalScalar);
    }
    // Take into account the mean over sample
    // Parallelization (evaluation over a sample) is handeled by distribution_
    const NumericalSample logPdfSample = distribution.computeLogPDF(sample_);
    const NumericalScalar logPdf = logPdfSample.computeMean()[0];
    result = SpecFunc::IsNormal(logPdf) ? logPdf : SpecFunc::LogMinNumericalScalar;
    return NumericalPoint(1, result);
  }

private:
  NumericalSample sample_;
  Distribution distribution_;
  NumericalPoint knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};

class LogLikelihoodGradient : public NumericalMathGradientImplementation
{
public:
  LogLikelihoodGradient(const NumericalSample & sample,
                        const Distribution & distribution,
                        const NumericalPoint & knownParameterValues,
                        const Indices & knownParameterIndices)
    : NumericalMathGradientImplementation()
    , sample_(sample)
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
  }

  LogLikelihoodGradient * clone() const
  {
    return new LogLikelihoodGradient(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return unknownParameterIndices_.getSize();
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
    return Description(1, "lhG");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Matrix gradient(const NumericalPoint & parameter) const
  {
    // Define conditinned distribution
    Distribution distribution(distribution_);
    NumericalPoint effectiveParameter(distribution.getParameter());
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
    distribution.setParameter(effectiveParameter);
    // Matrix result
    MatrixImplementation result(parameter.getSize(), 1);
    // Evaluate the gradient
    const NumericalSample logPdfGradientSample(distribution.computeLogPDFGradient(sample_).getMarginal(unknownParameterIndices_));
    const NumericalPoint logPdfGradient(logPdfGradientSample.computeMean());
    // Result as Matrix
    result = MatrixImplementation(getInputDimension(), 1, logPdfGradient);
    return result;
  }

private:
  NumericalSample sample_;
  Distribution distribution_;
  NumericalPoint knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};

NumericalPoint MaximumLikelihoodFactory::buildParameter(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (!knownParameterIndices_.check(effectiveParameterSize))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Error: known values size must match indices";

  // Define NumericalMathEvaluation using the LogLikelihoodEvaluation wrapper
  LogLikelihoodEvaluation logLikelihoodWrapper(sample, distribution_, knownParameterValues_, knownParameterIndices_);
  NumericalMathFunction logLikelihood(logLikelihoodWrapper.clone());
  // Define NumericalMathGradient using the LogLikelihoodEvaluation wrapper
  LogLikelihoodGradient logLikelihoodGradientWrapper(sample, distribution_, knownParameterValues_, knownParameterIndices_);
  logLikelihood.setGradient(logLikelihoodGradientWrapper.clone());

  // Define optimization problem
  OptimizationProblem problem;
  problem.setMinimization(false);
  problem.setObjective(logLikelihood);
  problem.setBounds(optimizationBounds_);
  OptimizationAlgorithm solver(solver_);
  if (solver.getStartingPoint().getDimension() != logLikelihood.getInputDimension())
  {
    NumericalPoint effectiveParameter(distribution_.getParameter());
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << logLikelihood.getInputDimension() << ". Switching to the default parameter value=" << effectiveParameter);

    // extract unknown values
    NumericalPoint parameter;
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices_.contains(j))
        parameter.add(effectiveParameter[j]);
    }
    solver.setStartingPoint(parameter);
  }
  // Usually problem requires to set new bounds if dimension != effectiveDimension (for defined bounds)
  if (problem.hasBounds() && (problem.getDimension() != problem.getBounds().getDimension()))
  {
    const Interval bounds(problem.getBounds());
    const NumericalPoint lowerBound(bounds.getLowerBound());
    const NumericalPoint upperBound(bounds.getUpperBound());
    const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
    const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
    NumericalPoint effectiveLowerBound;
    NumericalPoint effectiveUpperBound;
    Interval::BoolCollection effectiveFiniteLowerBound;
    Interval::BoolCollection effectiveFiniteUpperBound;
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices_.contains(j))
      {
        effectiveLowerBound.add(lowerBound[j]);
        effectiveUpperBound.add(upperBound[j]);
        effectiveFiniteLowerBound.add(finiteLowerBound[j]);
        effectiveFiniteUpperBound.add(finiteUpperBound[j]);
      }
    }
    Interval effectiveBounds(effectiveLowerBound, effectiveUpperBound, effectiveFiniteLowerBound, effectiveFiniteUpperBound);
    problem.setBounds(effectiveBounds);
  }
  // Check that problem is valid
  if(!problem.isValid())
    throw InvalidArgumentException(HERE) << "In MaximumLikelihoodFactory::buildParameter, given problem is invalid."
                                         << " Please check that bounds and various constraints have accurate dimensions";
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


DistributionFactoryImplementation::Implementation MaximumLikelihoodFactory::build(const NumericalSample & sample) const
{
  Distribution result(distribution_);
  result.setParameter(buildParameter(sample));
  result.setDescription(sample.getDescription());
  return result.getImplementation();
}


/* Accessor to optimization bounds */
void MaximumLikelihoodFactory::setOptimizationBounds(const Interval & optimizationBounds)
{
  optimizationBounds_ = optimizationBounds;
}

Interval MaximumLikelihoodFactory::getOptimizationBounds() const
{
  return optimizationBounds_;
}

void MaximumLikelihoodFactory::setOptimizationInequalityConstraint(const NumericalMathFunction & optimizationInequalityConstraint)
{
  optimizationInequalityConstraint_ = optimizationInequalityConstraint;
}

void MaximumLikelihoodFactory::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  solver_ = solver;
}

OptimizationAlgorithm MaximumLikelihoodFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void MaximumLikelihoodFactory::setOptimizationSolver(const OptimizationAlgorithm& solver)
{
  Log::Warn(OSS() << "MaximumLikelihoodFactory::setOptimizationSolver is deprecated");
  setOptimizationAlgorithm(solver);
}

OptimizationAlgorithm MaximumLikelihoodFactory::getOptimizationSolver() const
{
  Log::Warn(OSS() << "MaximumLikelihoodFactory::getOptimizationSolver is deprecated");
  return getOptimizationAlgorithm();
}

void MaximumLikelihoodFactory::setKnownParameter(const NumericalPoint & values,
    const Indices & indices)
{
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize()) throw InvalidArgumentException(HERE) << "Known parameters values and indices must have the same size";
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices MaximumLikelihoodFactory::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

NumericalPoint MaximumLikelihoodFactory::getKnownParameterValues() const
{
  return knownParameterValues_;
}


/* Method save() stores the object through the StorageManager */
void MaximumLikelihoodFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("knownParameterValues_", knownParameterValues_);
  adv.saveAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
  adv.saveAttribute("optimizationInequalityConstraint_", optimizationInequalityConstraint_);
}

/* Method load() reloads the object from the StorageManager */
void MaximumLikelihoodFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("knownParameterValues_", knownParameterValues_);
  adv.loadAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
  adv.loadAttribute("optimizationInequalityConstraint_", optimizationInequalityConstraint_);
}

END_NAMESPACE_OPENTURNS
