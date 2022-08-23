//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ThresholdEventImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ThresholdEventImplementation)

static const Factory<ThresholdEventImplementation> Factory_ThresholdEventImplementation;

/* Default constructor */
ThresholdEventImplementation::ThresholdEventImplementation()
  : CompositeRandomVector()
  , operator_()
  , threshold_()
{
  // Nothing to do
}

/* Constructor from RandomVector */
ThresholdEventImplementation::ThresholdEventImplementation(const RandomVector & antecedent,
    const ComparisonOperator & op,
    const Scalar threshold)
  : CompositeRandomVector()
  , operator_(op)
  , threshold_(threshold)
{
  // Event can only be constructed from composite random vectors
  if (!antecedent.isComposite())
    throw InvalidArgumentException(HERE) << "Event can only be constructed from composite random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of ThresholdEventImplementation "
                                         << " has incorrect type";
  // ThresholdEventImplementation can only be constructed from 1D random vectors
  if (antecedent.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "ThresholdEventImplementation can only be constructed from 1D random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of ThresholdEventImplementation "
                                         << " has incorrect dimension";
  function_ = antecedent.getFunction();
  antecedent_ = antecedent.getAntecedent();
  setName(antecedent.getName());
  setDescription(antecedent.getDescription());
}


/* Constructor from RandomVector */
ThresholdEventImplementation::ThresholdEventImplementation(const RandomVector & antecedent,
    const Interval & interval)
  : CompositeRandomVector()
{
  const UnsignedInteger dimension = interval.getDimension();
  const UnsignedInteger inputDimension = antecedent.getFunction().getInputDimension();
  const Interval::BoolCollection finiteLowerBound(interval.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(interval.getFiniteUpperBound());
  const Point lowerBound(interval.getLowerBound());
  const Point upperBound(interval.getUpperBound());
  SymbolicFunction testFunction(Description::BuildDefault(inputDimension, "x"), Description(1, "0.0"));

  // easy case: 1d interval
  if (interval.getDimension() == 1)
  {
    if (finiteLowerBound[0] && !finiteUpperBound[0])
    {
      *this = ThresholdEventImplementation(antecedent, Greater(), lowerBound[0]);
    }
    if (!finiteLowerBound[0] && finiteUpperBound[0])
    {
      *this = ThresholdEventImplementation(antecedent, Less(), upperBound[0]);
    }

    if (finiteLowerBound[0] && finiteUpperBound[0])
    {
      testFunction = SymbolicFunction("x", OSS() << "min(x-(" << lowerBound[0] << "), (" << upperBound[0] << ") - x)");
      CompositeRandomVector newVector(ComposedFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = ThresholdEventImplementation(newVector, Greater(), 0.0);
    }
    if (!finiteLowerBound[0] && !finiteUpperBound[0])
    {
      CompositeRandomVector newVector(Function(testFunction), antecedent.getAntecedent());
      *this = ThresholdEventImplementation(newVector, Less(), 1.0);
    }
  }
  // general case
  else
  {
    const Description inVars(Description::BuildDefault(dimension, "y"));
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
      const CompositeRandomVector newVector(Function(testFunction), antecedent.getAntecedent());
      *this = ThresholdEventImplementation(newVector, Less(), 1.0);
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
      const CompositeRandomVector newVector(ComposedFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = ThresholdEventImplementation(newVector, Greater(), 0.0);
    }
  }
}


ThresholdEventImplementation * ThresholdEventImplementation::clone() const
{
  return new ThresholdEventImplementation(*this);
}

/* String converter */
String ThresholdEventImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << ThresholdEventImplementation::GetClassName()
      << " antecedent=" << CompositeRandomVector::__repr__()
      << " operator=" << operator_
      << " threshold=" << threshold_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger ThresholdEventImplementation::getDimension() const
{
  return 1;
}

/* Operator accessor */
ComparisonOperator ThresholdEventImplementation::getOperator() const
{
  return operator_;
}

/* Threshold accessor */
Scalar ThresholdEventImplementation::getThreshold() const
{
  return threshold_;
}


Domain ThresholdEventImplementation::getDomain() const
{
  Interval result(threshold_, threshold_);
  if (operator_(1.0, 2.0))
    result.setFiniteLowerBound(Interval::BoolCollection(1, false));
  else
    result.setFiniteUpperBound(Interval::BoolCollection(1, false));
  return result;
}

/* Realization accessor */
Point ThresholdEventImplementation::getRealization() const
{
  return Point(1, operator_(CompositeRandomVector::getRealization()[0], threshold_));
}

Point ThresholdEventImplementation::getFixedValue(const Point & fixedValue) const
{
  return Point(1, operator_(CompositeRandomVector::getFixedValue(fixedValue)[0], threshold_));
}

/* Numerical sample accessor */
Sample ThresholdEventImplementation::getSample(const UnsignedInteger size) const
{
  // We don't build the return sample element by element because it doesn't
  // use the potential distribution of the computation. As the returned
  // sample can be huge, we use it twice in place
  // First, it stores a sample of its antecedent
  Sample returnSample(CompositeRandomVector::getSample(size));
  // Then, we loop over the sample and substitute realizations of the eventRandomVectorImplementation
  // in place of the realizations of the antecedent
  for (UnsignedInteger i = 0; i < size; ++i) returnSample(i, 0) = operator_(returnSample(i, 0), threshold_);
  returnSample.setName("ThresholdEventImplementation sample");
  returnSample.setDescription(getDescription());
  return returnSample;
}

Point ThresholdEventImplementation::getParameter() const
{
  Point parameter(CompositeRandomVector::getParameter());
  parameter.add(threshold_);
  return parameter;
}

void ThresholdEventImplementation::setParameter(const Point & parameter)
{
  const UnsignedInteger compositeParameterDimension = CompositeRandomVector::getParameter().getDimension();
  if (parameter.getDimension() != (compositeParameterDimension + 1))
    throw InvalidArgumentException(HERE) << "Wrong event random vector parameter size";
  Point compositeParameter(compositeParameterDimension);
  std::copy(parameter.begin(), parameter.begin() + compositeParameterDimension, compositeParameter.begin());
  CompositeRandomVector::setParameter(compositeParameter);
  threshold_ = parameter[compositeParameterDimension];
}

Description ThresholdEventImplementation::getParameterDescription() const
{
  Description description(CompositeRandomVector::getParameterDescription());
  description.add("threshold");
  return description;
}

Bool ThresholdEventImplementation::isEvent() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void ThresholdEventImplementation::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "operator_", operator_ );
  adv.saveAttribute( "threshold_", threshold_ );
}

/* Method load() reloads the object from the StorageManager */
void ThresholdEventImplementation::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "operator_", operator_ );
  adv.loadAttribute( "threshold_", threshold_ );
}

END_NAMESPACE_OPENTURNS
