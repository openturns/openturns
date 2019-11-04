//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math gradient implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/FiniteDifferenceGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NoEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteDifferenceGradient)

static const Factory<FiniteDifferenceGradient> Factory_FiniteDifferenceGradient;

/* Default constructor */
FiniteDifferenceGradient::FiniteDifferenceGradient()
  : GradientImplementation()
  , evaluation_(new NoEvaluation())
{
  // Nothing to do
}

/* First Parameter constructor  */
FiniteDifferenceGradient::FiniteDifferenceGradient(
  const Point & epsilon,
  const Evaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
  , finiteDifferenceStep_(epsilon)
{
  /* Check if the dimension of the constant term is compatible with the linear and quadratic terms */
  if (epsilon.getDimension() != evaluation.getInputDimension())
    throw InvalidDimensionException(HERE) << "Epsilon dimension is incompatible with the given evaluation";

  /* Check if any epsilon component is exactly zero */
  for (UnsignedInteger i = 0; i < epsilon.getDimension(); i++)
  {
    if (epsilon[i] == 0.0)
      throw InvalidArgumentException(HERE) << "At least one of the components of epsilon is equal to 0.0, namely component " << i;
  }
}

/* SecondParameter constructor */
FiniteDifferenceGradient::FiniteDifferenceGradient(const Scalar epsilon,
    const Evaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
  , finiteDifferenceStep_(Point(evaluation.getInputDimension(), epsilon))
{
  // Check if epsilon is exactly zero
  if (epsilon == 0.0)
    throw InvalidArgumentException(HERE) << "The given scalar epsilon is equal to 0.0";
}

/*  Parameter constructor with FiniteDifferenceStep*/
FiniteDifferenceGradient::FiniteDifferenceGradient(
  const FiniteDifferenceStep & finiteDifferenceStep,
  const Evaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
  , finiteDifferenceStep_(finiteDifferenceStep)
{
  Point epsilon(getEpsilon());
  //Check if the dimension of the constant term is compatible with the linear and quadratic terms
  if (epsilon.getDimension() != evaluation.getInputDimension())
    throw InvalidDimensionException(HERE) << "Epsilon dimension is incompatible with the given evaluation";

  //Check if any epsilon component is exactly zero
  for (UnsignedInteger i = 0; i < epsilon.getDimension(); i++)
  {
    if (epsilon[i] == 0.0)
      throw InvalidArgumentException(HERE) << "At least one of the components of epsilon is equal to 0.0, namely component " << i;
  }
}

/* Virtual constructor */
FiniteDifferenceGradient * FiniteDifferenceGradient::clone() const
{
  return new FiniteDifferenceGradient(*this);
}

/* Comparison operator */
Bool FiniteDifferenceGradient::operator ==(const FiniteDifferenceGradient & other) const
{
  return (getEpsilon() == other.getEpsilon());
}

/* String converter */
String FiniteDifferenceGradient:: __repr__() const
{
  OSS oss;
  oss << "class=" << FiniteDifferenceGradient::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_. __repr__();
  return oss;
}

/* Accessor for epsilon */
Point FiniteDifferenceGradient::getEpsilon() const
{
  return finiteDifferenceStep_.getEpsilon();
}

/* Accessor for the evaluation */
Evaluation FiniteDifferenceGradient::getEvaluation() const
{
  return evaluation_;
}

/* Accessor for input point dimension */
UnsignedInteger FiniteDifferenceGradient::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FiniteDifferenceGradient::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the finite difference step */
void FiniteDifferenceGradient::setFiniteDifferenceStep(const FiniteDifferenceStep & finiteDifferenceStep)
{
  finiteDifferenceStep_ = finiteDifferenceStep;
}

FiniteDifferenceStep FiniteDifferenceGradient::getFiniteDifferenceStep() const
{
  return finiteDifferenceStep_;
}

/* Evaluation method */
Matrix FiniteDifferenceGradient::gradient(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In FiniteDifferenceGradient::gradient(const Point & inP) const";
}

/* Method save() stores the object through the StorageManager */
void FiniteDifferenceGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
  adv.saveAttribute( "finiteDifferenceStep_", finiteDifferenceStep_ );
}

/* Method load() reloads the object from the StorageManager */
void FiniteDifferenceGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
  adv.loadAttribute( "finiteDifferenceStep_", finiteDifferenceStep_ );
}


END_NAMESPACE_OPENTURNS
