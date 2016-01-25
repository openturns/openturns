//                                               -*- C++ -*-
/**
 *  @brief RandomizedLHS is an implementation of the randomized Latin Hypercube Sampling method
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_RANDOMIZEDLHS_HXX
#define OPENTURNS_RANDOMIZEDLHS_HXX

#include "Simulation.hxx"
#include "Collection.hxx"
#include "PersistentCollection.hxx"
#include "Distribution.hxx"
#include "Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class RandomizedLHS
 */

class OT_API RandomizedLHS: public Simulation
{

  CLASSNAME;
public:

  typedef Collection<Distribution>           Marginals;
  typedef PersistentCollection<Distribution> PersistentMarginals;

  /** Default constructor */
  RandomizedLHS();

  /** Constructor with parameters */
  explicit RandomizedLHS(const Event & event);

  /** Virtual constructor */
  virtual RandomizedLHS * clone() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Compute the block sample */
  NumericalSample computeBlockSample();

  /** Second antecedent dimension */
  UnsignedInteger dimension_;

  /** Cells shuffle */
  Matrix shuffle_;

  /** Marginal distributions */
  PersistentMarginals marginals_;

private:

} ; /* class RANDOMIZEDLHS */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMIZEDLHS_HXX */
