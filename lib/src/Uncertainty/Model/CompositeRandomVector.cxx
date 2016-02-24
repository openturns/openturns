//                                               -*- C++ -*-
/**
 *  @brief An implementation class for Composite random vectors
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/CompositeRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CompositeRandomVector);

static const Factory<CompositeRandomVector> RegisteredFactory;

/* Standard constructor */
CompositeRandomVector::CompositeRandomVector()
  : RandomVectorImplementation()
  , function_()
  , p_antecedent_()
{
  // Nothing to do
}

/* Standard constructor */
CompositeRandomVector::CompositeRandomVector(const NumericalMathFunction & function,
    const Antecedent & p_antecedent)
  : RandomVectorImplementation()
  , function_(function)
  , p_antecedent_(p_antecedent)
{
  if (function.getInputDimension() != p_antecedent->getDimension()) throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeRandomVector from a RandomVector and a NumericalMathFunction with incompatible dimensions, here RandomVector dimension=" << p_antecedent->getDimension() << " and NumericalMathFunction input dimension=" << function.getInputDimension();
  // Get the description from the underlying function
  setDescription(function.getOutputDescription());
}

/* Standard constructor */
CompositeRandomVector::CompositeRandomVector(const NumericalMathFunction & function,
    const RandomVector & antecedent)
  : RandomVectorImplementation()
  , function_(function)
  , p_antecedent_(antecedent.getImplementation())
{
  if (function.getInputDimension() != p_antecedent_->getDimension()) throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeRandomVector from a RandomVector and a NumericalMathFunction with incompatible dimensions, here RandomVector dimension=" << p_antecedent_->getDimension() << " and NumericalMathFunction input dimension=" << function.getInputDimension();
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
      << " antecedent=" << (p_antecedent_ ? p_antecedent_->__repr__() : "");
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
NumericalPoint CompositeRandomVector::getRealization() const
{
  return function_(p_antecedent_->getRealization());
}

/* Numerical sample accessor */
NumericalSample CompositeRandomVector::getSample(const UnsignedInteger size) const
{
  NumericalSample sample(function_(p_antecedent_->getSample(size)));
  const Description description(getDescription());
  // It may append that the description has been overloaded by a child class
  // FIXME: change this ugly hack to something reasonable
  if (description.getSize() == sample.getDimension()) sample.setDescription(description);
  else sample.setDescription(function_.getOutputDescription());
  return sample;
}


/* Mean accessor */
NumericalPoint CompositeRandomVector::getMean() const
{
  /* To be implemented by a suitable algorithm, given by the user.
     It could be the e.g. quadratic cumul algorithm */
  throw NotYetImplementedException(HERE) << "In CompositeRandomVector::getMean() const";
}

/* Covariance accessor */
CovarianceMatrix CompositeRandomVector::getCovariance() const
{
  /* To be implemented by a suitable algorithm, given by the user.
     It could be the e.g. quadratic cumul algorithm */
  throw NotYetImplementedException(HERE) << "In CompositeRandomVector::getCovariance() const";
}

/* Get the random vector corresponding to the i-th marginal component */
CompositeRandomVector::Implementation CompositeRandomVector::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal random vector must be in the range [0, dim-1]";
  return new CompositeRandomVector(function_.getMarginal(i), p_antecedent_);
}

/* Get the marginal random vector corresponding to indices components */
CompositeRandomVector::Implementation CompositeRandomVector::getMarginal(const Indices & indices) const
{
  if (!indices.check(getDimension() - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal random vector must be in the range [0, dim-1] and  must be different";
  return new CompositeRandomVector(function_.getMarginal(indices), p_antecedent_);
}

/* Antecedent accessor */
CompositeRandomVector::Antecedent CompositeRandomVector::getAntecedent() const
{
  return p_antecedent_;
}

/* Function accessor */
NumericalMathFunction CompositeRandomVector::getFunction() const
{
  return function_;
}

/* Distribution accessor */
Distribution CompositeRandomVector::getDistribution() const
{
  throw NotYetImplementedException(HERE) << "In CompositeRandomVector::getDistribution() const";
}

/* Method save() stores the object through the StorageManager */
void CompositeRandomVector::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "antecedent_", *p_antecedent_ );
}

/* Method load() reloads the object from the StorageManager */
void CompositeRandomVector::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
  TypedInterfaceObject<RandomVectorImplementation> antecedent;
  adv.loadAttribute( "antecedent_", antecedent );
  p_antecedent_ = antecedent.getImplementation();
}

END_NAMESPACE_OPENTURNS
