//                                               -*- C++ -*-
/**
 *  @brief Graph implements graphic devices for plotting through R
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
#include "openturns/Graph.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(Graph)

/* Default constructor */
Graph::Graph(const String & title):
  TypedInterfaceObject<GraphImplementation>(new GraphImplementation(title))
{
  // Nothing to do
}

/* Constructor from implementation */
Graph::Graph(const GraphImplementation & implementation)
  : TypedInterfaceObject<GraphImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor with parameters */
Graph::Graph(const String & title,
             const String & xTitle,
             const String & yTitle,
             const Bool showAxes,
             const String & legendPosition,
             const Scalar legendFontSize,
             const LogScale logScale)
  : TypedInterfaceObject<GraphImplementation>(new GraphImplementation(title, xTitle, yTitle, showAxes, legendPosition, legendFontSize, logScale))
{
  // Nothing to do
}

/* String converter */
String Graph::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String Graph::__str__(const String & ) const
{
  return __repr__();
}

/* Adds a drawable instance to the collection of drawables contained in Graph */
void Graph::add(const Drawable & aDrawable)
{
  copyOnWrite();
  getImplementation()->add(aDrawable);
}

/* Adds a collection of drawable instances to the collection of drawables contained in GraphImplementation */
void Graph::add(const Graph & graph)
{
  copyOnWrite();
  getImplementation()->add(*graph.getImplementation());
}

/* Adds a collection of drawable instances to the collection of drawables contained in GraphImplementation */
void Graph::add(const DrawableCollection & drawableCollection)
{
  copyOnWrite();
  getImplementation()->add(drawableCollection);
}

/* Erase a drawable instance from the collection of drawables contained in GraphImplementation */
void Graph::erase(const UnsignedInteger i)
{
  copyOnWrite();
  getImplementation()->erase(i);
}

/* Drawables accessor */
Graph::DrawableCollection Graph::getDrawables() const
{
  return getImplementation()->getDrawables();
}

void Graph::setDrawables(const DrawableCollection & drawableCollection)
{
  copyOnWrite();
  getImplementation()->setDrawables(drawableCollection);
}
/* Individual drawable accessor */
Drawable Graph::getDrawable(const UnsignedInteger index) const
{
  return getImplementation()->getDrawable(index);
}

void Graph::setDrawable(const Drawable & drawable,
                        const UnsignedInteger index)
{
  copyOnWrite();
  getImplementation()->setDrawable(drawable, index);
}


/* Global color accessor */
Description Graph::getColors() const
{
  return getImplementation()->getColors();
}

void Graph::setColors(const Description & colors)
{
  copyOnWrite();
  getImplementation()->setColors(colors);
}

void Graph::setDefaultColors()
{
  copyOnWrite();
  getImplementation()->setDefaultColors();
}

/* Global legend accessor */
void Graph::setLegends(const Description & legends)
{
  copyOnWrite();
  getImplementation()->setLegends(legends);
}
Description Graph::getLegends() const
{
  return getImplementation()->getLegends();
}

/* Hide or show x and y axes */
void Graph::setAxes(const Bool showAxes)
{
  copyOnWrite();
  getImplementation()->setAxes(showAxes);
}

/* Accessor for showAxes_ */
Bool Graph::getAxes() const
{
  return getImplementation()->getAxes();
}

/* Ticks location flag accessor */
void Graph::setTickLocation(const TickLocation tickLocation)
{
  copyOnWrite();
  getImplementation()->setTickLocation(tickLocation);
}

Graph::TickLocation Graph::getTickLocation() const
{
  return getImplementation()->getTickLocation();
}

/* integer X-ticks flag accessor */
void Graph::setIntegerXTick(const Bool integerXTick)
{
  copyOnWrite();
  getImplementation()->setIntegerXTick(integerXTick);
}

Bool Graph::getIntegerXTick() const
{
  return getImplementation()->getIntegerXTick();
}

