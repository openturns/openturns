//                                               -*- C++ -*-
/**
 *  @brief An implementation returning the set of polynomials in sequence
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include <cstdlib>

#include "CleaningStrategy.hxx"
#include "Log.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"
#include "NumericalSample.hxx"
#include "Collection.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CleaningStrategy);

typedef Collection<NumericalScalar> NumericalScalarCollection;

static const Factory<CleaningStrategy> RegisteredFactory;


/* Default constructor */
CleaningStrategy::CleaningStrategy()
  : AdaptiveStrategyImplementation()
  , currentVectorIndex_(0)
  , maximumSize_(0)
  , significanceFactor_(0.0)
  , verbose_(false)
{
  // Nothing to do
}


/* Constructor from an orthogonal basis */
CleaningStrategy::CleaningStrategy(const OrthogonalBasis & basis,
                                   const UnsignedInteger maximumDimension,
                                   const Bool verbose)
  : AdaptiveStrategyImplementation(basis, maximumDimension)
  , currentVectorIndex_(0)
  , maximumSize_(ResourceMap::GetAsUnsignedInteger( "CleaningStrategy-DefaultMaximumSize" ))
  , significanceFactor_(ResourceMap::GetAsNumericalScalar( "CleaningStrategy-DefaultSignificanceFactor" ))
  , verbose_(verbose)
{
  // Nothing to do
}


/* Constructor from an orthogonal basis */
CleaningStrategy::CleaningStrategy(const OrthogonalBasis & basis,
                                   const UnsignedInteger maximumDimension,
                                   const UnsignedInteger maximumSize,
                                   const NumericalScalar significanceFactor,
                                   const Bool verbose)
  : AdaptiveStrategyImplementation(basis, maximumDimension)
  , currentVectorIndex_(0)
  , maximumSize_(maximumSize)
  , significanceFactor_(significanceFactor)
  , verbose_(verbose)
{
  // Nothing to do
}


/* Compute initial basis for the approximation */
void CleaningStrategy::computeInitialBasis()
{
  // Start with the first set of vectors
  const UnsignedInteger size(std::min(maximumSize_, maximumDimension_));
  Psi_k_p_ = NumericalMathFunctionCollection(size);
  I_p_ = Indices(size);
  I_p_.fill();
  for (UnsignedInteger i = 0; i < size; ++i) Psi_k_p_[i] = basis_.build(i);
  addedPsi_k_ranks_ = Indices(I_p_.getSize());
  addedPsi_k_ranks_.fill();
  removedPsi_k_ranks_ = Indices(0);
  conservedPsi_k_ranks_ = Indices(0);
  currentVectorIndex_ = size;
}

