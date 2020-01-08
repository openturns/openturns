//                                               -*- C++ -*-
/**
 *  @brief The gradient part of linear combination of polynomials
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/AggregatedGradient.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedGradient)

static const Factory<AggregatedGradient> Factory_AggregatedGradient;


/* Default constructor */
AggregatedGradient::AggregatedGradient()
  : GradientImplementation()
  , p_evaluation_()
{
  // Nothing to do
}


/* Parameters constructor */
AggregatedGradient::AggregatedGradient(const AggregatedEvaluation & evaluation)
  : GradientImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
AggregatedGradient::AggregatedGradient(const Pointer<AggregatedEvaluation> & p_evaluation)
  : GradientImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
AggregatedGradient * AggregatedGradient::clone() const
{
  return new AggregatedGradient(*this);
}


/* Gradient method */
Matrix AggregatedGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size = p_evaluation_->functionsCollection_.getSize();
  Matrix result(p_evaluation_->getInputDimension(), p_evaluation_->getOutputDimension());
  UnsignedInteger columnIndex = 0;
  // Loop over the contributors
  for (UnsignedInteger contributorIndex = 0; contributorIndex < size; ++contributorIndex)
  {
    const Matrix contributorGradient(p_evaluation_->functionsCollection_[contributorIndex].gradient(inP));
    // Copy the contributor gradient into the global matrix gradient
    // Must proceed in column order
    for (UnsignedInteger j = 0; j < contributorGradient.getNbColumns(); ++j)
    {
      for (UnsignedInteger i = 0; i < contributorGradient.getNbRows(); ++i)
        result(i, columnIndex) = contributorGradient(i, j);
      ++columnIndex;
    }
  } // contributorIndex
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger AggregatedGradient::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger AggregatedGradient::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String AggregatedGradient::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " evaluation=" << *p_evaluation_;
}

String AggregatedGradient::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void AggregatedGradient::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void AggregatedGradient::load(Advocate & adv)
{
  PersistentObject::load(adv);
  TypedInterfaceObject<AggregatedEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}



END_NAMESPACE_OPENTURNS
