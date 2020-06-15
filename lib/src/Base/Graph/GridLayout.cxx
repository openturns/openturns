//                                               -*- C++ -*-
/**
 *  @brief Grid layout of graphs
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
#include <cstdio>
#include <cstdlib>


#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/GridLayout.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GridLayout)

static const Factory<GridLayout> Factory_GridLayout;

/* Default constructor */
GridLayout::GridLayout(const UnsignedInteger nbRows,
                       const UnsignedInteger nbColumns)
  : GraphImplementation()
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
  GraphImplementation::save(adv);
  adv.saveAttribute("nbRows_", nbRows_);
  adv.saveAttribute("nbColumns_", nbColumns_);
  adv.saveAttribute("graphCollection_", graphCollection_);
}

/* Method load() reloads the object from the StorageManager */
void GridLayout::load(Advocate & adv)
{
  GraphImplementation::load(adv);
  adv.loadAttribute("nbRows_", nbRows_);
  adv.loadAttribute("nbColumns_", nbColumns_);
  adv.loadAttribute("graphCollection_", graphCollection_);
}

Bool GridLayout::isComposite() const
{
  return true;
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

/* Hide or show x and y axes */
void GridLayout::setAxes(const Bool showAxes)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setAxes(showAxes);
}

void GridLayout::setLegendPosition(const String & position)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setLegendPosition(position);
}

/* Accessor for logScale */
void GridLayout::setLogScale(const LogScale logScale)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setLogScale(logScale);
}

/* Hide or show grid */
void GridLayout::setGrid(const Bool showGrid)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setGrid(showGrid);
}

/* Grid color accessor */
void GridLayout::setGridColor(const String & color)
{
  for (UnsignedInteger i = 0; i < graphCollection_.getSize(); ++ i)
    graphCollection_[i].setGridColor(color);
}

END_NAMESPACE_OPENTURNS
