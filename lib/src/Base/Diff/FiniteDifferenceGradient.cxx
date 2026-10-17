//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a gradient implementation
 *         from an evaluation implementation by using centered
 *         finite difference formula.
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

#include "openturns/FiniteDifferenceGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NoEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteDifferenceGradient)

static const Factory<FiniteDifferenceGradient> Factory_FiniteDifferenceGradient;

static const Factory<PersistentCollection<FiniteDifferenceStep> > Factory_PersistentCollection_FiniteDifferenceStep;

/* Default constructor */
FiniteDifferenceGradient::FiniteDifferenceGradient()
  : GradientImplementation()
  , evaluation_(new NoEvaluation())
  , finiteDifferenceSteps_(1)
{
  // Nothing to do
}

/* First Parameter constructor  */
FiniteDifferenceGradient::FiniteDifferenceGradient(
  const Point & epsilon,
  const Evaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
  , finiteDifferenceSteps_(1, FiniteDifferenceStep(epsilon))
{
  /* Check if the dimension of the constant term is compatible with the linear and quadratic terms */
  if (epsilon.getDimension() != evaluation.getInputDimension())
    throw InvalidDimensionException(HERE) << "Epsilon dimension (" << epsilon.getDimension() << ") is incompatible with the given evaluation";

  /* Check if any epsilon component is exactly zero */
  for (UnsignedInteger i = 0; i < epsilon.getDimension(); i++)
  {
    if (!(epsilon[i] > 0.0 || epsilon[i] < 0.0)) // condition equivalent to epsilon[i] == 0.0, but true if epsilon[i] is NaN
      throw InvalidArgumentException(HERE) << "At least one of the components of epsilon is null, namely component " << i;
  }
}

/* SecondParameter constructor */
FiniteDifferenceGradient::FiniteDifferenceGradient(const Scalar epsilon,
    const Evaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
  , finiteDifferenceSteps_(1, FiniteDifferenceStep(Point(evaluation.getInputDimension(), epsilon)))
{
  // Check if epsilon is exactly zero
  if (!(epsilon > 0.0 || epsilon < 0.0)) // condition equivalent to epsilon == 0.0, but true if epsilon is NaN
    throw InvalidArgumentException(HERE) << "The given scalar epsilon is null.";
}

/*  Parameter constructor with FiniteDifferenceStep*/
FiniteDifferenceGradient::FiniteDifferenceGradient(
  const FiniteDifferenceStep & finiteDifferenceStep,
  const Evaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
  , finiteDifferenceSteps_(1, finiteDifferenceStep)
{
  Point epsilon(getEpsilon());
  //Check if the dimension of the constant term is compatible with the linear and quadratic terms
  if (epsilon.getDimension() != evaluation.getInputDimension())
    throw InvalidDimensionException(HERE) << "Epsilon dimension (" << epsilon.getDimension() << ") is incompatible with the given evaluation";

  //Check if any epsilon component is exactly zero
  for (UnsignedInteger i = 0; i < epsilon.getDimension(); i++)
  {
    if (!(epsilon[i] > 0.0 || epsilon[i] < 0.0)) // condition equivalent to epsilon[i] == 0.0, but true if epsilon[i] is NaN
      throw InvalidArgumentException(HERE) << "At least one of the components of epsilon is null, namely component " << i;
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
  if (this == &other) return true;
  return (getEpsilon() == other.getEpsilon());
}

Bool FiniteDifferenceGradient::equals(const GradientImplementation & other) const
{
  const FiniteDifferenceGradient * p_other = dynamic_cast<const FiniteDifferenceGradient *>(&other);
  return p_other && (*this == *p_other);
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
  if (finiteDifferenceSteps_.isEmpty()) return FiniteDifferenceStep().getEpsilon();
  return finiteDifferenceSteps_[0].getEpsilon();
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
  finiteDifferenceSteps_ = Collection<FiniteDifferenceStep>(1, finiteDifferenceStep);
}

FiniteDifferenceStep FiniteDifferenceGradient::getFiniteDifferenceStep() const
{
  if (finiteDifferenceSteps_.isEmpty()) return FiniteDifferenceStep();
  return finiteDifferenceSteps_[0];
}

void FiniteDifferenceGradient::setFiniteDifferenceStep(const UnsignedInteger outputIndex,
    const FiniteDifferenceStep & finiteDifferenceStep)
{
  const UnsignedInteger outputDimension = evaluation_.getOutputDimension();
  if (!(outputIndex < outputDimension)) throw InvalidArgumentException(HERE) << "Output index (" << outputIndex << ") is out of range, expected less than " << outputDimension;
  if (finiteDifferenceSteps_.getSize() == 1)
    finiteDifferenceSteps_ = Collection<FiniteDifferenceStep>(outputDimension, finiteDifferenceSteps_[0]);
  if (finiteDifferenceSteps_.getSize() != outputDimension) throw InvalidArgumentException(HERE) << "Invalid number of finite difference steps " << finiteDifferenceSteps_.getSize() << ", should be " << outputDimension;
  finiteDifferenceSteps_[outputIndex] = finiteDifferenceStep;
}

FiniteDifferenceStep FiniteDifferenceGradient::getFiniteDifferenceStep(const UnsignedInteger outputIndex) const
{
  const UnsignedInteger outputDimension = evaluation_.getOutputDimension();
  if (!(outputIndex < outputDimension)) throw InvalidArgumentException(HERE) << "Output index (" << outputIndex << ") is out of range, expected less than " << outputDimension;
  if (finiteDifferenceSteps_.getSize() == 1) return getFiniteDifferenceStep();
  if (!(outputIndex < finiteDifferenceSteps_.getSize())) throw InvalidArgumentException(HERE) << "Output index (" << outputIndex << ") is out of range, expected less than " << finiteDifferenceSteps_.getSize();
  return finiteDifferenceSteps_[outputIndex];
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
  const FiniteDifferenceStepPersistentCollection steps(finiteDifferenceSteps_);
  adv.saveAttribute( "finiteDifferenceSteps_", steps );
}

/* Method load() reloads the object from the StorageManager */
void FiniteDifferenceGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
  if (adv.hasAttribute("finiteDifferenceSteps_"))
  {
    FiniteDifferenceStepPersistentCollection steps;
    adv.loadAttribute( "finiteDifferenceSteps_", steps );
    finiteDifferenceSteps_ = steps;
  }
  else
  {
    // Backward compatibility with archives storing a single step
    FiniteDifferenceStep finiteDifferenceStep;
    adv.loadAttribute( "finiteDifferenceStep_", finiteDifferenceStep );
    finiteDifferenceSteps_ = Collection<FiniteDifferenceStep>(evaluation_.getOutputDimension(), finiteDifferenceStep);
  }
}


END_NAMESPACE_OPENTURNS
