//                                               -*- C++ -*-
/**
 *  @brief Factory for VonMises distribution
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
#include "openturns/VonMisesFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VonMisesFactory)

static const Factory<VonMisesFactory> Factory_VonMisesFactory;

/* Default constructor */
VonMisesFactory::VonMisesFactory(const Bool circular)
  : DistributionFactoryImplementation()
  , circular_(circular)
{
  // Nothing to do
}

/* Virtual constructor */
VonMisesFactory * VonMisesFactory::clone() const
{
  return new VonMisesFactory(*this);
}

class VonMisesFactoryParameterConstraint: public EvaluationImplementation
{
public:

  /** Constructor from a sample and a derivative factor estimate */
  VonMisesFactoryParameterConstraint(const Scalar logMeanCos)
    : EvaluationImplementation()
    , logMeanCos_(logMeanCos)
  {
    // Nothing to do
  }

  VonMisesFactoryParameterConstraint * clone() const
  {
    return new VonMisesFactoryParameterConstraint(*this);
  }

  Point operator()(const Point & parameter) const
  {
    const Scalar kappa = parameter[0];
    if (!(kappa > 0.0)) throw InvalidArgumentException(HERE) << "Error: the kappa parameter must be positive.";
    return Point(1, SpecFunc::DeltaLogBesselI10(kappa) - logMeanCos_);
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  String __repr__() const
  {
    OSS oss(true);
    oss << "class=VonMisesFactoryParameterConstraint"
        << " logMeanCos=" << logMeanCos_;
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss(false);
    oss << "VonMisesFactoryParameterConstraint("
        << "logMeanCos=" << logMeanCos_
        << ")";
    return oss;
  }

private:
  Scalar logMeanCos_;
};


/* Here is the interface that all derived class must implement */

Distribution VonMisesFactory::build(const Sample & sample) const
{
  return buildAsVonMises(sample).clone();
}

Distribution VonMisesFactory::build(const Point & parameters) const
{
  return buildAsVonMises(parameters).clone();
}

Distribution VonMisesFactory::build() const
{
  return buildAsVonMises().clone();
}

VonMises VonMisesFactory::buildAsVonMises(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a VonMises distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a VonMises distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (!(sample.getMin()[0] != sample.getMax()[0])) throw InvalidArgumentException(HERE) << "Error: cannot build a VonMises distribution based on a constant sample";
  // In the case of circular data
  Scalar mu = 0.0;
  Scalar ratio = 0.0;
  if (circular_)
  {
    Scalar sumCosXk = 0.0;
    Scalar sumSinXk = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      sumCosXk += std::cos(sample(i, 0));
      sumSinXk += std::sin(sample(i, 0));
    }
    sumCosXk /= size;
    sumSinXk /= size;
    mu = std::atan2(sumSinXk, sumCosXk);
    // Here we know that size > 1 otherwise the sample would be constant
    ratio = sqrt((size / (size - 1.0)) * (sumCosXk * sumCosXk + sumSinXk * sumSinXk - 1.0 / size));
  }
  // Use two moments: mu=E[X]
  else
  {
    // Mu estimate is the sample mean
    mu = sample.computeMean()[0];
    for (UnsignedInteger i = 0; i < size; ++i)
      ratio += std::cos(sample(i, 0) - mu);
    ratio /= size;
  }
  LOGDEBUG(OSS() << "mu=" << mu);
  // Kappa estimate is based on E[cos(X-mu)]=I0(kappa)/I1(kappa)
  // We work on log scale to avoid overflow in I0 and I1
  if (!(ratio > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a VonMises distribution if ratio is nonpositive, here ratio=" << ratio;
  LOGDEBUG(OSS() << "ratio=" << ratio);
  const VonMisesFactoryParameterConstraint f(std::log(ratio));
  // Find a bracketing interval
  Scalar a = 1.0;
  Scalar b = 2.0;
  Scalar fA = f(Point(1, a))[0];
  Scalar fB = f(Point(1, b))[0];
  // While f has the same sign at the two bounds, update the interval
  while (fA * fB > 0.0)
  {
    a = 0.5 * a;
    fA = f(Point(1, a))[0];
    if (fA * fB <= 0.0) break;
    b = 2.0 * b;
    fB = f(Point(1, b))[0];
  }
  // Solve the constraint equation
  Brent solver(ResourceMap::GetAsScalar( "VonMisesFactory-AbsolutePrecision" ), ResourceMap::GetAsScalar( "VonMisesFactory-RelativePrecision" ), ResourceMap::GetAsScalar( "VonMisesFactory-ResidualPrecision" ), ResourceMap::GetAsUnsignedInteger( "VonMisesFactory-MaximumIteration" ));
  // Kappa estimate
  const Scalar kappa = solver.solve(f, 0.0, a, b, fA, fB);

  VonMises result(mu, kappa);
  result.setDescription(sample.getDescription());
  return result;
}

VonMises VonMisesFactory::buildAsVonMises(const Point & parameters) const
{
  try
  {
    VonMises distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a VonMises distribution from the given parameters";
  }
}

VonMises VonMisesFactory::buildAsVonMises() const
{
  return VonMises();
}

END_NAMESPACE_OPENTURNS
