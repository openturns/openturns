//                                               -*- C++ -*-
/**
 *  @brief Factory for Rice distribution
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
#include "openturns/RiceFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RiceFactory)

static const Factory<RiceFactory> Factory_RiceFactory;

/* Default constructor */
RiceFactory::RiceFactory()
  : DistributionFactoryImplementation()
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
  RiceFactoryParameterConstraint(const Scalar r):
    r2p1_(1.0 + r * r)
  {
    // Nothing to do
  };

  Point computeConstraint(const Point & parameter) const
  {
    // Here u = \theta^2 wrt reference
    const Scalar u = parameter[0];
    const Scalar relation = u - (r2p1_ * computeXi(u) - 2.0);
    return Point(1, relation);
  }

  Scalar computeXi(const Scalar u) const
  {
    if (!(u > 0.0)) throw InvalidArgumentException(HERE) << "Error: the argument u=" << u << " in the constraint must be positive.";
    const Scalar up2 = u + 2.0;
    const Scalar quarterU = 0.25 * u;
    return up2 - 0.125 * M_PI * std::exp(-0.5 * u + 2.0 * SpecFunc::LogBesselI0(quarterU)) * std::pow(up2 + u * std::exp(SpecFunc::DeltaLogBesselI10(quarterU)), 2.0);
  }

  Scalar r2p1_;
};

/* Here is the interface that all derived class must implement */

Distribution RiceFactory::build(const Sample & sample) const
{
  return buildAsRice(sample).clone();
}

Distribution RiceFactory::build(const Point & parameters) const
{
  return buildAsRice(parameters).clone();
}

Distribution RiceFactory::build() const
{
  return buildAsRice().clone();
}

/* Parameters estimation using the Koay inversion technique, see:
   Koay, C. G. and Basser, P. J., "Analytically exact correction scheme for signal extraction from noisy magnitude MR signals", Journal of Magnetic Resonance, Vol. 179, 2, pp. 317-322 (2006)
*/
Rice RiceFactory::buildAsRice(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Rice distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Rice distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar mu = sample.computeMean()[0];
  const Scalar std = sample.computeStandardDeviationPerComponent()[0];
  // Koay inversion method
  RiceFactoryParameterConstraint constraint(mu / std);
  const Function f(bindMethod<RiceFactoryParameterConstraint, Point, Point>(constraint, &RiceFactoryParameterConstraint::computeConstraint, 1, 1));
  // Find a bracketing interval
  Scalar a = 1.0;
  Scalar b = 2.0;
  Scalar fA = f(Point(1, a))[0];
  Scalar fB = f(Point(1, b))[0];
  const Scalar largeValue = std::sqrt(SpecFunc::MaxScalar);
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger( "RiceFactory-MaximumIteration" );
  UnsignedInteger iteration = 0;
  // While f has the same sign at the two bounds, update the interval
  while ((fA * fB > 0.0) && (std::abs(fA) < largeValue) && (std::abs(fB) < largeValue) && (b < largeValue) && (iteration < maximumIteration))
  {
    a = 0.5 * a;
    fA = f(Point(1, a))[0];
    if (fA * fB <= 0.0) break;
    b = 2.0 * b;
    fB = f(Point(1, b))[0];
    LOGDEBUG(OSS() << "a=" << a << ", fa=" << fA << ", b=" << b << ", fb=" << fB);
    ++iteration;
  }
  if ((std::abs(fA) > largeValue) || (std::abs(fB) > largeValue) || (std::abs(b) > largeValue) || (iteration == maximumIteration)) throw InvalidArgumentException(HERE) << "Error: cannot estimate parameters of a Rice distribution from the given sample";
  // Solve the constraint equation
  Brent solver(ResourceMap::GetAsScalar( "RiceFactory-AbsolutePrecision" ), ResourceMap::GetAsScalar( "RiceFactory-RelativePrecision" ), ResourceMap::GetAsScalar( "RiceFactory-ResidualPrecision" ), maximumIteration);
  // u estimate
  const Scalar u = solver.solve(f, 0.0, a, b, fA, fB);
  const Scalar xiU = constraint.computeXi(u);
  // Corresponding beta estimate
  const Scalar beta = std / std::sqrt(xiU);
  // Corresponding nu estimate
  const Scalar nu = std::sqrt(mu * mu + beta * beta * (xiU - 2.0));
  try
  {
    Rice result(beta, nu);
    result.setDescription(sample.getDescription());
    return result;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot estimate parameters of a Rice distribution from the given sample";
  }
}

Rice RiceFactory::buildAsRice(const Point & parameters) const
{
  try
  {
    Rice distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Rice distribution from the given parameters";
  }
}

Rice RiceFactory::buildAsRice() const
{
  return Rice();
}

END_NAMESPACE_OPENTURNS
