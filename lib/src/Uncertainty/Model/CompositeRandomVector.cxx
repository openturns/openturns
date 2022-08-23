//                                               -*- C++ -*-
/**
 *  @brief An implementation class for Composite random vectors
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/CompositeRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CompositeRandomVector)

static const Factory<CompositeRandomVector> Factory_CompositeRandomVector;

/* Standard constructor */
CompositeRandomVector::CompositeRandomVector()
  : RandomVectorImplementation()
  , function_()
  , antecedent_()
{
  // Nothing to do
}

/* Standard constructor */
CompositeRandomVector::CompositeRandomVector(const Function & function,
    const RandomVector & antecedent)
  : RandomVectorImplementation()
  , function_(function)
  , antecedent_(antecedent)
{
  if (function.getInputDimension() != antecedent_.getDimension()) throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeRandomVector from a RandomVector and a Function with incompatible dimensions, here RandomVector dimension=" << antecedent_.getDimension() << " and Function input dimension=" << function.getInputDimension();
  // Get the description from the underlying function
  setDescription(function.getOutputDescription());
}

/* Virtual constructor */
CompositeRandomVector * CompositeRandomVector::clone() const
{
  return new CompositeRandomVector(*this);
}

/* String converter */
String CompositeRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << CompositeRandomVector::GetClassName()
      << " function=" << function_
      << " antecedent=" << (antecedent_.getImplementation().get() ? antecedent_.getImplementation()->__repr__() : "");
  return oss;
}



/* Is the underlying random vector composite ? */
Bool CompositeRandomVector::isComposite() const
{
  return true;
}



/* Here is the interface that all derived class must implement */


/* Dimension accessor */
UnsignedInteger CompositeRandomVector::getDimension() const
{
  return function_.getOutputDimension();
}

/* Realization accessor */
Point CompositeRandomVector::getRealization() const
{
  return function_(antecedent_.getRealization());
}

/* Realization accessor */
Point CompositeRandomVector::getFixedValue(const Point & fixedValue) const
{
  return function_(antecedent_.getFixedValue(fixedValue));
}

/* Numerical sample accessor */
Sample CompositeRandomVector::getSample(const UnsignedInteger size) const
{
  Sample sample(function_(antecedent_.getSample(size)));
  const Description description(getDescription());
  // It may append that the description has been overloaded by a child class
  // FIXME: change this ugly hack to something reasonable
  if (description.getSize() == sample.getDimension()) sample.setDescription(description);
  else sample.setDescription(function_.getOutputDescription());
  return sample;
}

/* Get the random vector corresponding to the i-th marginal component */
RandomVector CompositeRandomVector::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal random vector must be in the range [0, dim-1]";
  return new CompositeRandomVector(function_.getMarginal(i), antecedent_);
}

/* Get the marginal random vector corresponding to indices components */
RandomVector CompositeRandomVector::getMarginal(const Indices & indices) const
{
  if (!indices.check(getDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal random vector must be in the range [0, dim-1] and must be different";
  return new CompositeRandomVector(function_.getMarginal(indices), antecedent_);
}

/* Antecedent accessor */
RandomVector CompositeRandomVector::getAntecedent() const
{
  return antecedent_;
}

/* Function accessor */
Function CompositeRandomVector::getFunction() const
{
  return function_;
}

Point CompositeRandomVector::getParameter() const
{
  Point parameter(function_.getParameter());
  parameter.add(antecedent_.getParameter());
  return parameter;
}

void CompositeRandomVector::setParameter(const Point & parameter)
{
  const UnsignedInteger functionParameterDimension = function_.getParameter().getDimension();
  const UnsignedInteger antecedentParameterDimension = antecedent_.getParameter().getDimension();
  if (parameter.getDimension() != (functionParameterDimension + antecedentParameterDimension))
    throw InvalidArgumentException(HERE) << "Wrong composite random vector parameter size";
  Point functionParameter(functionParameterDimension);
  std::copy(parameter.begin(), parameter.begin() + functionParameterDimension, functionParameter.begin());
  function_.setParameter(functionParameter);
  Point antecedentParameter(antecedentParameterDimension);
  std::copy(parameter.begin() + functionParameterDimension, parameter.end(), antecedentParameter.begin());
  antecedent_.setParameter(antecedentParameter);
}

Description CompositeRandomVector::getParameterDescription() const
{
  Description description(function_.getParameterDescription());
  description.add(antecedent_.getParameterDescription());
  return description;
}

/* Method save() stores the object through the StorageManager */
void CompositeRandomVector::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "antecedent_", antecedent_ );
}

/* Method load() reloads the object from the StorageManager */
void CompositeRandomVector::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "antecedent_", antecedent_ );
}

END_NAMESPACE_OPENTURNS
