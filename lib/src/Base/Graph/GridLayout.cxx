//                                               -*- C++ -*-
/**
 *  @brief Grid layout of graphs
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
#include <cstdio>
#include <cstdlib>


#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/GridLayout.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GridLayout)

static const Factory<GridLayout> Factory_GridLayout;
static const Factory<PersistentCollection<Graph> > Factory_PersistentCollection_Graph;

/* Default constructor */
GridLayout::GridLayout(const UnsignedInteger nbRows,
                       const UnsignedInteger nbColumns)
  : PersistentObject()
  , nbRows_(nbRows)
  , nbColumns_(nbColumns)
{
  graphCollection_.resize(nbRows * nbColumns);
  setAxes(false);
}


/* String converter */
String GridLayout::__repr__() const
{
  OSS oss;
  oss << "class=" << GridLayout::GetClassName()
      << " name=" << getName()
      << " nbRows=" << nbRows_
      << " nbColumns=" << nbColumns_
      << " graphCollection=" << graphCollection_;
  return oss;
}

/* Clone method */
GridLayout * GridLayout::clone() const
{
  return new GridLayout(*this);
}

/* Method save() stores the object through the StorageManager */
void GridLayout::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("nbRows_", nbRows_);
  adv.saveAttribute("nbColumns_", nbColumns_);
  adv.saveAttribute("graphCollection_", graphCollection_);
  adv.saveAttribute("title_", title_);
}

/* Method load() reloads the object from the StorageManager */
void GridLayout::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("nbRows_", nbRows_);
  adv.loadAttribute("nbColumns_", nbColumns_);
  adv.loadAttribute("graphCollection_", graphCollection_);
  adv.loadAttribute("title_", title_);
}

UnsignedInteger GridLayout::getNbRows() const
{
  return nbRows_;
}

UnsignedInteger GridLayout::getNbColumns() const
{
  return nbColumns_;
}

Graph GridLayout::getGraph(const UnsignedInteger i, const UnsignedInteger j) const
{
  if (!(i < nbRows_))
    throw InvalidArgumentException(HERE) << "Row index must be lesser than number of rows (" << nbRows_ << ")";
  if (!(j < nbColumns_))
    throw InvalidArgumentException(HERE) << "Column index must be lesser than number of columns (" << nbColumns_ << ")";
  return graphCollection_[i * nbColumns_ + j];
}

void GridLayout::setGraph(const UnsignedInteger i, const UnsignedInteger j, const Graph & elt)
{
  if (!(i < nbRows_))
    throw InvalidArgumentException(HERE) << "Row index must be lesser than number of rows (" << nbRows_ << ")";
  if (!(j < nbColumns_))
    throw InvalidArgumentException(HERE) << "Column index must be lesser than number of columns (" << nbColumns_ << ")";
  graphCollection_[i * nbColumns_ + j] = elt;
}

Graph::GraphCollection GridLayout::getGraphCollection() const
{
  return graphCollection_;
}

void GridLayout::setGraphCollection(const Graph::GraphCollection & coll)
{
  if (coll.getSize() > graphCollection_.getSize()) throw InvalidArgumentException(HERE) << "Error: expected a graph collection of size at most " << graphCollection_.getSize() << ", got size=" << coll.getSize();
  graphCollection_ = Graph::GraphCollection(nbRows_ * nbColumns_);
  std::copy(coll.begin(), coll.end(), graphCollection_.begin());
}

void GridLayout::setLayout(const UnsignedInteger nbRows,
                           const UnsignedInteger nbColumns)
{
  const UnsignedInteger oldSize = graphCollection_.getSize();
  const UnsignedInteger newSize = nbRows * nbColumns;
  if (newSize < oldSize) LOGWARN(OSS() << "The new layout will contain " << newSize << " graphs, the old layout was for " << oldSize << " graphs. The remaining graphs are removed from the layout.");
  graphCollection_.resize(newSize);
  nbRows_ = nbRows;
  nbColumns_ = nbColumns;
}

void GridLayout::setTitle(const String & title)
{
  title_ = title;
}

String GridLayout::getTitle() const
{
  return title_;
}

/* Hide or show x and y axes */
void GridLayout::setAxes(const Bool showAxes)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setAxes(showAxes);
}

/* Set the legend position */
void GridLayout::setLegendPosition(const String & position)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setLegendPosition(position);
}

END_NAMESPACE_OPENTURNS
