//                                               -*- C++ -*-
/**
 *  @brief A class which implements a discrete Markov chain process
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DISCRETEMARKOVCHAIN_HXX
#define OPENTURNS_DISCRETEMARKOVCHAIN_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/Process.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Point.hxx"
#include "openturns/SpecFunc.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DiscreteMarkovChain
 *
 * An implementation class for composite discrete Markov chains
 */
class OT_API DiscreteMarkovChain
  : public ProcessImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  DiscreteMarkovChain();

  /** Standard constructors */
  DiscreteMarkovChain(const Distribution & origin,
                      const SquareMatrix & transition);

  DiscreteMarkovChain(const Distribution & origin,
                      const SquareMatrix & transition,
                      const RegularGrid & timeGrid);

  /** Constructors with fixed origin */
  DiscreteMarkovChain(const UnsignedInteger origin,
                      const SquareMatrix & transition);

  DiscreteMarkovChain(const UnsignedInteger origin,
                      const SquareMatrix & transition,
                      const RegularGrid & timeGrid);

  /** Virtual constructor */
  virtual DiscreteMarkovChain * clone() const;

  /** String converters */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Realization accessor */
  Field getRealization() const;

  /** Continuation of the last realization on a given number of steps  */
  using ProcessImplementation::getFuture;
  TimeSeries getFuture(const UnsignedInteger stepNumber) const;

  /** Transition matrix accessors */
  SquareMatrix getTransitionMatrix() const;
  void setTransitionMatrix(const SquareMatrix & transitionMatrix);

  /** Origin accessors */
  Distribution getOrigin() const;
  void setOrigin(const Distribution & origin);
  void setOrigin(const UnsignedInteger origin);

  /** Mesh accessors */
  void setMesh(const Mesh & mesh);

  /** DOT export */
  void exportToDOTFile(const FileName & filename) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** The origin of the chain */
  Distribution origin_;

  /** The transition matr
  ix of the chain */
  SquareMatrix transitionMatrix_ ;

  /** The current position of the chain */
  mutable UnsignedInteger currentState_;

}; /* class DiscreteMarkovChain */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISCRETEMARKOVCHAIN_HXX */
