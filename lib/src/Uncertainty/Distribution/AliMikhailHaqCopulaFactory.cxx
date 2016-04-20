//                                               -*- C++ -*-
/**
 *  @brief Factory for AliMikhailHaqCopula distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>

#include "openturns/AliMikhailHaqCopulaFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AliMikhailHaqCopulaFactory);

/* Default constructor */
AliMikhailHaqCopulaFactory::AliMikhailHaqCopulaFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
AliMikhailHaqCopulaFactory * AliMikhailHaqCopulaFactory::clone() const
{
  return new AliMikhailHaqCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

AliMikhailHaqCopulaFactory::Implementation AliMikhailHaqCopulaFactory::build(const NumericalSample & sample) const
{
  return buildAsAliMikhailHaqCopula(sample).clone();
}

AliMikhailHaqCopulaFactory::Implementation AliMikhailHaqCopulaFactory::build(const NumericalPoint & parameters) const
{
  return buildAsAliMikhailHaqCopula(parameters).clone();
}

AliMikhailHaqCopulaFactory::Implementation AliMikhailHaqCopulaFactory::build() const
{
  return buildAsAliMikhailHaqCopula().clone();
}

AliMikhailHaqCopula AliMikhailHaqCopulaFactory::buildAsAliMikhailHaqCopula(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a AliMikhailHaqCopula distribution from an empty sample";
  if (sample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a AliMikhailHaqCopula distribution from a sample of dimension not equal to 2";
  NumericalScalar tau = sample.computeKendallTau().operator()(0, 1);
  if ((tau < (5.0 - 8.0 * std::log(2.0)) / 3.0) || (tau > 1.0 / 3.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a AliMikhailHaqCopula distribution from a sample with Kendall tau not in [(5-8ln(2))/3 ~ -0.182, 1/3~0.333]. Here tau=" << tau;
  // Search the value of the AliMikhailHaq copula parameter by numerically inverting the relation:
  // between Kendall's tau and AliMikhailHaq copula's parameter
  NumericalScalar theta = 0.0;
  NumericalScalar step = 0.5;
  NumericalScalar tauTheta = KendallTauFromParameter(theta);
  // Find a lower bound
  while (tauTheta > tau)
  {
    theta -= step;
    tauTheta = KendallTauFromParameter(theta);
    step *= 0.5;
  }
  // Here, tauTheta <= tau, hence theta is a lower bound of the parameter
  NumericalScalar minTheta = theta;
  // Now, look for an upper bound
  // If we started from a value of theta such that tauTheta > tau, theta + 2 * step is an upper bound
  NumericalScalar maxTheta = theta + 2.0 * step;
  // but if step = 0.5, it is because tauTheta was < tau for the initial choice of theta
  if (step < 0.5)
  {
    while (tauTheta <= tau)
    {
      theta += step;
      tauTheta = KendallTauFromParameter(theta);
      step *= 2.0;
    }
    // Here, we know that [theta - 0.5 * step, theta] is a bracketing interval
    minTheta = theta - 0.5 * step;
    maxTheta = theta;
  }
  // Bisection
  const NumericalScalar thetaEpsilon = ResourceMap::GetAsNumericalScalar( "AliMikhailHaqCopulaFactory-ThetaEpsilon" );
  while (maxTheta - minTheta > thetaEpsilon)
  {
    theta = 0.5 * (maxTheta + minTheta);
    tauTheta = KendallTauFromParameter(theta);
    if (tauTheta <= tau) minTheta = theta;
    else maxTheta = theta;
  }
  AliMikhailHaqCopula result(theta);
  result.setDescription(sample.getDescription());
  return result;
}

AliMikhailHaqCopula AliMikhailHaqCopulaFactory::buildAsAliMikhailHaqCopula(const NumericalPoint & parameters) const
{
  try
  {
    AliMikhailHaqCopula copula;
    copula.setParameter(parameters);
    return copula;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a AliMikhailHaqCopula from the given parameters";
  }
}

AliMikhailHaqCopula AliMikhailHaqCopulaFactory::buildAsAliMikhailHaqCopula() const
{
  return AliMikhailHaqCopula();
}

// Compute Kendall's tau from AliMikhailHaq copula's parameter. It is an increasing function
NumericalScalar AliMikhailHaqCopulaFactory::KendallTauFromParameter(const NumericalScalar theta) const
{
  return AliMikhailHaqCopula(theta).getKendallTau()(0, 1);
}

END_NAMESPACE_OPENTURNS
