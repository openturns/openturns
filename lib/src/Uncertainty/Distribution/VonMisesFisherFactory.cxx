//                                               -*- C++ -*-
/**
 *  @brief Factory for VonMisesFisher distribution
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
#include "openturns/VonMisesFisherFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VonMisesFisherFactory)

static const Factory<VonMisesFisherFactory> Factory_VonMisesFisherFactory;

/* Default constructor */
VonMisesFisherFactory::VonMisesFisherFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
VonMisesFisherFactory * VonMisesFisherFactory::clone() const
{
  return new VonMisesFisherFactory(*this);
}

class VonMisesFisherFactoryParameterConstraint: public EvaluationImplementation
{
public:

  /** Constructor from a sample and a derivative factor estimate */
  VonMisesFisherFactoryParameterConstraint(const Scalar logMuNorm, const UnsignedInteger dimension)
    : EvaluationImplementation()
    , logMuNorm_(logMuNorm)
    , dimension_(dimension)
  {
    // Nothing to do
  }

  VonMisesFisherFactoryParameterConstraint * clone() const override
  {
    return new VonMisesFisherFactoryParameterConstraint(*this);
  }

  Point operator()(const Point & parameter) const override
  {
    const Scalar kappa = parameter[0];
    if (!(kappa > 0.0)) throw InvalidArgumentException(HERE) << "Error: the kappa parameter must be positive.";
    return Point(1, SpecFunc::LogBesselInu(kappa, 0.5 * dimension_) - SpecFunc::LogBesselInu(kappa, 0.5 * dimension_ - 1.0) - logMuNorm_);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  String __repr__() const override
  {
    OSS oss(true);
    oss << "class=VonMisesFisherFactoryParameterConstraint"
        << " logMuNorm=" << logMuNorm_
        << " dimension=" << dimension_;
    return oss;
  }

  String __str__(const String & ) const override
  {
    OSS oss(false);
    oss << "VonMisesFisherFactoryParameterConstraint("
        << "logMuNorm=" << logMuNorm_
        << "dimension=" << dimension_
        << ")";
    return oss;
  }

private:
  Scalar logMuNorm_;
  UnsignedInteger dimension_;
};


/* Here is the interface that all derived class must implement */

Distribution VonMisesFisherFactory::build(const Sample & sample) const
{
  return buildAsVonMisesFisher(sample).clone();
}

Distribution VonMisesFisherFactory::build(const Point & parameters) const
{
  return buildAsVonMisesFisher(parameters).clone();
}

Distribution VonMisesFisherFactory::build() const
{
  return buildAsVonMisesFisher().clone();
}

VonMisesFisher VonMisesFisherFactory::buildAsVonMisesFisher(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a VonMisesFisher distribution from an empty sample";
  if (sample.getDimension() <= 1) throw InvalidArgumentException(HERE) << "Error: can build a VonMisesFisher distribution only from a sample of dimension greater or equal to 2, here dimension=" << sample.getDimension();
  if (!(sample.getMin() != sample.getMax())) throw InvalidArgumentException(HERE) << "Error: cannot build a VonMisesFisher distribution based on a constant sample";
  // Mu estimate is the sample mean
  Point mu = sample.computeMean();
  Scalar kappa = 0.0;
  const Scalar muNorm = mu.norm();
  // In the case where the empirical mean is exactly zero, the sample
  // is supposed to be uniformly distributed over the hypersphere so the
  // current values of mu and kappa are correct.
  // Otherwise estimate them
  if (muNorm > 0.0)
  {
    mu /= muNorm;
    LOGDEBUG(OSS() << "mu=" << mu);
    // Kappa estimate is based on muNorm = Inu(kappa, dim/2) / Inu(kappa, dim/2 - 1)
    // Work on log(muNorm) = logInu(kappa, dim/2) - logInu(kappa, dim/2 - 1)
    const VonMisesFisherFactoryParameterConstraint f(std::log(muNorm), mu.getDimension());
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
    Brent solver(ResourceMap::GetAsScalar( "VonMisesFisherFactory-AbsolutePrecision" ), ResourceMap::GetAsScalar( "VonMisesFisherFactory-RelativePrecision" ), ResourceMap::GetAsScalar( "VonMisesFisherFactory-ResidualPrecision" ), ResourceMap::GetAsUnsignedInteger( "VonMisesFisherFactory-MaximumIteration" ));
    // Kappa estimate
    kappa = solver.solve(f, 0.0, a, b, fA, fB);
  } // Estimate nontrivial (mu, kappa)
  VonMisesFisher result(mu, kappa);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

VonMisesFisher VonMisesFisherFactory::buildAsVonMisesFisher(const Point & parameters) const
{
  try
  {
    VonMisesFisher distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a VonMisesFisher distribution from the given parameters";
  }
}

VonMisesFisher VonMisesFisherFactory::buildAsVonMisesFisher() const
{
  return VonMisesFisher();
}

END_NAMESPACE_OPENTURNS
