//                                               -*- C++ -*-
/**
 *  @brief Factory for FrankCopula distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>

#include "openturns/FrankCopulaFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FrankCopulaFactory)

static const Factory<FrankCopulaFactory> Factory_FrankCopulaFactory;

/* Default constructor */
FrankCopulaFactory::FrankCopulaFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
FrankCopulaFactory * FrankCopulaFactory::clone() const
{
  return new FrankCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution FrankCopulaFactory::build(const Sample & sample) const
{
  return buildAsFrankCopula(sample).clone();
}

Distribution FrankCopulaFactory::build(const Point & parameters) const
{
  return buildAsFrankCopula(parameters).clone();
}

Distribution FrankCopulaFactory::build() const
{
  return buildAsFrankCopula().clone();
}

FrankCopula FrankCopulaFactory::buildAsFrankCopula(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a FrankCopula distribution from an empty sample";
  if (sample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a FrankCopula distribution from a sample of dimension not equal to 2";
  Scalar tau = sample.computeKendallTau().operator()(0, 1);
  if (tau == 1.0) throw InvalidArgumentException(HERE) << "Error: cannot build a FrankCopula distribution from a sample with Kendall tau equal to 1";
  if (tau == -1.0) throw InvalidArgumentException(HERE) << "Error: cannot build a FrankCopula distribution from a sample with Kendall tau equal to -1";
  // Search the value of the Frank copula parameter by numerically inverting the relation between Kendall's tau and Frank copula's parameter
  const Bool isTauNegative = tau < 0.0;
  tau = std::abs(tau);
  Scalar theta = 1.0;
  Scalar step = 0.5;
  Scalar tauTheta = KendallTauFromParameter(Point(1, theta))[0];
  // Find a lower bound
  while (tauTheta > tau)
  {
    theta -= step;
    tauTheta = KendallTauFromParameter(Point(1, theta))[0];
    step *= 0.5;
  }
  // Here, tauTheta <= tau, hence theta is a lower bound of the parameter
  const Scalar minTheta = theta;
  const Scalar minTau = tauTheta;
  // Now, look for an upper bound
  while (tauTheta <= tau)
  {
    theta += step;
    tauTheta = KendallTauFromParameter(Point(1, theta))[0];
    step *= 2.0;
  }
  const Scalar maxTheta = theta;
  const Scalar maxTau = tauTheta;
  const Function f(bindMethod<FrankCopulaFactory, Point, Point>(*this, &FrankCopulaFactory::KendallTauFromParameter, 1, 1));
  // Solve the constraint equation
  Brent solver(ResourceMap::GetAsScalar( "FrankCopulaFactory-AbsolutePrecision" ), ResourceMap::GetAsScalar( "FrankCopulaFactory-RelativePrecision" ), ResourceMap::GetAsScalar( "FrankCopulaFactory-ResidualPrecision" ), ResourceMap::GetAsUnsignedInteger( "FrankCopulaFactory-MaximumIteration" ));
  theta = solver.solve(f, tau, minTheta, maxTheta, minTau, maxTau);
  FrankCopula result(isTauNegative ? -theta : theta);
  result.setDescription(sample.getDescription());
  return result;
}

FrankCopula FrankCopulaFactory::buildAsFrankCopula(const Point & parameters) const
{
  try
  {
    FrankCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a FrankCopula from the given parameters";
  }
}

FrankCopula FrankCopulaFactory::buildAsFrankCopula() const
{
  return FrankCopula();
}

// Compute Kendall's tau from Frank copula's parameter. It is an increasing function
Point FrankCopulaFactory::KendallTauFromParameter(const Point & theta) const
{
  return Point(1, FrankCopula(theta[0]).getKendallTau()(0, 1));
}

END_NAMESPACE_OPENTURNS
