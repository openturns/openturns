//                                               -*- C++ -*-
/**
 *  @brief Factory for Dirichlet distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "DirichletFactory.hxx"
#include "ResourceMap.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DirichletFactory);

/* Default constructor */
DirichletFactory::DirichletFactory():
  DistributionImplementationFactory()
{
  // Nothing to do
}

/* Virtual constructor */
DirichletFactory * DirichletFactory::clone() const
{
  return new DirichletFactory(*this);
}


/* Here is the interface that all derived class must implement */

DirichletFactory::Implementation DirichletFactory::build(const NumericalSample & sample) const
{
  return buildAsDirichlet(sample).clone();
}

DirichletFactory::Implementation DirichletFactory::build(const NumericalPoint & parameters) const
{
  return buildAsDirichlet(parameters).clone();
}

DirichletFactory::Implementation DirichletFactory::build() const
{
  return buildAsDirichlet().clone();
}

Dirichlet DirichletFactory::buildAsDirichlet(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Dirichlet distribution from a sample of size < 2";
  const UnsignedInteger dimension(sample.getDimension());
  // Check that the points lie in the simplex x_1+...+x_d < 1, x_k > 0
  // and precompute the sufficient statistics
  NumericalPoint meanLog(dimension + 1);
  NumericalPoint sumX(dimension, 0.0);
  NumericalPoint sumX2(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    NumericalScalar sum(0.0);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const NumericalScalar xIJ(sample[i][j]);
      if (xIJ <= 0.0) throw InvalidArgumentException(HERE) << "Error: the sample contains points not in the unit simplex: x=" << sample[i];
      sum += xIJ;
      meanLog[j] += std::log(xIJ);
      sumX[j] += xIJ;
      sumX2[j] += xIJ * xIJ;
    }
    if (sum >= 1.0) throw InvalidArgumentException(HERE) << "Error: the sample contains points not in the unit simplex: x=" << sample[i];
    meanLog[dimension] += log1p(-sum);
  }
  // Normalize the sum of the logarithms
  meanLog = meanLog * (1.0 / size);
  // Find the maximum likelihood estimate using a fixed-point strategy
  // First, compute a reasonable initial guess using moments
  NumericalPoint theta(dimension + 1, 0.0);
  NumericalScalar sumTheta(0.0);
  // Estimate the sum of parameters
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar sumXI(sumX[i]);
    const NumericalScalar sumX2I(sumX2[i]);
    const NumericalScalar numerator(sumXI - sumX2I);
    const NumericalScalar denominator(sumX2I - sumXI * sumXI / size);
    if (denominator == 0.0) throw InvalidArgumentException(HERE) << "Error: the component " << i << " of the sample is constant (equal to " << sumXI / size << "). Impossible to estimate a Dirichlet distribution.";
    sumTheta += numerator / denominator;
  }
  sumTheta /= dimension;
  // Estimate the parameters from the mean of the sample
  NumericalScalar lastTheta(sumTheta);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar thetaI((sumX[i] / size) * sumTheta);
    // If the estimate is positive, use it, if not, use a default value of ResourceMap::GetAsNumericalScalar( "DirichletFactory-ParametersEpsilon" )
    theta[i] = (thetaI > 0.0 ? thetaI : ResourceMap::GetAsNumericalScalar( "DirichletFactory-ParametersEpsilon" ));
    lastTheta -= theta[i];
  }
  // If the estimate is positive, use it, if not, use a default value of ResourceMap::GetAsNumericalScalar( "DirichletFactory-ParametersEpsilon" )
  theta[dimension] = (lastTheta > 0.0 ? lastTheta : ResourceMap::GetAsNumericalScalar( "DirichletFactory-ParametersEpsilon" ));
  Bool convergence(false);
  UnsignedInteger iteration(0);
  while (!convergence && (iteration < ResourceMap::GetAsUnsignedInteger( "DirichletFactory-MaximumIteration" )))
  {
    // Newton iteration
    ++iteration;
    sumTheta = 0.0;
    for (UnsignedInteger i = 0; i <= dimension; ++i) sumTheta += theta[i];
    const NumericalScalar diGammaSumTheta(SpecFunc::DiGamma(sumTheta));
    const NumericalScalar triGammaSumTheta(SpecFunc::TriGamma(sumTheta));
    NumericalPoint g(dimension + 1);
    NumericalPoint q(dimension + 1);
    NumericalScalar numerator(0.0);
    NumericalScalar denominator(0.0);
    for (UnsignedInteger i = 0; i <= dimension; ++i)
    {
      g[i] = meanLog[i] - SpecFunc::DiGamma(theta[i]) + diGammaSumTheta;
      q[i] = -SpecFunc::TriGamma(theta[i]);
      numerator += g[i] / q[i];
      denominator += 1.0 / q[i];
    }
    const NumericalScalar b(numerator / (1.0 / triGammaSumTheta + denominator));
    NumericalPoint delta(dimension + 1);
    for (UnsignedInteger i = 0; i <= dimension; ++i) delta[i] = (g[i] - b) / q[i];
    // Newton update
    theta = theta - delta;
    convergence = (delta.norm() < dimension * ResourceMap::GetAsNumericalScalar( "DirichletFactory-ParametersEpsilon" ));
  }
  // Fixed point algorithm, works but is slow. Should never go there, as the Newton iteration should converge
  iteration = 0;
  while (!convergence && (iteration < ResourceMap::GetAsUnsignedInteger( "DirichletFactory-MaximumIteration" )))
  {
    ++ iteration;
    sumTheta = 0.0;
    for (UnsignedInteger i = 0; i <= dimension; ++i) sumTheta += theta[i];
    const NumericalScalar psiSumTheta(SpecFunc::DiGamma(sumTheta));
    NumericalScalar delta(0.0);
    for (UnsignedInteger i = 0; i <= dimension; ++i)
    {
      const NumericalScalar thetaI(SpecFunc::DiGammaInv(psiSumTheta + meanLog[i]));
      delta += std::abs(theta[i] - thetaI);
      theta[i] = thetaI;
    }
    convergence = (delta < dimension * ResourceMap::GetAsNumericalScalar( "DirichletFactory-ParametersEpsilon" ));
  }
  Dirichlet result(theta);
  result.setDescription(sample.getDescription());
  return result;
}

Dirichlet DirichletFactory::buildAsDirichlet(const NumericalPoint & parameters) const
{
  try
  {
    Dirichlet distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Dirichlet distribution from the given parameters";
  }
}

Dirichlet DirichletFactory::buildAsDirichlet() const
{
  return Dirichlet();
}

END_NAMESPACE_OPENTURNS
