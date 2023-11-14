//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
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
#ifndef OPENTURNS_RANDOMVECTORMETROPOLISHASTINGS_HXX
#define OPENTURNS_RANDOMVECTORMETROPOLISHASTINGS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MetropolisHastingsImplementation.hxx"
#include "openturns/RandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomVectorMetropolisHastings
 *
 * @brief Metropolis-Hastings algorithm.
 *
 */
class OT_API RandomVectorMetropolisHastings
  : public MetropolisHastingsImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  RandomVectorMetropolisHastings();

  /** Constructor with parameters*/
  RandomVectorMetropolisHastings(const RandomVector & randomVector,
                                 const Point & initialState,
                                 const Indices & marginalIndices = Indices(),
                                 const Function & linkFunction = Function());

  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  RandomVectorMetropolisHastings * clone() const override;

  /** RV accessor */
  void setRandomVector(const RandomVector & randomVector);
  RandomVector getRandomVector() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Propose a new point in the chain */
  Point getCandidate() const override;

private:
  mutable RandomVector randomVector_;

  // link between the state and the parameters of the source random vector
  Function randomVectorLinkFunction_;

}; /* class RandomVectorMetropolisHastings */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX */
