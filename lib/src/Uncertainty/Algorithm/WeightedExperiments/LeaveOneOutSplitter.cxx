//                                               -*- C++ -*-
/**
 *  @brief LeaveOneOutSplitter
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
#include "openturns/LeaveOneOutSplitter.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeaveOneOutSplitter)

static const Factory<LeaveOneOutSplitter> Factory_LeaveOneOutSplitter;


/* Default constructor */
LeaveOneOutSplitter::LeaveOneOutSplitter()
  : SplitterImplementation()
{
}

/* Default constructor */
LeaveOneOutSplitter::LeaveOneOutSplitter(const UnsignedInteger size)
  : SplitterImplementation(size)
{

}

/* Virtual constructor method */
LeaveOneOutSplitter * LeaveOneOutSplitter::clone() const
{
  return new LeaveOneOutSplitter(*this);
}

Indices LeaveOneOutSplitter::generate(Indices & indices2Out) const
{
  if (currentIndex_ >= N_)
    throw OutOfBoundException(HERE) << "end of LOO set";

  Indices indices1;
  indices2Out.clear();
  for (UnsignedInteger i = 0; i < N_; ++ i)
  {
    if (i != currentIndex_)
      indices1.add(i);
    else
      indices2Out.add(i);
  }
  ++ currentIndex_;
  return indices1;
}

/* Number of indices pairs accessor */
UnsignedInteger LeaveOneOutSplitter::getSize() const
{
  return N_;
}

/* String converter */
String LeaveOneOutSplitter::__repr__() const
{
  OSS oss;
  oss << "class=" << LeaveOneOutSplitter::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LeaveOneOutSplitter::save(Advocate & adv) const
{
  SplitterImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void LeaveOneOutSplitter::load(Advocate & adv)
{
  SplitterImplementation::load(adv);
}


END_NAMESPACE_OPENTURNS
