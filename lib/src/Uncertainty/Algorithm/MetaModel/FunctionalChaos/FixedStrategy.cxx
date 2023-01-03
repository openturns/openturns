//                                               -*- C++ -*-
/**
 *  @brief An implementation directly returning the full set of polynomials
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
#include "openturns/FixedStrategy.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FixedStrategy)

static const Factory<FixedStrategy> Factory_FixedStrategy;

/* Default constructor */
FixedStrategy::FixedStrategy()
  : AdaptiveStrategyImplementation()
{
  // Nothing to do
}


/* Constructor from an orthogonal basis */
FixedStrategy::FixedStrategy(const OrthogonalBasis & basis,
                             const UnsignedInteger maximumDimension)
  : AdaptiveStrategyImplementation(basis, maximumDimension)
{
  // Nothing to do
}


/* Compute initial basis for the approximation */
void FixedStrategy::computeInitialBasis()
{
  if (Psi_k_p_.getSize() == 0)
  {
    Psi_k_p_ = FunctionCollection(maximumDimension_);
    for (UnsignedInteger i = 0; i < maximumDimension_; ++i)
    {
      LOGINFO(OSS() << "Build function " << i << " over " << maximumDimension_ - 1);
      Psi_k_p_[i] = basis_.build(i);
    }
  }
  Psi_ = Psi_k_p_;
  I_p_ = Indices(maximumDimension_);
  I_p_.fill();
  addedPsi_k_ranks_ = Indices(maximumDimension_);
  addedPsi_k_ranks_.fill();
  removedPsi_k_ranks_ = Indices(0);
  conservedPsi_k_ranks_ = Indices(0);
}

/* Update the basis for the next iteration of approximation */
void FixedStrategy::updateBasis(const Point &,
                                const Scalar,
                                const Scalar )
{
  // No change to the basis in the fixed strategy
  addedPsi_k_ranks_ = Indices(0);
  removedPsi_k_ranks_ = Indices(0);
  conservedPsi_k_ranks_ = Indices(I_p_.getSize());
  conservedPsi_k_ranks_.fill();
}


/* Virtual constructor */
FixedStrategy * FixedStrategy::clone() const
{
  return new FixedStrategy(*this);
}


/* String converter */
String FixedStrategy::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " derived from " << AdaptiveStrategyImplementation::__repr__();
}


/* Method save() stores the object through the StorageManager */
void FixedStrategy::save(Advocate & adv) const
{
  AdaptiveStrategyImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void FixedStrategy::load(Advocate & adv)
{
  AdaptiveStrategyImplementation::load(adv);
}



END_NAMESPACE_OPENTURNS
