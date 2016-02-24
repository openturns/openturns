//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
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
#include "openturns/RandomVector.hxx"
#include "openturns/ConstantRandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/EventRandomVectorImplementation.hxx"
#include "openturns/EventDomainImplementation.hxx"
#include "openturns/EventProcess.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/Domain.hxx"
#include "openturns/ConditionalRandomVector.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(RandomVector);

/* Default constructor */
RandomVector:: RandomVector()
  : TypedInterfaceObject<RandomVectorImplementation>(new RandomVectorImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
RandomVector::RandomVector(const RandomVectorImplementation & implementation)
  : TypedInterfaceObject<RandomVectorImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
RandomVector::RandomVector(const Implementation & p_implementation)
  : TypedInterfaceObject<RandomVectorImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
RandomVector::RandomVector(RandomVectorImplementation * p_implementation)
  : TypedInterfaceObject<RandomVectorImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor for constant vector */
RandomVector::RandomVector(const NumericalPoint & point)
  : TypedInterfaceObject<RandomVectorImplementation>(new ConstantRandomVector(point))
{
  // Nothing to do
}

/* Constructor for distribution-based vector */
RandomVector::RandomVector(const Distribution & distribution)
  : TypedInterfaceObject<RandomVectorImplementation>(new UsualRandomVector(distribution))
{
  // Nothing to do
}

/* Constructor for distribution-based conditional vector */
RandomVector::RandomVector(const Distribution & distribution,
                           const RandomVector & randomParameters)
  : TypedInterfaceObject<RandomVectorImplementation>(new ConditionalRandomVector(distribution, randomParameters))
{
  // Nothing to do
}

/* Constructor for composite vector */
RandomVector::RandomVector(const NumericalMathFunction & function,
                           const RandomVector & antecedent)
  : TypedInterfaceObject<RandomVectorImplementation>(new CompositeRandomVector(function,
      antecedent.getImplementation()))
{
  // Nothing to do
}

/* Constructor for functional chaos vector */
RandomVector::RandomVector(const FunctionalChaosResult & functionalChaosResult)
  : TypedInterfaceObject<RandomVectorImplementation>(new FunctionalChaosRandomVector(functionalChaosResult))
{
  // Nothing to do
}

/* Constructor from event RandomVector */
RandomVector::RandomVector(const RandomVector & antecedent,
                           const ComparisonOperator & op,
                           const NumericalScalar threshold)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventRandomVectorImplementation(*antecedent.getImplementation(), op, threshold))
{
  // Nothing to do
}

/* Constructor from domain event */
RandomVector::RandomVector(const RandomVector & antecedent,
                           const Domain & domain)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventDomainImplementation(*antecedent.getImplementation(), domain))
{
  // Nothing to do
}

