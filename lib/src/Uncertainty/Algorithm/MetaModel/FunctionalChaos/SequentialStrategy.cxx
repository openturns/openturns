//                                               -*- C++ -*-
/**
 *  @brief An implementation returning the set of polynomials in sequence
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
#include "openturns/SequentialStrategy.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SequentialStrategy)

static const Factory<SequentialStrategy> Factory_SequentialStrategy;


/* Default constructor */
SequentialStrategy::SequentialStrategy()
  : AdaptiveStrategyImplementation(),
    currentVectorIndex_(0)
{
  // Nothing to do
}


/* Constructor from an orthogonal basis */
SequentialStrategy::SequentialStrategy(const OrthogonalBasis & basis,
                                       const UnsignedInteger maximumDimension,
                                       const Bool verbose)
  : AdaptiveStrategyImplementation(basis, maximumDimension),
    currentVectorIndex_(0),
    verbose_(verbose)
{
  LOGWARN(OSS() << "SequentialStrategy is deprecated");
}


/* Compute initial basis for the approximation */
void SequentialStrategy::computeInitialBasis()
{
  // Start with the first vector
  currentVectorIndex_ = 0;
  Psi_k_p_ = FunctionCollection(1, basis_.build(currentVectorIndex_));
  Psi_ = Psi_k_p_;
  I_p_ = Indices(1, currentVectorIndex_);
  addedPsi_k_ranks_ = Indices(1, currentVectorIndex_);
  removedPsi_k_ranks_ = Indices(0);
  conservedPsi_k_ranks_ = Indices(0);
  ++currentVectorIndex_;
}

/* Update the basis for the next iteration of approximation */
void SequentialStrategy::updateBasis(const Point & alpha_k,
                                     const Scalar,
                                     const Scalar )
{
  if (verbose_)
  {
    LOGINFO(OSS() << "initial state:");
    LOGINFO(OSS() << "  vector  index=" << currentVectorIndex_);
    LOGINFO(OSS() << "  coeffs   size=" << alpha_k.getSize());
    LOGINFO(OSS() << "  coeffs       =" << alpha_k);
    LOGINFO(OSS() << "  I_p      size=" << I_p_.getSize());
    LOGINFO(OSS() << "  I_p          =" << I_p_);
  }
  // If we have not generated all the vectors, go to the next one
  // All the previous vectors are kept
  conservedPsi_k_ranks_ = Indices(I_p_.getSize());
  conservedPsi_k_ranks_.fill();
  if (currentVectorIndex_ < maximumDimension_)
  {
    const Function newVector(basis_.build(currentVectorIndex_));
    Psi_k_p_.add(newVector);
    Psi_.add(newVector);
    I_p_.add(currentVectorIndex_);
    addedPsi_k_ranks_ = Indices(1, currentVectorIndex_);
    ++currentVectorIndex_;
  }
  else
  {
    addedPsi_k_ranks_   = Indices(0);
  }
  // No vector is removed
  removedPsi_k_ranks_ = Indices(0);
  if (verbose_)
  {
    LOGINFO(OSS() << "final state:");
    LOGINFO(OSS() << "  vector  index=" << currentVectorIndex_);
    LOGINFO(OSS() << "  coeffs   size=" << alpha_k.getSize());
    LOGINFO(OSS() << "  coeffs       =" << alpha_k);
    LOGINFO(OSS() << "  I_p      size=" << I_p_.getSize());
    LOGINFO(OSS() << "  I_p          =" << I_p_);
  }
}


/* Virtual constructor */
SequentialStrategy * SequentialStrategy::clone() const
{
  return new SequentialStrategy(*this);
}


/* String converter */
String SequentialStrategy::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " derived from " << AdaptiveStrategyImplementation::__repr__();
}


/* Method save() stores the object through the StorageManager */
void SequentialStrategy::save(Advocate & adv) const
{
  AdaptiveStrategyImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void SequentialStrategy::load(Advocate & adv)
{
  AdaptiveStrategyImplementation::load(adv);
}



END_NAMESPACE_OPENTURNS
