//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an combinatorialGenerator plane
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_COMBINATORIALGENERATORIMPLEMENTATION_HXX
#define OPENTURNS_COMBINATORIALGENERATORIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Sample.hxx"
#include "openturns/IndicesCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class CombinatorialGeneratorImplementation
 *
 * The class describes the probabilistic concept of combinatorialGeneratorImplementation plan
 */
class OT_API CombinatorialGeneratorImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Pointer<CombinatorialGeneratorImplementation> Implementation;

  /** Default constructor */
  CombinatorialGeneratorImplementation();

  /** Virtual constructor */
  CombinatorialGeneratorImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** @deprecated Sample generation, loop over generateNext() */
  virtual IndicesCollection generate() const;

  /* Here is the interface that all derived class must implement */

  /** Next combination generation, stateful iteration like SplitterImplementation */
  virtual Indices generateNext() const;

  /** Number of combinations accessor */
  virtual UnsignedInteger getSize() const;

  /** Dimension of the generated combinations accessor */
  virtual UnsignedInteger getDimension() const;

  /** Restart the combinatorial sequence */
  virtual void restart() const;

protected:

private:

}; /* class CombinatorialGeneratorImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMBINATORIALGENERATORIMPLEMENTATION_HXX */
