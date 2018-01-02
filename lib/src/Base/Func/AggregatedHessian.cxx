//                                               -*- C++ -*-
/**
 *  @brief The hessian part of linear combination of polynomials
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
 *  You should have received a copy of the GNU Lesser General Public
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
  , evaluation_()
{
  // Nothing to do
}


/* Parameters constructor */
AggregatedHessian::AggregatedHessian(const AggregatedEvaluation & evaluation)
  : HessianImplementation()
  , evaluation_(evaluation)
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
  const UnsignedInteger size = evaluation_.functionsCollection_.getSize();
  SymmetricTensor result(evaluation_.getInputDimension(), evaluation_.getOutputDimension());
  UnsignedInteger sheetIndex = 0;
  // Loop over the contributors
  for (UnsignedInteger contributorIndex = 0; contributorIndex < size; ++contributorIndex)
  {
    const SymmetricTensor contributorHessian(evaluation_.functionsCollection_[contributorIndex].hessian(inP));
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
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger AggregatedHessian::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* String converter */
String AggregatedHessian::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " evaluation=" << evaluation_;
}

String AggregatedHessian::__str__(const String & offset) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void AggregatedHessian::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void AggregatedHessian::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}



END_NAMESPACE_OPENTURNS
