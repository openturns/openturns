//                                               -*- C++ -*-
/**
 *  @brief Factory for Rice distribution
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
#include "RiceFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "SpecFunc.hxx"
#include "Brent.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RiceFactory);

/* Default constructor */
RiceFactory::RiceFactory()
  : DistributionImplementationFactory()
{
  // Nothing to do
}

/* Virtual constructor */
RiceFactory * RiceFactory::clone() const
{
  return new RiceFactory(*this);
}

struct RiceFactoryParameterConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  RiceFactoryParameterConstraint(const NumericalScalar r):
    r2p1_(1.0 + r * r)
  {
    // Nothing to do
  };

  NumericalPoint computeConstraint(const NumericalPoint & parameter) const
  {
    // Here u = \theta^2 wrt reference
    const NumericalScalar u(parameter[0]);
    const NumericalScalar relation(u - (r2p1_ * computeXi(u) - 2.0));
    return NumericalPoint(1, relation);
  }

  NumericalScalar computeXi(const NumericalScalar u) const
  {
    if (u <= 0.0) throw InvalidArgumentException(HERE) << "Error: the argument u=" << u << " in the constraint must be positive.";
    const NumericalScalar up2(u + 2.0);
    const NumericalScalar quarterU(0.25 * u);
    return up2 - 0.125 * M_PI * std::exp(-0.5 * u + 2.0 * SpecFunc::LogBesselI0(quarterU)) * std::pow(up2 + u * std::exp(SpecFunc::DeltaLogBesselI10(quarterU)), 2.0);
  }

  NumericalScalar r2p1_;
};

/* Here is the interface that all derived class must implement */

RiceFactory::Implementation RiceFactory::build(const NumericalSample & sample) const
{
  return buildAsRice(sample).clone();
}

RiceFactory::Implementation RiceFactory::build(const NumericalPoint & parameters) const
{
  return buildAsRice(parameters).clone();
}

RiceFactory::Implementation RiceFactory::build() const
{
  return buildAsRice().clone();
}

/* Parameters estimation using the Koay inversion technique, see:
   Koay, C. G. and Basser, P. J., "Analytically exact correction scheme for signal extraction from noisy magnitude MR signals", Journal of Magnetic Resonance, Vol. 179, 2, pp. 317-322 (2006)
*/
Rice RiceFactory::buildAsRice(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Rice distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Rice distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const NumericalScalar mu(sample.computeMean()[0]);
  const NumericalScalar std(sample.computeStandardDeviationPerComponent()[0]);
  // Koay inversion method
  RiceFactoryParameterConstraint constraint(mu / std);
  const NumericalMathFunction f(bindMethod<RiceFactoryParameterConstraint, NumericalPoint, NumericalPoint>(constraint, &RiceFactoryParameterConstraint::computeConstraint, 1, 1));
  // Find a bracketing interval
  NumericalScalar a(1.0);
  NumericalScalar b(2.0);
  NumericalScalar fA(f(NumericalPoint(1, a))[0]);
  NumericalScalar fB(f(NumericalPoint(1, b))[0]);
  const NumericalScalar largeValue(std::sqrt(SpecFunc::MaxNumericalScalar));
  const UnsignedInteger maximumIteration(ResourceMap::GetAsUnsignedInteger( "RiceFactory-MaximumIteration" ));
  UnsignedInteger iteration(0);
  // While f has the same sign at the two bounds, update the interval
  while ((fA * fB > 0.0) && (std::abs(fA) < largeValue) && (std::abs(fB) < largeValue) && (b < largeValue) && (iteration < maximumIteration))
  {
    a = 0.5 * a;
    fA = f(NumericalPoint(1, a))[0];
    if (fA * fB <= 0.0) break;
    b = 2.0 * b;
    fB = f(NumericalPoint(1, b))[0];
    LOGINFO(OSS() << "a=" << a << ", fa=" << fA << ", b=" << b << ", fb=" << fB);
    ++iteration;
  }
  if ((std::abs(fA) > largeValue) || (std::abs(fB) > largeValue) || (std::abs(b) > largeValue) || (iteration == maximumIteration)) throw InvalidArgumentException(HERE) << "Error: cannot estimate parameters of a Rice distribution from the given sample";
  // Solve the constraint equation
  Brent solver(ResourceMap::GetAsNumericalScalar( "RiceFactory-AbsolutePrecision" ), ResourceMap::GetAsNumericalScalar( "RiceFactory-RelativePrecision" ), ResourceMap::GetAsNumericalScalar( "RiceFactory-ResidualPrecision" ), maximumIteration);
  // u estimate
  const NumericalScalar u(solver.solve(f, 0.0, a, b, fA, fB));
  const NumericalScalar xiU(constraint.computeXi(u));
  // Corresponding sigma estimate
  const NumericalScalar sigma(std / std::sqrt(xiU));
  // Corresponding nu estimate
  const NumericalScalar nu(std::sqrt(mu * mu + sigma * sigma * (xiU - 2.0)));
  try
  {
    Rice result(sigma, nu);
    result.setDescription(sample.getDescription());
    return result;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot estimate parameters of a Rice distribution from the given sample";
  }
}

Rice RiceFactory::buildAsRice(const NumericalPoint & parameters) const
{
  try
  {
    Rice distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Rice distribution from the given parameters";
  }
}

Rice RiceFactory::buildAsRice() const
{
  return Rice();
}

END_NAMESPACE_OPENTURNS
