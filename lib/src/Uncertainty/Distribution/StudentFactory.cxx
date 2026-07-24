//                                               -*- C++ -*-
/**
 *  @brief Factory for Student distribution
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
#include "openturns/StudentFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
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


class StudentFactoryLogLikelihood : public EvaluationImplementation
{
public:
  /** Constructor from a sample, fixed mu, and correlation matrix */
  StudentFactoryLogLikelihood(const Sample & sample,
                              const Point & mu,
                              const CorrelationMatrix & R,
                              const Point & sigmaTip,
                              const UnsignedInteger reducedDimThreshold)
    : sample_(sample)
    , d_(sample.getDimension())
    , mu_(mu)
    , R_(R)
    , sigmaTip_(sigmaTip)
    , reducedDimThreshold_(reducedDimThreshold)
  {
    // Nothing to do
  }

  StudentFactoryLogLikelihood * clone() const override
  {
    return new StudentFactoryLogLikelihood(*this);
  }

  Point operator() (const Point & parameter) const override
  {
    const Scalar nu = parameter[0];
    if (nu <= 0.0) return Point(1, SpecFunc::LowestScalar);

    Point sigma(d_);
    if (d_ <= reducedDimThreshold_)
    {
      for (UnsignedInteger j = 0; j < d_; ++ j)
      {
        sigma[j] = parameter[1 + j];
        if (sigma[j] <= 0.0) return Point(1, SpecFunc::LowestScalar);
      }
    }
    else
    {
      sigma = sigmaTip_;
    }
    return Student(nu, mu_, sigma, R_).computeLogPDF(sample_).computeMean();
  }

  UnsignedInteger getInputDimension() const override
  {
    return (d_ <= reducedDimThreshold_) ? 1 + d_ : 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  Sample sample_;
  UnsignedInteger d_;
  Point mu_;
  CorrelationMatrix R_;
  Point sigmaTip_;
  UnsignedInteger reducedDimThreshold_;
};


Student StudentFactory::buildAsStudent(const Sample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Student distribution from a sample of size < 2";
  const Point mu(sample.computeMedian());
  const CorrelationMatrix R(NormalCopula::GetCorrelationFromKendallCorrelation(sample.computeKendallTau()));
  const UnsignedInteger d = sample.getDimension();

  // Robust scale estimate
  const Point q25(sample.computeQuantilePerComponent(0.25));
  const Point q75(sample.computeQuantilePerComponent(0.75));
  Point sigmaTip(d);
  for (UnsignedInteger j = 0; j < d; ++ j)
    sigmaTip[j] = (q75[j] - q25[j]) / 1.349;

  const UnsignedInteger reducedDimThreshold = 5;
  StudentFactoryLogLikelihood logLikelihood(sample, mu, R, sigmaTip, reducedDimThreshold);
  const Function objective(logLikelihood.clone());
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  const UnsignedInteger dim = (d <= reducedDimThreshold) ? 1 + d : 1;
  Point lowerBound(dim, 0.0);
  Point upperBound(dim, SpecFunc::Infinity);
  Interval::BoolCollection finiteLowerBound(dim, false);
  Interval::BoolCollection finiteUpperBound(dim, false);
  lowerBound[0] = SpecFunc::ScalarEpsilon;
  finiteLowerBound[0] = true;
  if (d <= reducedDimThreshold)
  {
    for (UnsignedInteger j = 0; j < d; ++ j)
    {
      lowerBound[1 + j] = SpecFunc::ScalarEpsilon;
      finiteLowerBound[1 + j] = true;
    }
  }
  problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

  Scalar nu0 = 1.0;
  Point startingPoint(dim);
  startingPoint[0] = nu0;
  if (d <= reducedDimThreshold)
  {
    for (UnsignedInteger j = 0; j < d; ++ j)
      startingPoint[1 + j] = sigmaTip[j];
  }

  TNC solver(problem);
  solver.setStartingPoint(startingPoint);
  solver.run();
  const Point optimalParameter(solver.getResult().getOptimalPoint());
  const Scalar nu = optimalParameter[0];
  Point optimalSigma(d);
  if (d <= reducedDimThreshold)
  {
    for (UnsignedInteger j = 0; j < d; ++ j)
      optimalSigma[j] = optimalParameter[1 + j];
  }
  else
  {
    optimalSigma = sigmaTip;
  }
  Student result(nu, mu, optimalSigma, R);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
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
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Student distribution from the given parameters";
  }
}

Student StudentFactory::buildAsStudent() const
{
  return Student();
}

END_NAMESPACE_OPENTURNS
