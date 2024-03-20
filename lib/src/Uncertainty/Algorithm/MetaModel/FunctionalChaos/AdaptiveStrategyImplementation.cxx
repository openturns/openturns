//                                               -*- C++ -*-
/**
 *  @brief This is a abstract class for adaptive strategy implementations
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/AdaptiveStrategyImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(AdaptiveStrategyImplementation)

static const Factory<AdaptiveStrategyImplementation> Factory_AdaptiveStrategyImplementation;


/* Default constructor */
AdaptiveStrategyImplementation::AdaptiveStrategyImplementation()
  : PersistentObject(),
    basis_(),
    maximumDimension_(0),
    I_p_(0),
    addedPsi_k_ranks_(0),
    conservedPsi_k_ranks_(0),
    removedPsi_k_ranks_(0),
    Psi_k_p_(0)
{
  // Nothing to do
}

/* Constructor from an orthogonal basis */
AdaptiveStrategyImplementation::AdaptiveStrategyImplementation(const OrthogonalBasis & basis,
    const UnsignedInteger maximumDimension)
  : PersistentObject(),
    basis_(basis),
    maximumDimension_(maximumDimension),
    I_p_(0),
    addedPsi_k_ranks_(0),
    conservedPsi_k_ranks_(0),
    removedPsi_k_ranks_(0),
    Psi_k_p_(0)
{
  // Nothing to do
}


/* Virtual constructor */
AdaptiveStrategyImplementation * AdaptiveStrategyImplementation::clone() const
{
  return new AdaptiveStrategyImplementation(*this);
}


/* String converter */
String AdaptiveStrategyImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " maximumDimension=" << maximumDimension_;
}


/* Basis accessor */
OrthogonalBasis AdaptiveStrategyImplementation::getBasis() const
{
  return basis_;
}


/* Maximum dimension accessor */
void AdaptiveStrategyImplementation::setMaximumDimension(const UnsignedInteger maximumDimension)
{
  maximumDimension_ = maximumDimension;
}

UnsignedInteger AdaptiveStrategyImplementation::getMaximumDimension() const
{
  return maximumDimension_;
}


/* Compute initial basis for the approximation */
void AdaptiveStrategyImplementation::computeInitialBasis()
{
  throw NotYetImplementedException(HERE) << "In AdaptiveStrategyImplementation::computeInitialBasis()";
}

/* Update the basis for the next iteration of approximation */
void AdaptiveStrategyImplementation::updateBasis(const Point &,
    const Scalar,
    const Scalar )
{
  throw NotYetImplementedException(HERE) << "In AdaptiveStrategyImplementation::updateBasis(const Point & alpha_k, const Scalar residual, const Scalar relativeError)";
}

/* Psi accessor */
AdaptiveStrategyImplementation::FunctionCollection AdaptiveStrategyImplementation::getPsi() const
{
  return Psi_k_p_;
}

/* Method save() stores the object through the StorageManager */
void AdaptiveStrategyImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "basis_", basis_ );
}


/* Method load() reloads the object from the StorageManager */
void AdaptiveStrategyImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "basis_", basis_ );
}



END_NAMESPACE_OPENTURNS
