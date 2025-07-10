//                                               -*- C++ -*-
/**
 *  @brief KFoldSplitter
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/KFoldSplitter.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KFoldSplitter)

static const Factory<KFoldSplitter> Factory_KFoldSplitter;

/* Default constructor */
KFoldSplitter::KFoldSplitter()
  : SplitterImplementation()
{
}

/* Default constructor */
KFoldSplitter::KFoldSplitter(const UnsignedInteger size, const UnsignedInteger k)
  : SplitterImplementation(size)
  , k_(k)
{
  if (k > size)
    throw InvalidArgumentException(HERE) << "The number of folds (" << k << ") cannot be greater than the sample size (" << size << ")";
  setRandomize(ResourceMap::GetAsBool("KFoldSplitter-Randomize"));
}

/* Virtual constructor method */
KFoldSplitter * KFoldSplitter::clone() const
{
  return new KFoldSplitter(*this);
}

Indices KFoldSplitter::generate(Indices & indicesTest) const
{
  if (currentIndex_ >= k_)
    throw OutOfBoundException(HERE) << "end of KFold set";

  const UnsignedInteger foldSize = (currentIndex_ >= N_ % k_) ? N_ / k_ : N_ / k_ + 1;

  // test indices
  indicesTest.resize(foldSize);
  indicesTest.fill(start_);

  // train indices
  Indices indicesTrain(start_);
  indicesTrain.fill();
  if (currentIndex_ + 1 < k_)
  {
    Indices after(N_ - foldSize - start_);
    after.fill(start_ + foldSize);
    indicesTrain.add(after);
  }

  // take shuffle into account
  if (shuffle_.getSize())
  {
    for (UnsignedInteger i = 0; i < indicesTest.getSize(); ++ i)
      indicesTest[i] = shuffle_[indicesTest[i]];
    for (UnsignedInteger i = 0; i < indicesTrain.getSize(); ++ i)
      indicesTrain[i] = shuffle_[indicesTrain[i]];
  }

  start_ += foldSize;
  ++ currentIndex_;
  return indicesTrain;
}

/* Reset sequence state */
void KFoldSplitter::reset() const
{
  SplitterImplementation::reset();
  start_ = 0;
}

void KFoldSplitter::setRandomize(const Bool randomize)
{
  if (randomize)
  {
    const KPermutationsDistribution permutationDistribution(N_, N_);
    const Point shuffleP(permutationDistribution.getRealization());
    shuffle_ = Indices(N_);
    for (UnsignedInteger i = 0; i < N_; ++ i)
      shuffle_[i] = shuffleP[i];
  }
  else
    shuffle_.clear();
}

/* Number of indices pairs accessor */
UnsignedInteger KFoldSplitter::getSize() const
{
  return k_;
}

/* String converter */
String KFoldSplitter::__repr__() const
{
  OSS oss;
  oss << "class=" << KFoldSplitter::GetClassName()
      << " k=" << k_
      << " randomize=" << (shuffle_.getSize() > 0);
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KFoldSplitter::save(Advocate & adv) const
{
  SplitterImplementation::save(adv);
  adv.saveAttribute("k_", k_);
  adv.saveAttribute("shuffle_", shuffle_);
}

/* Method load() reloads the object from the StorageManager */
void KFoldSplitter::load(Advocate & adv)
{
  SplitterImplementation::load(adv);
  adv.loadAttribute("k_", k_);
  adv.loadAttribute("shuffle_", shuffle_);
}


END_NAMESPACE_OPENTURNS
