//                                               -*- C++ -*-
/**
 *  @brief BipartiteGraph defines a graph with two sets of nodes (red and black)
 *         and links from one set to the other only.
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <algorithm>
#include "openturns/BipartiteGraph.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BipartiteGraph)
static const Factory<BipartiteGraph> Factory_BipartiteGraph;

/* Accessor to the red nodes */
Indices BipartiteGraph::getRedNodes() const
{
  return Indices(getSize());
}

/* Accessor to the black nodes */
Indices BipartiteGraph::getBlackNodes() const
{
  Indices blackNodes(0);
  for (UnsignedInteger i = 0; i < getSize(); ++i)
    blackNodes.add(Collection<UnsignedInteger>(cbegin_at(i), cend_at(i)));
  std::sort(blackNodes.begin(), blackNodes.end());
  Indices::iterator it = std::unique(blackNodes.begin(), blackNodes.end());
  blackNodes.resize(std::distance(blackNodes.begin(), it));
  return blackNodes;
}

/* Draw the bipartite graph */
Graph BipartiteGraph::draw() const
{
  Graph graph("Bipartite graph", "", "", true, "topright");
  for (UnsignedInteger i = 0; i < getSize(); ++i)
  {
    Point redPoint(2);
    redPoint[0] = i;
    redPoint[1] = 2.0;
    for (const_iterator cit = cbegin_at(i); cit != cend_at(i); ++cit)
    {
      Point blackPoint(2);
      blackPoint[0] = *cit;
      blackPoint[1] = 1.0;
      // First, the link
      Sample data(2, 2);
      data[0] = redPoint;
      data[1] = blackPoint;
      Curve curve(data);
      curve.setColor("blue");
      graph.add(curve);
      Cloud cloud(Sample(1, blackPoint));
      cloud.setColor("black");
      cloud.setPointStyle("fcircle");
      graph.add(cloud);
    }
    Cloud cloud(Sample(1, redPoint));
    cloud.setColor("red");
    cloud.setPointStyle("fcircle");
    graph.add(cloud);
  }
  const Interval bb(graph.getBoundingBox());
  graph.setBoundingBox(Interval(bb.getLowerBound() - Point(2, 0.5), bb.getUpperBound() + Point(2, 0.5)));
  return graph;
}

END_NAMESPACE_OPENTURNS
