//                                               -*- C++ -*-
/**
 *  @brief RandomGenerator implements methods to control the random generator
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
#ifndef OPENTURNS_RANDOMGENERATOR_HXX
#define OPENTURNS_RANDOMGENERATOR_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Point.hxx"
#include "openturns/RandomGeneratorState.hxx"

BEGIN_NAMESPACE_OPENTURNS



class MersenneTwister;

/**
 * @class RandomGenerator
 *
 * RandomGenerator implements methods to control the random generator
 */

class OT_API RandomGenerator
{
public:

  typedef Collection<UnsignedInteger> UnsignedIntegerCollection;

  /** Seed accessor */
  static void SetSeed(const UnsignedInteger seed);

  /** State accessor */
  static void SetState(const RandomGeneratorState & state);
  static RandomGeneratorState GetState();

  /** Generate a pseudo-random number uniformly distributed over [0, 1[ */
  static Scalar Generate();
  /** Generate a pseudo-random integer uniformly distributed over [[0,...,n-1]] */
  static UnsignedInteger IntegerGenerate(const UnsignedInteger n);

  /** Generate a pseudo-random vector of numbers uniformly distributed over [0, 1[ */
  static Point Generate(const UnsignedInteger size);
  /** Generate a pseudo-random vector of integers uniformly distributed over [[0,...,n-1]] */
  static UnsignedIntegerCollection IntegerGenerate(const UnsignedInteger size, const UnsignedInteger n);

private:
  /** Default constructor */
  RandomGenerator();

  static void Initialize();

  static Bool IsInitialized_;
  static MersenneTwister Generator_;

}; /* class RandomGenerator */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMGENERATOR_HXX */
