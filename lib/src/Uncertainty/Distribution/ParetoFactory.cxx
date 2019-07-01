//                                               -*- C++ -*-
/**
 *  @brief Factory for Pareto distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ParetoFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/MethodOfMomentsFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/LeastSquaresDistributionFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParetoFactory)

static const Factory<ParetoFactory> Factory_ParetoFactory;

/* Default constructor */
ParetoFactory::ParetoFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ParetoFactory * ParetoFactory::clone() const
{
  return new ParetoFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution ParetoFactory::build(const Sample & sample) const
{
  return buildAsPareto(sample).clone();
}

Distribution ParetoFactory::build(const Point & parameters) const
{
  return buildAsPareto(parameters).clone();
}

Distribution ParetoFactory::build() const
{
  return buildAsPareto().clone();
}

Pareto ParetoFactory::buildMethodOfMoments(const Sample & sample) const
{
  // numerically inverse the skewness
  SymbolicFunction constraint("alpha", OSS() << "2*(1+alpha)/(alpha-3)*sqrt((alpha-2)/alpha)");
  const Scalar skewness = sample.computeSkewness()[0];
  if (skewness < constraint(Point(1, ResourceMap::GetAsScalar("ParetoFactory-AlphaUpperBound")))[0])
    throw InvalidArgumentException(HERE) << "alpha is not defined";
  const Brent solver;
  // alpha> 3.0 so as the skewness to be defined
  const Scalar alpha = solver.solve(constraint, skewness, 3.0 + std::sqrt(SpecFunc::Precision), ResourceMap::GetAsScalar("ParetoFactory-AlphaUpperBound"));
  const Scalar beta = (alpha - 1.0) * std::sqrt((alpha - 2.0) / alpha) * sample.computeStandardDeviationPerComponent()[0];
  const Scalar gamma = sample.computeMean()[0] - alpha * beta / (alpha - 1.0);

  Pareto result(beta, alpha, gamma);
  result.setDescription(sample.getDescription());
  return result;
}

Pareto ParetoFactory::buildMethodOfLikelihoodMaximization(const Sample & sample) const
{
  const MaximumLikelihoodFactory factory(buildMethodOfMoments(sample));
  return buildAsPareto(factory.build(sample).getParameter());
}

Pareto ParetoFactory::buildMethodOfLeastSquares(const Sample & sample) const
{
  const LeastSquaresDistributionFactory factory(buildMethodOfMoments(sample));
  return buildAsPareto(factory.build(sample).getParameter());
}

Pareto ParetoFactory::buildAsPareto(const Sample & sample) const
{
  return buildMethodOfLeastSquares(sample);
}

Pareto ParetoFactory::buildAsPareto(const Point & parameters) const
{
  try
  {
    Pareto distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Pareto distribution from the given parameters";
  }
}

Pareto ParetoFactory::buildAsPareto() const
{
  return Pareto();
}

END_NAMESPACE_OPENTURNS
