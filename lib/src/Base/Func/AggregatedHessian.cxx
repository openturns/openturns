//                                               -*- C++ -*-
/**
 *  @brief The hessian part of linear combination of polynomials
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
#include "openturns/AggregatedHessian.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedHessian)

static const Factory<AggregatedHessian> Factory_AggregatedHessian;


/* Default constructor */
AggregatedHessian::AggregatedHessian()
  : HessianImplementation()
  , p_evaluation_()
{
  // Nothing to do
}


/* Parameters constructor */
AggregatedHessian::AggregatedHessian(const AggregatedEvaluation & evaluation)
  : HessianImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
AggregatedHessian::AggregatedHessian(const Pointer<AggregatedEvaluation> & p_evaluation)
  : HessianImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
AggregatedHessian * AggregatedHessian::clone() const
{
  return new AggregatedHessian(*this);
}


/* Hessian method */
SymmetricTensor AggregatedHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size = p_evaluation_->functionsCollection_.getSize();
  SymmetricTensor result(p_evaluation_->getInputDimension(), p_evaluation_->getOutputDimension());
  UnsignedInteger sheetIndex = 0;
  // Loop over the contributors
  for (UnsignedInteger contributorIndex = 0; contributorIndex < size; ++contributorIndex)
  {
    const SymmetricTensor contributorHessian(p_evaluation_->functionsCollection_[contributorIndex].hessian(inP));
    // Copy the contributor hessian into the global tensor hessian
    for (UnsignedInteger k = 0; k < contributorHessian.getNbSheets(); ++k)
    {
      for (UnsignedInteger j = 0; j < contributorHessian.getNbColumns(); ++j)
        for (UnsignedInteger i = j; i < contributorHessian.getNbRows(); ++i)
          result(i, j, sheetIndex) = contributorHessian(i, j, k);
      ++sheetIndex;
    }
  } // contributorIndex
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger AggregatedHessian::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger AggregatedHessian::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String AggregatedHessian::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " evaluation=" << *p_evaluation_;
}

String AggregatedHessian::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void AggregatedHessian::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void AggregatedHessian::load(Advocate & adv)
{
  PersistentObject::load(adv);
  TypedInterfaceObject<AggregatedEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}



END_NAMESPACE_OPENTURNS
