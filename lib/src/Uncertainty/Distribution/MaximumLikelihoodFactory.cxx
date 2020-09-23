//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TNC.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumLikelihoodFactory)

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
{
  // Initialize optimization solver parameter using the ResourceMap
  String solverName(ResourceMap::GetAsString("MaximumLikelihoodFactory-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::Build(solverName);
  TNC* tnc = dynamic_cast<TNC *>(solver_.getImplementation().get());
  if (tnc)
    tnc->setIgnoreFailure(true);
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("MaximumLikelihoodFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumConstraintError"));
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
String MaximumLikelihoodFactory::__str__(const String & ) const
{
  return this->getClassName();
}

class LogLikelihoodEvaluation : public EvaluationImplementation
{
public:
  LogLikelihoodEvaluation(const Sample & sample,
                          const Distribution & distribution,
                          const Point & knownParameterValues,
                          const Indices & knownParameterIndices)
    : EvaluationImplementation()
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

  Point operator() (const Point & parameter) const
  {
    Scalar result = 0.0;
    // Define conditinned distribution
    Distribution distribution(distribution_);
    Point effectiveParameter(distribution.getParameter());
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
      return Point(1, SpecFunc::LogMinScalar);
    }
    // Take into account the mean over sample
    // Parallelization (evaluation over a sample) is handled by distribution_
    const Sample logPdfSample = distribution.computeLogPDF(sample_);
    const Scalar logPdf = logPdfSample.computeMean()[0];
    result = SpecFunc::IsNormal(logPdf) ? logPdf : SpecFunc::LogMinScalar;
    return Point(1, result);
  }

private:
  Sample sample_;
  Distribution distribution_;
  Point knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};

class LogLikelihoodGradient : public GradientImplementation
{
public:
  LogLikelihoodGradient(const Sample & sample,
                        const Distribution & distribution,
                        const Point & knownParameterValues,
                        const Indices & knownParameterIndices)
    : GradientImplementation()
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

  Matrix gradient(const Point & parameter) const
  {
    // Define conditinned distribution
    Distribution distribution(distribution_);
    Point effectiveParameter(distribution.getParameter());
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
    const Sample logPdfGradientSample(distribution.computeLogPDFGradient(sample_).getMarginal(unknownParameterIndices_));
    const Point logPdfGradient(logPdfGradientSample.computeMean());
    // Result as Matrix
    result = MatrixImplementation(getInputDimension(), 1, logPdfGradient);
    return result;
  }

private:
  Sample sample_;
  Distribution distribution_;
  Point knownParameterValues_;
  Indices knownParameterIndices_;
  Indices unknownParameterIndices_;
};

Point MaximumLikelihoodFactory::buildParameter(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (!knownParameterIndices_.check(effectiveParameterSize))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Error: known values size must match indices";

  // Define evaluation
  LogLikelihoodEvaluation logLikelihoodWrapper(sample, distribution_, knownParameterValues_, knownParameterIndices_);
  Function logLikelihood(logLikelihoodWrapper.clone());
  // Define gradient
  LogLikelihoodGradient logLikelihoodGradientWrapper(sample, distribution_, knownParameterValues_, knownParameterIndices_);
  logLikelihood.setGradient(logLikelihoodGradientWrapper.clone());

  // Define optimization problem
  OptimizationProblem problem(logLikelihood);
  problem.setMinimization(false);
  problem.setBounds(optimizationBounds_);
  problem.setInequalityConstraint(optimizationInequalityConstraint_);

  OptimizationAlgorithm solver(solver_);
  if (solver.getStartingPoint().getDimension() != logLikelihood.getInputDimension())
  {
    Point effectiveParameter(distribution_.getParameter());
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << logLikelihood.getInputDimension() << ". Switching to the default parameter value=" << effectiveParameter);

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
  UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
  {
    effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
  }
  return effectiveParameter;
}


Distribution MaximumLikelihoodFactory::build(const Sample & sample) const
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

void MaximumLikelihoodFactory::setOptimizationInequalityConstraint(const Function & optimizationInequalityConstraint)
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

void MaximumLikelihoodFactory::setKnownParameter(const Point & values,
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

Point MaximumLikelihoodFactory::getKnownParameterValues() const
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