RandomVector::RandomVector(const RandomVector & antecedent,
                           const Interval & interval)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventDomainImplementation(*antecedent.getImplementation(), interval))
{
#ifdef OPENTURNS_HAVE_MUPARSER
  UnsignedInteger dimension = interval.getDimension();
  UnsignedInteger inputDimension = antecedent.getFunction().getInputDimension();
  Interval::BoolCollection finiteLowerBound(interval.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(interval.getFiniteUpperBound());
  NumericalPoint lowerBound(interval.getLowerBound());
  NumericalPoint upperBound(interval.getUpperBound());
  NumericalMathFunction testFunction(Description::BuildDefault(inputDimension, "x"), Description(1, "0.0"));

  // easy case: 1d interval
  if (interval.getDimension() == 1)
  {
    if (finiteLowerBound[0] && !finiteUpperBound[0])
    {
      *this = RandomVector(antecedent, Greater(), lowerBound[0]);
    }
    if (!finiteLowerBound[0] && finiteUpperBound[0])
    {
      *this = RandomVector(antecedent, Less(), upperBound[0]);
    }

    if (finiteLowerBound[0] && finiteUpperBound[0])
    {
      testFunction = NumericalMathFunction("x", OSS() << "min(x-(" << lowerBound[0] << "), (" << upperBound[0] << ") - x)");
      RandomVector newVector(NumericalMathFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = RandomVector(newVector, Greater(), 0.0);
    }
    if (!finiteLowerBound[0] && !finiteUpperBound[0])
    {
      RandomVector newVector(NumericalMathFunction(testFunction), antecedent.getAntecedent());
      *this = RandomVector(newVector, Less(), 1.0);
    }
  }
  // general case
  else
  {
    Description inVars(Description::BuildDefault(dimension, "y"));
    Description slacks(0);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      if (finiteLowerBound[i])
        slacks.add(OSS() << inVars[i] << "-(" << lowerBound[i] << ")");
      if (finiteUpperBound[i])
        slacks.add(OSS() << "(" << upperBound[i] << ")-" << inVars[i]);
    }
    // No constraint
    if (slacks.getSize() == 0)
    {
      RandomVector newVector(NumericalMathFunction(testFunction), antecedent.getAntecedent());
      *this = RandomVector(newVector, Less(), 1.0);
    }
    else
    {
      String formula;
      if (slacks.getSize() == 1)
      {
        formula = slacks[0];
      }
      else
      {
        formula = "min(" + slacks[0];
        for (UnsignedInteger i = 1; i < slacks.getSize(); ++ i)
          formula += "," + slacks[i];
        formula += ")";
      }
      testFunction = NumericalMathFunction(inVars, Description(1, formula));
      RandomVector newVector(NumericalMathFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = RandomVector(newVector, Greater(), 0.0);
    }
  }
#endif
}

/* Constructor from domain event */
RandomVector::RandomVector(const Process & process,
                           const Domain & domain)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventProcess(process, domain))
{
  // Nothing to do
}


/* String converter */
String RandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << RandomVector::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String RandomVector::__str__(const String & offset) const
{
  return __repr__();
}



/* Description Accessor */
void RandomVector::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}


/* Description Accessor */
Description RandomVector::getDescription() const
{
  return getImplementation()->getDescription();
}


/* Is the underlying random vector composite ? */
Bool RandomVector::isComposite() const
{
  return getImplementation()->isComposite();
}


/* Here is the interface that all derived class may implement */

/* Dimension accessor */
UnsignedInteger RandomVector::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Realization accessor */
NumericalPoint RandomVector::getRealization() const
{
  return getImplementation()->getRealization();
}

/* Numerical sample accessor */
NumericalSample RandomVector::getSample(UnsignedInteger size) const
{
  return getImplementation()->getSample(size);
}

/* Mean accessor */
NumericalPoint RandomVector::getMean() const
{
  return getImplementation()->getMean();
}

/* Covariance accessor */
CovarianceMatrix RandomVector::getCovariance() const
{
  return getImplementation()->getCovariance();
}

/* Get the random vector corresponding to the i-th marginal component */
RandomVector RandomVector::getMarginal(const UnsignedInteger i) const
{
  return *(getImplementation()->getMarginal(i));
}

/* Get the random vector corresponding to the i-th marginal component */
RandomVector RandomVector::getMarginal(const Indices & indices) const
{
  return *(getImplementation()->getMarginal(indices));
}

/* This method allows to access the antecedent RandomVector in case of a composite RandomVector */
RandomVector::Antecedent RandomVector::getAntecedent() const
{
  return getImplementation()->getAntecedent();
}

/* This method allows to access the NumericalMathFunction in case of a composite RandomVector */
NumericalMathFunction RandomVector::getFunction() const
{
  return getImplementation()->getFunction();
}

/* This method allows to access the Distribution in case of a usual RandomVector */
Distribution RandomVector::getDistribution() const
{
  return getImplementation()->getDistribution();
}

/* Operator accessor */
ComparisonOperator RandomVector::getOperator() const
{
  return getImplementation()->getOperator();
}

/* Threshold accessor */
NumericalScalar RandomVector::getThreshold() const
{
  return getImplementation()->getThreshold();
}

END_NAMESPACE_OPENTURNS
