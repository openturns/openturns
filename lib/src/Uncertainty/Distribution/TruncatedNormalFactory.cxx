//                                               -*- C++ -*-
/**
 *  @brief Factory for TruncatedNormal distribution
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
#include "TruncatedNormalFactory.hxx"
#include "SpecFunc.hxx"
#include "MaximumLikelihoodFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedNormalFactory);

/* Default constructor */
TruncatedNormalFactory::TruncatedNormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
TruncatedNormalFactory * TruncatedNormalFactory::clone() const
{
  return new TruncatedNormalFactory(*this);
}


/* Here is the interface that all derived class must implement */

TruncatedNormalFactory::Implementation TruncatedNormalFactory::build(const NumericalSample & sample) const
{
  return buildAsTruncatedNormal(sample).clone();
}

TruncatedNormalFactory::Implementation TruncatedNormalFactory::build(const NumericalPoint & parameters) const
{
  return buildAsTruncatedNormal(parameters).clone();
}

TruncatedNormalFactory::Implementation TruncatedNormalFactory::build() const
{
  return buildAsTruncatedNormal().clone();
}


TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal(const NumericalSample & sample) const
{
  const UnsignedInteger dimension = build()->getParameterDimension();
  const UnsignedInteger size = sample.getSize();
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a TruncatedNormal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // In order to avoid numerical stability issues, we normalize the data to [-1, 1]
  const NumericalScalar xMin = sample.getMin()[0];
  const NumericalScalar xMax = sample.getMax()[0];
  if (!SpecFunc::IsNormal(xMin) || !SpecFunc::IsNormal(xMax)) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution if data contains NaN or Inf";
  if (xMin == xMax)
    {
      const NumericalScalar delta(std::max(std::abs(xMin), 10.0) * SpecFunc::NumericalScalarEpsilon);
      TruncatedNormal result(xMin, 1.0, xMin - delta, xMax + delta);
      result.setDescription(sample.getDescription());
      return result;
    }
  // X_norm = alpha * (X - beta)
  const NumericalScalar alpha = 2.0 / (xMax - xMin);
  const NumericalScalar beta = 0.5 * (xMin + xMax);
  NumericalSample normalizedSample(sample);
  normalizedSample -= NumericalPoint(1, beta);
  normalizedSample *= NumericalPoint(1, alpha);


  NumericalPoint parametersLowerBound(dimension, -SpecFunc::MaxNumericalScalar);
  parametersLowerBound[1] = ResourceMap::GetAsNumericalScalar( "TruncatedNormalFactory-SigmaLowerBound");
  Interval::BoolCollection parametersLowerFlags(dimension, false);
  parametersLowerFlags[1] = true;
  NumericalPoint startingPoint(dimension);
  startingPoint[0] = normalizedSample.computeMean()[0];
  startingPoint[1] = normalizedSample.computeStandardDeviationPerComponent()[0];

  const NumericalScalar oneEps = 1.0 + 1.0 / size;

  MaximumLikelihoodFactory factory(buildAsTruncatedNormal());

  // bounds are fixed
  NumericalPoint knownParameterValues(2, oneEps);
  knownParameterValues[0] = -oneEps;
  Indices knownParameterIndices(2);
  knownParameterIndices.fill(2);
  factory.setKnownParameter(knownParameterValues, knownParameterIndices);

  // override starting point
  OptimizationSolver solver(factory.getOptimizationSolver());
  solver.setStartingPoint(startingPoint);
  factory.setOptimizationSolver(solver);

  // override bounds
  OptimizationProblem problem;
  problem.setBounds(Interval(parametersLowerBound, NumericalPoint(dimension, SpecFunc::MaxNumericalScalar), parametersLowerFlags, Interval::BoolCollection(dimension, false)));
  factory.setOptimizationProblem(problem);

  const NumericalPoint parameters(factory.buildParameter(normalizedSample));

  // The parameters are scaled back
  // X_norm = alpha * (X - beta)
  // X = beta + X_norm / alpha
  NumericalPoint scaledParameters(dimension, beta);
  scaledParameters[0] += parameters[0] / alpha;// mu
  scaledParameters[1] = parameters[1] / alpha;
  scaledParameters[2] -= oneEps / alpha;
  scaledParameters[3] += oneEps / alpha;

  TruncatedNormal result(buildAsTruncatedNormal(scaledParameters));
  result.setDescription(sample.getDescription());
  return result;
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal(const NumericalPoint & parameters) const
{
  try
  {
    TruncatedNormal distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution from the given parameters";
  }
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal() const
{
  return TruncatedNormal();
}

/* Optimization solver accessor */
OptimizationSolver TruncatedNormalFactory::getOptimizationSolver() const
{
  return solver_;
}

void TruncatedNormalFactory::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}
END_NAMESPACE_OPENTURNS
