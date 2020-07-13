//                                               -*- C++ -*-
/**
 *  @brief Factory for Student distribution
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
#include "openturns/StudentFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/TNC.hxx"
#include "openturns/NormalCopula.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StudentFactory)

static const Factory<StudentFactory> Factory_StudentFactory;

/* Default constructor */
StudentFactory::StudentFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
StudentFactory * StudentFactory::clone() const
{
  return new StudentFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution StudentFactory::build(const Sample & sample) const
{
  return buildAsStudent(sample).clone();
}

Distribution StudentFactory::build(const Point & parameter) const
{
  return buildAsStudent(parameter).clone();
}

Distribution StudentFactory::build() const
{
  return buildAsStudent().clone();
}

DistributionFactoryResult StudentFactory::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}


struct StudentFactoryReducedLogLikelihood
{
  /** Constructor from a sample and a derivative factor estimate */
  StudentFactoryReducedLogLikelihood(const Sample & sample,
                                     const Point & mu,
                                     const Point & stdev,
                                     const CorrelationMatrix & R)
    : sample_(sample)
    , mu_(mu)
    , stdev_(stdev)
    , R_(R)
  {
    // Nothing to do
  };

  Point computeLogLikelihood(const Point & parameter) const
  {
    const Scalar nu = parameter[0];
    const Scalar factor = 1.0 - 2.0 / nu;
    if (factor <= 0.0) return Point(1, -SpecFunc::MaxScalar);
    const Point sigma(stdev_ * std::sqrt(factor));
    return Student(nu, mu_, sigma, R_).computeLogPDF(sample_).computeMean();
  }

  Sample sample_;
  Point mu_;
  Point stdev_;
  CorrelationMatrix R_;
};


Student StudentFactory::buildAsStudent(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Student distribution from an empty sample";
  const Point mu(sample.computeMean());
  const Point stdev(sample.computeStandardDeviationPerComponent());
  // The relation between Kendall's tau and shape matrix is universal among the elliptical copulas. Use the method in NormalCopula.
  const CorrelationMatrix R(NormalCopula::GetCorrelationFromKendallCorrelation(sample.computeKendallTau()));

  // Now, nu is found by reduced likelihood maximization
  StudentFactoryReducedLogLikelihood logLikelihood(sample, mu, stdev, R);
  const Function objective(bindMethod<StudentFactoryReducedLogLikelihood, Point, Point>(logLikelihood, &StudentFactoryReducedLogLikelihood::computeLogLikelihood, 1, 1));
  OptimizationProblem problem(objective);
  const Interval bounds(2.0 * (1.0 + SpecFunc::ScalarEpsilon), ResourceMap::GetAsScalar("StudentFactory-NuMax"));
  problem.setBounds(bounds);
  problem.setMinimization(false);
  TNC solver(problem);
  solver.setStartingPoint((bounds.getLowerBound() + bounds.getUpperBound()) * 0.5);
  solver.run();
  const Scalar nu = solver.getResult().getOptimalPoint()[0];
  const Point sigma(stdev * std::sqrt(1.0 - 2.0 / nu));
  Student result(nu, mu, sigma, R);
  result.setDescription(sample.getDescription());
  return result;
}

Student StudentFactory::buildAsStudent(const Point & parameter) const
{
  try
  {
    Student distribution;
    distribution.setParameter(parameter);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Student distribution from the given parameters";
  }
}

Student StudentFactory::buildAsStudent() const
{
  return Student();
}

END_NAMESPACE_OPENTURNS