/* Update the basis for the next iteration of approximation */
void CleaningStrategy::updateBasis(const NumericalPoint & alpha_k,
                                   const NumericalScalar residual,
                                   const NumericalScalar relativeError)
{
  // The dimension will be adapted, so it is not const
  UnsignedInteger dimension(alpha_k.getSize());
  NumericalScalarCollection coefficients(alpha_k.getCollection());
  if (verbose_)
  {
    LOGINFO(OSS() << "initial state:");
    LOGINFO(OSS() << "  vector index=" << currentVectorIndex_);
    LOGINFO(OSS() << "  coeffs  size=" << coefficients.getSize());
    LOGINFO(OSS() << "  coeffs      =" << coefficients);
    LOGINFO(OSS() << "  I_p     size=" << I_p_.getSize());
    LOGINFO(OSS() << "  I_p         =" << I_p_);
  }
  removedPsi_k_ranks_ = Indices(0);
  conservedPsi_k_ranks_ = Indices(I_p_.getSize());
  conservedPsi_k_ranks_.fill();
  NumericalScalarCollection removedCoefficients(0);

  // We do the cleaning based on a variance criterion, so we must exclude the first coefficient from the cleaning as it is only related to the mean. As a result, there is nothing to do if we have just one coefficient.
  if (dimension > 1)
  {
    // We keep at most maximumSize_ elements, the ones that have the largest magnitude and have a magnitude larger or equal to maximumMagnitude * significanceFactor
    // First, find the extrem magnitudes.
    NumericalScalar largest(fabs(coefficients[1]));
    NumericalScalar smallest(largest);
    NumericalScalar secondSmallest(smallest);
    UnsignedInteger rankSmallest(1);
    for (UnsignedInteger i = 2; i < dimension; ++i)
    {
      const NumericalScalar tmp(fabs(coefficients[i]));
      if (tmp > largest) largest = tmp;
      if (tmp < smallest)
      {
        secondSmallest = smallest;
        smallest = tmp;
        rankSmallest = i;
      }
    } // Search for the extrems
    // Second, if the coefficient list is too large (it can be by only one term), remove the smallest term to free a place for the next vector.
    UnsignedInteger shift(0);
    if ((dimension > maximumSize_) && (currentVectorIndex_ < maximumDimension_))
    {
      // Add the smallest element to the removed list
      removedPsi_k_ranks_.add(rankSmallest);

      if (verbose_) removedCoefficients.add(coefficients[rankSmallest]);
      // Compact Psi_k_p_ and I_p_
      Psi_k_p_.erase(Psi_k_p_.begin() + rankSmallest);
      I_p_.erase(I_p_.begin() + rankSmallest);
      coefficients.erase(coefficients.begin() + rankSmallest);
      if (verbose_)
      {
        LOGINFO(OSS() << "intermediate state:");
        LOGINFO(OSS() << "  coeffs  size=" << coefficients.getSize());
        LOGINFO(OSS() << "  coeffs      =" << coefficients);
        LOGINFO(OSS() << "  rem coeffs  =" << removedCoefficients);
        LOGINFO(OSS() << "  I_p     size=" << I_p_.getSize());
        LOGINFO(OSS() << "  I_p         =" << I_p_);
      }
      // The smallest remaining element is now the second smallest one
      smallest = secondSmallest;
      dimension = maximumSize_;
      shift = 1;
    } // Too much elements
    // Now, Psi_k_p_ contains at most maximumSize_ elements, associated with the coefficients the largest in magnitude
    // Third, remove all the elements with a magnitude less than largest * significanceFactor_
    // Quick rejection test: nothing to do if smallest >= largest * significanceFactor_
    if (smallest < largest * significanceFactor_)
    {
      UnsignedInteger currentIndex(0);
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        if (fabs(coefficients[i]) >= largest * significanceFactor_)
        {
          Psi_k_p_[currentIndex] = Psi_k_p_[i];
          I_p_[currentIndex] = I_p_[i];
          if (verbose_) coefficients[currentIndex] = coefficients[i];
          ++currentIndex;
        } // Keep the current vector
        else
        {
          // Remove the vector
          // As the removedPsi_k_ranks_ stores the indices of the removed vectors in the *previous* partial basis Psi_k_p_,
          // we must keep track to any shift in the indices
          // If there is no shift, use directly the current counter
          if (shift == 0) removedPsi_k_ranks_.add(i);
          // else we must take the shift into account
          else
          {
            // If we are stricly before the index of the smallest coefficient, no shift
            if (i < rankSmallest) removedPsi_k_ranks_.add(i);
            // Else take the shift into account
            else removedPsi_k_ranks_.add(i + shift);
          }
          if (verbose_) removedCoefficients.add(coefficients[i]);
        } // Remove the vector
      } // Loop over the coefficients
      Psi_k_p_.resize(currentIndex);
      I_p_.resize(currentIndex);
      if (verbose_) coefficients.resize(currentIndex);
    } // Cleaning step
    // At this step, I_p_ stores all the indices that are common between the previous partial basis and the one being built
    // Remove the ranks of the deleted vectors from the list of conserved vectors
    // First, sort the ranks of the deleted vectors in ascending order
    std::sort(removedPsi_k_ranks_.begin(), removedPsi_k_ranks_.end());
    // Second, remove them from end to start to avoid shift in the conservedPsi_k_ranks_ values
    for (UnsignedInteger i = removedPsi_k_ranks_.getSize(); i > 0; --i) conservedPsi_k_ranks_.erase(conservedPsi_k_ranks_.begin() + removedPsi_k_ranks_[i - 1]);
  } // If dimension > 1
  // If we have not generated all the vectors, go to the next one
  if (currentVectorIndex_ < maximumDimension_)
  {
    const NumericalMathFunction newVector(basis_.build(currentVectorIndex_));
    Psi_k_p_.add(newVector);
    I_p_.add(currentVectorIndex_);
    addedPsi_k_ranks_ = Indices(1, I_p_.getSize() - 1);
    ++currentVectorIndex_;
  } // Another vector has been added
  else
  {
    addedPsi_k_ranks_ = Indices(0);
  } // No more vector to add
  if (verbose_)
  {
    LOGINFO(OSS() << "final state:");
    LOGINFO(OSS() << "  vector index=" << currentVectorIndex_ << " / " << maximumDimension_ << " (" << 0.1 * int((1000.0 * currentVectorIndex_) / maximumDimension_) << "%)");
    LOGINFO(OSS() << "  coeffs  size=" << coefficients.getSize());
    LOGINFO(OSS() << "  coeffs      =" << coefficients);
    LOGINFO(OSS() << "  rem coeffs  =" << removedCoefficients);
    LOGINFO(OSS() << "  I_p     size=" << I_p_.getSize());
    LOGINFO(OSS() << "  I_p         =" << I_p_);
  }
}


/* Virtual constructor */
CleaningStrategy * CleaningStrategy::clone() const
{
  return new CleaningStrategy(*this);
}


/* String converter */
String CleaningStrategy::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " maximum size=" << maximumSize_
         << " significance factor=" << significanceFactor_
         << " derived from " << AdaptiveStrategyImplementation::__repr__();
}


/* Current vector index accessor */
UnsignedInteger CleaningStrategy::getCurrentVectorIndex() const
{
  return currentVectorIndex_;
}

/* Maximum size accessor */
UnsignedInteger CleaningStrategy::getMaximumSize() const
{
  return maximumSize_;
}

void CleaningStrategy::setMaximumSize(const UnsignedInteger maximumSize)
{
  maximumSize_ = maximumSize;
}

/* Significance factor */
NumericalScalar CleaningStrategy::getSignificanceFactor() const
{
  return significanceFactor_;
}

void CleaningStrategy::setSignificanceFactor(const NumericalScalar significanceFactor)
{
  significanceFactor_ = significanceFactor;
}

/* Verbosity accessor */
Bool CleaningStrategy::getVerbose() const
{
  return verbose_;
}

void CleaningStrategy::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}


/* Method save() stores the object through the StorageManager */
void CleaningStrategy::save(Advocate & adv) const
{
  AdaptiveStrategyImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void CleaningStrategy::load(Advocate & adv)
{
  AdaptiveStrategyImplementation::load(adv);
}



END_NAMESPACE_OPENTURNS
