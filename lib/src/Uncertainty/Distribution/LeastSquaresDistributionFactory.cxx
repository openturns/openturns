//                                               -*- C++ -*-
/**
 *  @brief Least squares estimation
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
#include "openturns/LeastSquaresDistributionFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/Normal.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresDistributionFactory)

static const Factory<LeastSquaresDistributionFactory> Factory_LeastSquaresDistributionFactory;

/* Default constructor */
LeastSquaresDistributionFactory::LeastSquaresDistributionFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresDistributionFactory::LeastSquaresDistributionFactory(const Distribution & distribution)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
{
  solver_ = OptimizationAlgorithm::Build(LeastSquaresProblem());
  // Initialize optimization solver parameter using the ResourceMap
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("MaximumLikelihoodFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("MaximumLikelihoodFactory-MaximumConstraintError"));
  solver_.setVerbose(Log::HasInfo());
}

/* Virtual constructor */
LeastSquaresDistributionFactory * LeastSquaresDistributionFactory::clone() const
{
  return new LeastSquaresDistributionFactory(*this);
}

/* String converter */
String LeastSquaresDistributionFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName()
      << " distribution=" << distribution_
      << " solver=" << solver_;
  return oss;
}

/* String converter */
String LeastSquaresDistributionFactory::__str__(const String & ) const
{
  return this->getClassName();
}

class LeastSquaresFactoryResidualEvaluation : public EvaluationImplementation
{
public:
  LeastSquaresFactoryResidualEvaluation(const Sample & sample,
                                        const Distribution & distribution,
                                        const Point & knownParameterValues,
                                        const Indices & knownParameterIndices)
    : EvaluationImplementation()
    , sample_(sample)
    , distribution_(distribution)
  {
    // build the unknown indices
    const UnsignedInteger effectiveParameterSize = distribution.getParameter().getSize();
    effectiveParameter_ = Point(effectiveParameterSize);
    // set known values
    UnsignedInteger knownParametersSize = knownParameterIndices.getSize();
    for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
    {
      effectiveParameter_[knownParameterIndices[j]] = knownParameterValues[j];
    }
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices.contains(j))
        unknownParameterIndices_.add(j);
    }
    empiricalCDF_ = Point(sample.getSize());
    for(UnsignedInteger i = 0 ; i < sample.getSize(); ++ i)
      empiricalCDF_[i] = sample.computeEmpiricalCDF(sample[i]);
  }

  LeastSquaresFactoryResidualEvaluation * clone() const
  {
    return new LeastSquaresFactoryResidualEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return unknownParameterIndices_.getSize();
  }

  UnsignedInteger getOutputDimension() const
  {
    return sample_.getSize();
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(getInputDimension(), "theta");
  }

  Description getOutputDescription() const
  {
    return Description(sample_.getSize(), "r");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Point operator() (const Point & parameter) const
  {
    // Define conditinned distribution
    Distribution distribution(distribution_);
    Point effectiveParameter(effectiveParameter_);
    // set unknown values
    UnsignedInteger unknownParameterSize = unknownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < unknownParameterSize; ++ j)
    {
      effectiveParameter[unknownParameterIndices_[j]] = parameter[j];
    }
    try
    {
      distribution.setParameter(effectiveParameter);
    }
    catch (const Exception &)
    {
      return Point(getOutputDimension(), SpecFunc::MaxScalar);
    }

    // Compute the residual between the candidate CDF and the empirical CDF
    return distribution.computeCDF(sample_).asPoint() - empiricalCDF_;
  }

private:
  Sample sample_;
  Point empiricalCDF_;
  Distribution distribution_;
  Indices unknownParameterIndices_;
  Point effectiveParameter_;
};

class LeastSquaresFactoryResidualGradient : public GradientImplementation
{
public:
  LeastSquaresFactoryResidualGradient(const Sample & sample,
                                      const Distribution & distribution,
                                      const Point & knownParameterValues,
                                      const Indices & knownParameterIndices)
    : GradientImplementation()
    , sample_(sample)
    , distribution_(distribution)
  {
    // build the unknown indices
    const UnsignedInteger effectiveParameterSize = distribution.getParameter().getSize();
    for (UnsignedInteger j = 0; j < effectiveParameterSize; ++ j)
    {
      if (!knownParameterIndices.contains(j))
        unknownParameterIndices_.add(j);
    }
    effectiveParameter_ = Point(effectiveParameterSize);
    // set known values
    for (UnsignedInteger j = 0; j < knownParameterIndices.getSize(); ++ j)
    {
      effectiveParameter_[knownParameterIndices[j]] = knownParameterValues[j];
    }
  }

