//                                               -*- C++ -*-
/**
 *  @brief Factory for Trapezoidal distribution
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
#include "TrapezoidalFactory.hxx"
#include <limits>
#include "Distribution.hxx"
#include "SpecFunc.hxx"
#include "ResourceMap.hxx"
#include "algocobyla.h"
#include "Cobyla.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrapezoidalFactory);

/* Default constructor */
TrapezoidalFactory::TrapezoidalFactory()
  : DistributionImplementationFactory()
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Virtual constructor */
TrapezoidalFactory * TrapezoidalFactory::clone() const
{
  return new TrapezoidalFactory(*this);
}


NumericalScalar TrapezoidalFactory::computeLogLikelihood(const NumericalPoint & x) const
{
  NumericalScalar result(0.0);
  const UnsignedInteger size(sample_.getSize());
  NumericalPointCollection coll;
  coll.add(x);
  const Trapezoidal distribution(buildAsTrapezoidal( coll ));
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const NumericalScalar pdf(distribution.computePDF(sample_[i]));
    if ( pdf > 0.0 ) result += std::log(pdf);
    else result += SpecFunc::LogMinNumericalScalar;
  }
  return result;
}


int TrapezoidalFactory::ComputeObjectiveAndConstraint(int n,
    int m,
    double *x,
    double *f,
    double *con,
    void *state)
{
  TrapezoidalFactory * factory = static_cast<TrapezoidalFactory *>(state);
  const NumericalScalar a(x[0]);
  const NumericalScalar b(x[1]);
  const NumericalScalar c(x[2]);
  const NumericalScalar d(x[3]);
  NumericalPoint vec(0);
  vec.add(a);
  vec.add(b);
  vec.add(c);
  vec.add(d);

  /* Compute the likelihood function at [a,b,c,d] */
  *f = - factory->computeLogLikelihood( vec );

  /* Compute the constraints */
  con[0] = b - a;//a<=b
  con[1] = c - b - SpecFunc::MinNumericalScalar;// b<c
  con[2] = d - c;// c<=d
  return 0;
}


/* Here is the interface that all derived class must implement */

TrapezoidalFactory::Implementation TrapezoidalFactory::build(const NumericalSample & sample) const
{
  return buildAsTrapezoidal(sample).clone();
}

TrapezoidalFactory::Implementation TrapezoidalFactory::build(const NumericalPointCollection & parameters) const
{
  return buildAsTrapezoidal(parameters).clone();
}

TrapezoidalFactory::Implementation TrapezoidalFactory::build() const
{
  return buildAsTrapezoidal().clone();
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Trapezoidal distribution from an empty sample";

  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a Trapezoidal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const NumericalScalar min(sample.getMin()[0]);
  const NumericalScalar max(sample.getMax()[0]);

  // number of optimized variables
  int n(4);

  // number of constraints
  int m(3);

  // starting point
  NumericalPoint x(4);
  x[0] = min - std::abs( min ) / ( 2.0 + size );// a
  x[1] = sample.computeQuantilePerComponent( 0.25 )[0];// b
  x[2] = sample.computeQuantilePerComponent( 0.75 )[0];// c
  x[3] = max + std::abs( max ) / ( 2.0 + size );// d

  NumericalScalar rhoBeg(ResourceMap::GetAsNumericalScalar( "TrapezoidalFactory-RhoBeg" ));
  NumericalScalar rhoEnd(ResourceMap::GetAsNumericalScalar( "TrapezoidalFactory-RhoEnd" ));
  int maxFun(ResourceMap::GetAsUnsignedInteger( "TrapezoidalFactory-MaximumIteration" ));
  cobyla_message message( COBYLA_MSG_NONE );

  // use attributes to pass the data
  sample_ = sample;

  // call cobyla algo
  int returnCode(cobyla( n, m, &x[0], rhoBeg, rhoEnd, message, &maxFun, TrapezoidalFactory::ComputeObjectiveAndConstraint, (void*) this ));
  if (returnCode != 0)
  {
    LOGWARN(OSS() << "Error solving maximum likelihood problem by cobyla method, message=" << cobyla_rc_string[returnCode - COBYLA_MINRC]);
  }
  Trapezoidal result(x[0], x[1], x[2], x[3]);
  result.setDescription(sample.getDescription());
  return result;
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const NumericalPointWithDescriptionCollection & parameters) const
{
  return buildAsTrapezoidal(RemoveDescriptionFromCollection(parameters));
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const NumericalPointCollection & parameters) const
{
  try
  {
    Trapezoidal distribution;
    distribution.setParametersCollection(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Trapezoidal distribution from the given parameters: " << parameters;
  }
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal() const
{
  return Trapezoidal();
}

END_NAMESPACE_OPENTURNS
