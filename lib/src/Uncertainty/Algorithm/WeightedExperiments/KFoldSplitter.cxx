//                                               -*- C++ -*-
/**
 *  @brief KFoldSplitter
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
  setRandomize(ResourceMap::GetAsBool("KFoldSplitter-Randomize"));
}

/* Virtual constructor method */
KFoldSplitter * KFoldSplitter::clone() const
{
  return new KFoldSplitter(*this);
}

Indices KFoldSplitter::generate(Indices & indices2Out) const
{
  if (currentIndex_ >= k_)
    throw OutOfBoundException(HERE) << "end of KFold set";

  Indices indices1;
  indices2Out.clear();
  for (UnsignedInteger i = 0; i < N_; ++ i)
  {
    const UnsignedInteger si = shuffle_.getSize() ? shuffle_[i] : i;
    if (i % k_ != currentIndex_)
      indices1.add(si);
    else
      indices2Out.add(si);
  }
  ++ currentIndex_;
  return indices1;
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