  LeastSquaresFactoryResidualGradient * clone() const
  {
    return new LeastSquaresFactoryResidualGradient(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return unknownParameterIndices_.getSize();
  }

  UnsignedInteger getOutputDimension() const
  {
    return sample_.getSize();
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

  Matrix gradient(const Point & parameter) const
  {
    // Define conditinned distribution
    Distribution distribution(distribution_);
    Point effectiveParameter(effectiveParameter_);
    // set unknown values
    UnsignedInteger unknownParameterSize = unknownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < unknownParameterSize; ++ j)
    {
      effectiveParameter[unknownParameterIndices_[j]] = parameter[j];
    }
    distribution.setParameter(effectiveParameter);
    // Matrix result
    MatrixImplementation result(parameter.getSize(), getOutputDimension());
    // Evaluate the gradient
    const Sample cdfGradientSample(distribution.computeCDFGradient(sample_).getMarginal(unknownParameterIndices_));
    const Matrix transposedGradient(cdfGradientSample.getDimension(), cdfGradientSample.getSize(), cdfGradientSample.getImplementation()->data_begin(), cdfGradientSample.getImplementation()->data_end());

    return transposedGradient.transpose();
  }

private:
  Sample sample_;
  Distribution distribution_;
  Indices unknownParameterIndices_;
  Point effectiveParameter_;
};

Point LeastSquaresDistributionFactory::buildParameter(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  UnsignedInteger effectiveParameterSize = distribution_.getParameterDimension();
  if (!knownParameterIndices_.check(effectiveParameterSize))
    throw InvalidArgumentException(HERE) << "Error: known indices cannot exceed parameter size";
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Error: known values size must match indices";

  LeastSquaresFactoryResidualEvaluation residualEvaluation(sample, distribution_, knownParameterValues_, knownParameterIndices_);
  Function residual(residualEvaluation.clone());

  // Define optimization problem
  LeastSquaresProblem problem(residual);
  problem.setBounds(optimizationBounds_);
  problem.setInequalityConstraint(optimizationInequalityConstraint_);

  OptimizationAlgorithm solver(solver_);
  if (solver.getStartingPoint().getDimension() != residual.getInputDimension())
  {
    Point effectiveParameter(distribution_.getParameter());
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << residual.getInputDimension() << ". Switching to the default parameter value=" << effectiveParameter);

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
  UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
  {
    effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
  }
  return effectiveParameter;
}


Distribution LeastSquaresDistributionFactory::build(const Point & parameter) const
{
  Distribution result(distribution_);
  Point parameter2(parameter);
  // set known values
  UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
  {
    parameter2[knownParameterIndices_[j]] = knownParameterValues_[j];
  }
  result.setParameter(parameter2);
  return result;
}


Distribution LeastSquaresDistributionFactory::build() const
{
  return build(distribution_.getParameter());
}


Distribution LeastSquaresDistributionFactory::build(const Sample & sample) const
{
  Distribution result(distribution_);
  result.setParameter(buildParameter(sample));
  result.setDescription(sample.getDescription());
  return result;
}


/* Accessor to optimization bounds */
void LeastSquaresDistributionFactory::setOptimizationBounds(const Interval & optimizationBounds)
{
  optimizationBounds_ = optimizationBounds;
}

Interval LeastSquaresDistributionFactory::getOptimizationBounds() const
{
  return optimizationBounds_;
}

void LeastSquaresDistributionFactory::setOptimizationInequalityConstraint(const Function & optimizationInequalityConstraint)
{
  optimizationInequalityConstraint_ = optimizationInequalityConstraint;
}

void LeastSquaresDistributionFactory::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  solver_ = solver;
}

OptimizationAlgorithm LeastSquaresDistributionFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void LeastSquaresDistributionFactory::setKnownParameter(const Point & values,
    const Indices & indices)
{
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize())
    throw InvalidArgumentException(HERE) << "Known parameters values and indices must have the same size";
  if (!indices.check(distribution_.getParameter().getSize()))
    throw InvalidArgumentException(HERE) << "Know parameters indices must be < parameter dimension";
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices LeastSquaresDistributionFactory::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

Point LeastSquaresDistributionFactory::getKnownParameterValues() const
{
  return knownParameterValues_;
}


/* Method save() stores the object through the StorageManager */
void LeastSquaresDistributionFactory::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("knownParameterValues_", knownParameterValues_);
  adv.saveAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
  adv.saveAttribute("optimizationInequalityConstraint_", optimizationInequalityConstraint_);
}

/* Method load() reloads the object from the StorageManager */
void LeastSquaresDistributionFactory::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("knownParameterValues_", knownParameterValues_);
  adv.loadAttribute("knownParameterIndices_", knownParameterIndices_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
  adv.loadAttribute("optimizationInequalityConstraint_", optimizationInequalityConstraint_);
}

END_NAMESPACE_OPENTURNS