/* integer Y-ticks flag accessor */
void Graph::setIntegerYTick(const Bool integerYTick)
{
  copyOnWrite();
  getImplementation()->setIntegerYTick(integerYTick);
}

Bool Graph::getIntegerYTick() const
{
  return getImplementation()->getIntegerYTick();
}

/* Set log scale for x, y both or none axes */
void Graph::setLogScale(const LogScale logScale)
{
  copyOnWrite();
  getImplementation()->setLogScale(logScale);
}

/* Accessor for showAxes_ */
Graph::LogScale Graph::getLogScale() const
{
  return getImplementation()->getLogScale();
}

/* Hide or show grid */
void Graph::setGrid(const Bool showGrid)
{
  copyOnWrite();
  getImplementation()->setGrid(showGrid);
}

/* Accessor for showAxes_ */
Bool Graph::getGrid() const
{
  return getImplementation()->getGrid();
}

/* Grid color accessors */
void Graph::setGridColor(const String & color)
{
  copyOnWrite();
  getImplementation()->setGridColor(color);
}

/* Accessor for showGrid_ */
String Graph::getGridColor() const
{
  return getImplementation()->getGridColor();
}

/* Accessor for xTitle */
String Graph::getXTitle() const
{
  return getImplementation()->getXTitle();
}

/* Accessor for xTitle */
void Graph::setXTitle(const String & title)
{
  copyOnWrite();
  getImplementation()->setXTitle(title);
}

/* Accessor for yTitle */
String Graph::getYTitle() const
{
  return getImplementation()->getYTitle();
}

/* Accessor for yTitle */
void Graph::setYTitle(const String & title)
{
  copyOnWrite();
  getImplementation()->setYTitle(title);
}

/* Accessor for title */
String Graph::getTitle() const
{
  return getImplementation()->getTitle();
}

/* Accessor for title */
void Graph::setTitle(const String & title)
{
  copyOnWrite();
  getImplementation()->setTitle(title);
}

/* Margin accessor */
void Graph::setXMargin(const Scalar xMargin)
{
  getImplementation()->setXMargin(xMargin);
}

void Graph::setYMargin(const Scalar yMargin)
{
  getImplementation()->setYMargin(yMargin);
}

/* Get the bounding box of the whole plot */
Interval Graph::getBoundingBox() const
{
  return getImplementation()->getBoundingBox();
}

void Graph::setBoundingBox(const Interval & boundingBox)
{
  copyOnWrite();
  getImplementation()->setBoundingBox(boundingBox);
}

/* Automatic bounding box accessor */
Bool Graph::getAutomaticBoundingBox() const
{
  return getImplementation()->getAutomaticBoundingBox();
}

void Graph::setAutomaticBoundingBox(const Bool automaticBoundingBox)
{
  copyOnWrite();
  getImplementation()->setAutomaticBoundingBox(automaticBoundingBox);
}

/* Get the legend position */
String Graph::getLegendPosition() const
{
  return getImplementation()->getLegendPosition();
}

/* Set the legend position */
void Graph::setLegendPosition(const String & position)
{
  copyOnWrite();
  getImplementation()->setLegendPosition(position);
}

/* Get the legend font size */
Scalar Graph::getLegendFontSize() const
{
  return getImplementation()->getLegendFontSize();
}

/* Set the legend font size */
void Graph::setLegendFontSize(const Scalar legendFontSize)
{
  copyOnWrite();
  getImplementation()->setLegendFontSize(legendFontSize);
}

/* check for legend position validity */
Bool Graph::IsValidLegendPosition(const String & position)
{
  return GraphImplementation::IsValidLegendPosition(position);
}

/* Gives all the valid legend positions */
Description Graph::GetValidLegendPositions()
{
  return GraphImplementation::GetValidLegendPositions();
}

/* Legend bounding box accessor */
void Graph::setLegendCorner(const Point & corner)
{
  copyOnWrite();
  getImplementation()->setLegendCorner(corner);
}

Point Graph::getLegendCorner() const
{
  return getImplementation()->getLegendCorner();
}

END_NAMESPACE_OPENTURNS
