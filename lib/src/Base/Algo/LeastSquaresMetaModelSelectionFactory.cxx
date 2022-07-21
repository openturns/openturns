//                                               -*- C++ -*-
/**
 *  @brief A factory for building LeastSquaresMetaModelSelectionFactory objects
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LeastSquaresMetaModelSelectionFactory.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"
#include "openturns/BasisSequenceFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(LeastSquaresMetaModelSelectionFactory)

static const Factory<LeastSquaresMetaModelSelectionFactory> Factory_LeastSquaresMetaModelSelectionFactory;

/* Constructor */
LeastSquaresMetaModelSelectionFactory::LeastSquaresMetaModelSelectionFactory(const BasisSequenceFactory & fact,
    const FittingAlgorithm & algo)
  : ApproximationAlgorithmImplementationFactory(),
    basisSequenceFactory_(fact),
    fittingAlgorithm_(algo)
{
  // Nothing to do
}


/* Virtual constructor */
LeastSquaresMetaModelSelectionFactory * LeastSquaresMetaModelSelectionFactory::clone() const

{
  return new LeastSquaresMetaModelSelectionFactory(*this);
}


/* Accessors */
BasisSequenceFactory LeastSquaresMetaModelSelectionFactory::getBasisSequenceFactory() const
{
  return basisSequenceFactory_;
}

FittingAlgorithm LeastSquaresMetaModelSelectionFactory::getFittingAlgorithm() const
{
  return fittingAlgorithm_;
}


/* String converter */
String LeastSquaresMetaModelSelectionFactory::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

/* Method to create new LeastSquaresMetaModelSelection objects */
LeastSquaresMetaModelSelection * LeastSquaresMetaModelSelectionFactory::build(const Sample & x,
    const Sample & y,
    const Point & weight,
    const FunctionCollection & psi,
    const Indices & indices) const
{
  return new LeastSquaresMetaModelSelection( x, y, weight, psi, indices, basisSequenceFactory_, fittingAlgorithm_ );
}

/* Method save() stores the object through the StorageManager */
void LeastSquaresMetaModelSelectionFactory::save(Advocate & adv) const
{
  ApproximationAlgorithmImplementationFactory::save(adv);
  adv.saveAttribute( "basisSequenceFactory_", basisSequenceFactory_ );
  adv.saveAttribute( "fittingAlgorithm_", fittingAlgorithm_ );
}

/* Method load() reloads the object from the StorageManager */
void LeastSquaresMetaModelSelectionFactory::load(Advocate & adv)
{
  ApproximationAlgorithmImplementationFactory::load(adv);
  adv.loadAttribute( "basisSequenceFactory_", basisSequenceFactory_ );
  adv.loadAttribute( "fittingAlgorithm_", fittingAlgorithm_ );
}

END_NAMESPACE_OPENTURNS
