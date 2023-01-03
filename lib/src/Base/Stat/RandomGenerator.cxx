//                                               -*- C++ -*-
/**
 *  @brief RandomGenerator implements methods to control the random generator
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
#include <cstdlib>
#include "dsfmt.h"
#include "openturns/RandomGenerator.hxx"
#include "openturns/RandomGeneratorState.hxx"
#include "openturns/ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

class MersenneTwister : public tutils::dsfmt19937
{
public:
  explicit MersenneTwister(UnsignedInteger seed)
    : tutils::dsfmt19937(seed)
  {}
};



Bool RandomGenerator::IsInitialized = false;

MersenneTwister RandomGenerator::Generator(ResourceMap::GetAsUnsignedInteger( "RandomGenerator-InitialSeed" ));

/* Sub-classes methods */




/* DefaultConstructor */
RandomGenerator::RandomGenerator()
{
  // Nothing to do
}

/* Seed accessor */
void RandomGenerator::SetSeed(const UnsignedInteger seed)
{
  Generator.init((uint32_t)(seed));
  IsInitialized = true;
}

/* State accessor */
void RandomGenerator::SetState(const RandomGeneratorState & state)
{
  UnsignedInteger size = state.buffer_.getSize();
  UnsignedInteger stateSize = Generator.get_state_length_32();
  /* The unusual case, the given seed is too small. It is completed with 0 */
  Indices stateArray(state.buffer_);
  for (UnsignedInteger i = size; i < stateSize; i++) stateArray.add(0);
  // Set the state array
  Generator.set_state(&stateArray[0]);
  // Set the index
  Generator.set_index(state.index_);
  IsInitialized = true;
  return;
}

/* Seed accessor */
RandomGeneratorState RandomGenerator::GetState()
{
  UnsignedInteger size = (UnsignedInteger)(Generator.get_state_length_32());
  // Create the state and get the index at the same time
  RandomGeneratorState state(Indices(size, 0), (UnsignedInteger)(Generator.get_index()));
  // Get the state array
  Generator.get_state(&state.buffer_[0]);
  return state;
}

/* Generate a pseudo-random number uniformly distributed over ]0, 1[ */
Scalar RandomGenerator::Generate()
{
  if (!IsInitialized)
  {
    SetSeed(ResourceMap::GetAsUnsignedInteger( "RandomGenerator-InitialSeed" ));
    IsInitialized = true;
  }
  return Generator.gen();
}

/* Generate a pseudo-random integer uniformly distributed over [[0,...,n-1]] */
UnsignedInteger RandomGenerator::IntegerGenerate(const UnsignedInteger n)
{
  if (!IsInitialized)
  {
    SetSeed(ResourceMap::GetAsUnsignedInteger( "RandomGenerator-InitialSeed" ));
    IsInitialized = true;
  }
  return Generator.igen((uint32_t)(n));
}

/* Generate a pseudo-random vector of numbers uniformly distributed over ]0, 1[ */
Point RandomGenerator::Generate(const UnsignedInteger size)
{
  Point result(size);
  if (!IsInitialized)
  {
    SetSeed(ResourceMap::GetAsUnsignedInteger( "RandomGenerator-InitialSeed" ));
    IsInitialized = true;
  }
  for (UnsignedInteger i = 0; i < size; i++)
  {
    result[i] = Generator.gen();
  }
  return result;
}

/* Generate a pseudo-random vector of numbers uniformly distributed over ]0, 1[ */
RandomGenerator::UnsignedIntegerCollection RandomGenerator::IntegerGenerate(const UnsignedInteger size, const UnsignedInteger n)
{
  UnsignedIntegerCollection result(size);
  if (!IsInitialized)
  {
    SetSeed(ResourceMap::GetAsUnsignedInteger( "RandomGenerator-InitialSeed" ));
    IsInitialized = true;
  }
  for (UnsignedInteger i = 0; i < size; i++)
  {
    result[i] = Generator.igen((uint32_t)(n));
  }
  return result;
}

END_NAMESPACE_OPENTURNS
