//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/RandomVector.hxx"
#include "openturns/ConstantRandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/EventRandomVector.hxx"
#include "openturns/EventDomain.hxx"
#include "openturns/EventProcess.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/Domain.hxx"
#include "openturns/ConditionalRandomVector.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicFunction.hxx"
#endif
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(RandomVector)

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
RandomVector::RandomVector(const Point & point)
  : TypedInterfaceObject<RandomVectorImplementation>(new ConstantRandomVector(point))
{
  LOGWARN(OSS() << "RandomVector(Point) is deprecated, use ConstantRandomVector");
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
  LOGWARN(OSS() << "RandomVector(Distribution, RandomVector) is deprecated, use ConditionalRandomVector");
}

/* Constructor for composite vector */
RandomVector::RandomVector(const Function & function,
                           const RandomVector & antecedent)
  : TypedInterfaceObject<RandomVectorImplementation>(new CompositeRandomVector(function,
      antecedent.getImplementation()))
{
  LOGWARN(OSS() << "RandomVector(Function, RandomVector) is deprecated, use CompositeRandomVector");
}

/* Constructor for functional chaos vector */
RandomVector::RandomVector(const FunctionalChaosResult & functionalChaosResult)
  : TypedInterfaceObject<RandomVectorImplementation>(new FunctionalChaosRandomVector(functionalChaosResult))
{
  LOGWARN(OSS() << "RandomVector(FunctionalChaosResult) is deprecated, use FunctionalChaosRandomVector");
}

/* Constructor from event RandomVector */
RandomVector::RandomVector(const RandomVector & antecedent,
                           const ComparisonOperator & op,
                           const Scalar threshold)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventRandomVector(*antecedent.getImplementation(), op, threshold))
{
  // Nothing to do
}

/* Constructor from domain event */
RandomVector::RandomVector(const RandomVector & antecedent,
                           const Domain & domain)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventDomain(*antecedent.getImplementation(), domain))
{
  // Nothing to do
}

RandomVector::RandomVector(const RandomVector & antecedent,
                           const Interval & interval)
  : TypedInterfaceObject<RandomVectorImplementation>(new EventDomain(*antecedent.getImplementation(), interval))
{
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  UnsignedInteger dimension = interval.getDimension();
  UnsignedInteger inputDimension = antecedent.getFunction().getInputDimension();
  Interval::BoolCollection finiteLowerBound(interval.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(interval.getFiniteUpperBound());
  Point lowerBound(interval.getLowerBound());
  Point upperBound(interval.getUpperBound());
  SymbolicFunction testFunction(Description::BuildDefault(inputDimension, "x"), Description(1, "0.0"));

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
      testFunction = SymbolicFunction("x", OSS() << "min(x-(" << lowerBound[0] << "), (" << upperBound[0] << ") - x)");
      CompositeRandomVector newVector(ComposedFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = RandomVector(newVector, Greater(), 0.0);
    }
    if (!finiteLowerBound[0] && !finiteUpperBound[0])
    {
      CompositeRandomVector newVector(Function(testFunction), antecedent.getAntecedent());
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
      CompositeRandomVector newVector(Function(testFunction), antecedent.getAntecedent());
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
      testFunction = SymbolicFunction(inVars, Description(1, formula));
      CompositeRandomVector newVector(ComposedFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
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
String RandomVector::__str__(const String & ) const
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
Point RandomVector::getRealization() const
{
  return getImplementation()->getRealization();
}

/* Numerical sample accessor */
Sample RandomVector::getSample(UnsignedInteger size) const
{
  return getImplementation()->getSample(size);
}

/* Mean accessor */
Point RandomVector::getMean() const
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
  return getImplementation()->getMarginal(i);
}

/* Get the random vector corresponding to the i-th marginal component */
RandomVector RandomVector::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* This method allows to access the antecedent RandomVector in case of a composite RandomVector */
RandomVector RandomVector::getAntecedent() const
{
  return getImplementation()->getAntecedent();
}

/* This method allows to access the Function in case of a composite RandomVector */
Function RandomVector::getFunction() const
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
Scalar RandomVector::getThreshold() const
{
  return getImplementation()->getThreshold();
}

/* Domain accessor */
Domain RandomVector::getDomain() const
{
  return getImplementation()->getDomain();
}

Point RandomVector::getParameter() const
{
  return getImplementation()->getParameter();
}

void RandomVector::setParameter(const Point & parameter)
{
  getImplementation()->setParameter(parameter);
}

Description RandomVector::getParameterDescription() const
{
  return getImplementation()->getParameterDescription();
}

END_NAMESPACE_OPENTURNS
