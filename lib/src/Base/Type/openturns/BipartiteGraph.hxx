//                                               -*- C++ -*-
/**
 *  @brief BipartiteGraph defines a graph with two sets of nodes (red and black)
 *         and links from one set to the other only.
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
#ifndef OPENTURNS_BIPARTITEGRAPH_HXX
#define OPENTURNS_BIPARTITEGRAPH_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/IndicesCollectionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BipartiteGraph
 *
 * A class that represents bipartite graphs
 */
class OT_API BipartiteGraph :
  public IndicesCollectionImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  BipartiteGraph()
    : IndicesCollectionImplementation(1, 1)
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements */
  BipartiteGraph(const UnsignedInteger size, const UnsignedInteger stride)
    : IndicesCollectionImplementation(size, stride)
  {
    // Nothing to do
  }

  /** Constructor from a base object */
  BipartiteGraph(const IndicesCollection & coll) : IndicesCollectionImplementation(*coll.getImplementation())
  {
    // Nothing to do
  }

  /** Accessor to the red nodes */
  Indices getRedNodes() const;

  /** Accessor to the black nodes */
  Indices getBlackNodes() const;

  /** Draw the bipartite graph */
  Graph draw() const;

#ifdef SWIG
  /** @copydoc Object::__repr__() const */
  String __repr__() const override;

  /** @copydoc Object::__str__() const */
  String __str__(const String & offset = "") const override;
#endif

}; /* class BipartiteGraph */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BIPARTITEGRAPH_HXX */
