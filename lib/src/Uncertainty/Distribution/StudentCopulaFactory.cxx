//                                               -*- C++ -*-
/**
 *  @brief Factory for StudentCopula distribution
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
#include "openturns/StudentCopulaFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/NormalCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StudentCopulaFactory)

static const Factory<StudentCopulaFactory> Factory_StudentCopulaFactory;

/* Default constructor */
StudentCopulaFactory::StudentCopulaFactory()
  : DistributionFactoryImplementation()
{
  // Initialize optimization solver parameter using the ResourceMap
  String solverName(ResourceMap::GetAsString("StudentCopulaFactory-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::Build(solverName);
  solver_.setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("StudentCopulaFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("StudentCopulaFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("StudentCopulaFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("StudentCopulaFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("StudentCopulaFactory-MaximumConstraintError"));
}

/* Virtual constructor */
StudentCopulaFactory * StudentCopulaFactory::clone() const
{
  return new StudentCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution StudentCopulaFactory::build(const Sample & sample) const
{
  return buildAsStudentCopula(sample).clone();
}

Distribution StudentCopulaFactory::build(const Point & parameters) const
{
  return buildAsStudentCopula(parameters).clone();
}

Distribution StudentCopulaFactory::build() const
{
  return buildAsStudentCopula().clone();
}


StudentCopula StudentCopulaFactory::buildAsStudentCopula(const Sample & sample) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "StudentCopulaFactory cannot build a StudentCopula distribution from an empty sample";
  if (dimension < 2)
    throw InvalidArgumentException(HERE) << "StudentCopulaFactory cannot build a StudentCopula distribution from a sample of dimension < 2";

  // estimate the correlation
  const CorrelationMatrix R(NormalCopula::GetCorrelationFromKendallCorrelation(sample.computeKendallTau()));

  // estimate nu with profiled likelihood
  const Scalar nuStart = ResourceMap::GetAsScalar("StudentCopulaFactory-NuStart");
  StudentCopula model(nuStart, R);
  MaximumLikelihoodFactory factory(model);

  // freeze all other parameters
  Point values(model.getParameter());
  values.erase(0);
  Indices indices(values.getSize());
  indices.fill(1);
  factory.setKnownParameter(values, indices);

  factory.setOptimizationAlgorithm(solver_);
  const Scalar nuMin = ResourceMap::GetAsScalar("StudentCopulaFactory-NuMin");
  const Scalar nuMax = ResourceMap::GetAsScalar("StudentCopulaFactory-NuMax");
  factory.setOptimizationBounds(Interval(nuMin, nuMax));
  return buildAsStudentCopula(factory.buildParameter(sample));
}

StudentCopula StudentCopulaFactory::buildAsStudentCopula(const Point & parameters) const
{
  try
  {
    // find the dimension
    StudentCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a StudentCopula from the given parameters";
  }
}

StudentCopula StudentCopulaFactory::buildAsStudentCopula() const
{
  return StudentCopula();
}

void StudentCopulaFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

OptimizationAlgorithm StudentCopulaFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

END_NAMESPACE_OPENTURNS
