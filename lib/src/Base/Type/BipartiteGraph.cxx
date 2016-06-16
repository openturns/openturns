//                                               -*- C++ -*-
/**
 *  @brief BipartiteGraph defines a graph with two sets of nodes (red and black)
 *         and links from one set to the other only.
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
#include <algorithm>
#include "openturns/BipartiteGraph.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BipartiteGraph);
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
    blackNodes.add((*this)[i]);
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
      NumericalPoint redPoint(2);
      redPoint[0] = i;
      redPoint[1] = 2.0;
      for (UnsignedInteger j = 0; j < (*this)[i].getSize(); ++j)
        {
          NumericalPoint blackPoint(2);
          blackPoint[0] = (*this)[i][j];
          blackPoint[1] = 1.0;
          // First, the link
          NumericalSample data(2, 2);
          data[0] = redPoint;
          data[1] = blackPoint;
          Curve curve(data);
          curve.setColor("blue");
          graph.add(curve);
          Cloud cloud(NumericalSample(1, blackPoint));
          cloud.setColor("black");
          cloud.setPointStyle("fcircle");
	  graph.add(cloud);
        }
      Cloud cloud(NumericalSample(1, redPoint));
      cloud.setColor("red");
      cloud.setPointStyle("fcircle");
      graph.add(cloud);
    }
  NumericalPoint bb(graph.getBoundingBox());
  bb[0] -= 0.5;
  bb[1] += 0.5;
  bb[2] -= 0.5;
  bb[3] += 0.5;
  graph.setBoundingBox(bb);
  return graph;
}

END_NAMESPACE_OPENTURNS
