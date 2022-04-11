//                                               -*- C++ -*-
/**
 *  @brief Graph implements graphic devices for plotting through R
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GRAPH_HXX
#define OPENTURNS_GRAPH_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/GraphImplementation.hxx"
#include "openturns/DrawableImplementation.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Graph
 *
 * Graph implements graphic devices for plotting through R,
 * and manages drawables to be plotted on the same window
 */

class OT_API Graph :
  public TypedInterfaceObject<GraphImplementation>
{

  CLASSNAME

public:

  typedef Collection<Graph>                             GraphCollection;
  typedef GraphImplementation::DrawableCollection       DrawableCollection;
  typedef GraphImplementation::Format                   Format;
  typedef GraphImplementation::LogScale                 LogScale;
  typedef GraphImplementation::TickLocation             TickLocation;

  /** Default constructor */
  explicit Graph(const String & title = "");

  /** Constructor from implementation */
  Graph(const GraphImplementation & implementation);

  /** Constructor with parameters */
  Graph(const String & title,
        const String & xTitle,
        const String & yTitle,
        const Bool showAxes,
        const String & legendPosition = "",
        const Scalar legendFontSize = ResourceMap::GetAsScalar("Graph-DefaultLegendFontSize"),
        const LogScale logScale = GraphImplementation::NONE);


  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Adds a drawable instance to the collection of drawables contained in Graph */
  void add(const Drawable & aDrawable);

  /** Adds a collection of drawable instances to the collection of drawables contained in GraphImplementation */
  void add(const DrawableCollection & drawableCollection);

  /** Adds all the drawables in the GraphImplementation to the collection of drawables contained in GraphImplementation */
  void add(const Graph & graph);

  /** Erase a drawable instance from the collection of drawables contained in GraphImplementation */
  void erase(const UnsignedInteger i);

  /** Drawables accessor */
  DrawableCollection getDrawables() const;
  void setDrawables(const DrawableCollection & drawableCollection);

  /** Individual drawable accessor */
  Drawable getDrawable(const UnsignedInteger index) const;
  void setDrawable(const Drawable & drawable,
                   const UnsignedInteger index);

  /** Global color accessor */
  Description getColors() const;
  void setColors(const Description & colors);
  void setDefaultColors();

  /** Global legend accessor */
  Description getLegends() const;
  void setLegends(const Description & legends);

  /** Hide or show x and y axes */
  void setAxes(const Bool showAxes);
  Bool getAxes() const;

  /** Ticks location flag accessor */
  void setTickLocation(const TickLocation tickLocation);
  TickLocation getTickLocation() const;

  /** Set log scale for x, y both or none axes */
  void setLogScale(const LogScale logScale);
  LogScale getLogScale() const;

  /** Hide or show grid */
  void setGrid(const Bool showGrid);
  Bool getGrid() const;

  /** Grid color accessors */
  void setGridColor(const String & color);
  String getGridColor() const;

  /** Accessor for xTitle */
  String getXTitle() const;
  void setXTitle(const String & title);

  /** Accessor for yTitle */
  String getYTitle() const;
  void setYTitle(const String & title);

  /** Accessor for title */
  String getTitle() const;
  void setTitle(const String & title);

  /** The method that generates the graphic files */
  void draw(const String & file,
            const Scalar width = ResourceMap::GetAsUnsignedInteger("Graph-DefaultWidth"),
            const Scalar height = ResourceMap::GetAsUnsignedInteger("Graph-DefaultHeight"),
            SignedInteger format = GraphImplementation::ALL);

  /** Get the R command corresponding to the graph */
  String getRCommand() const;

  /** Clean temporary files */
  void clean();

  /** Margin accessor */
  void setXMargin(const Scalar xMargin);
  void setYMargin(const Scalar yMargin);

  /** Get the bounding box of the whole plot */
  Interval getBoundingBox() const;
  void setBoundingBox(const Interval & boundingBox);

  /** Automatic bounding box accessor */
  Bool getAutomaticBoundingBox() const;
  void setAutomaticBoundingBox(const Bool automaticBoundingBox);

  /** Get the legend position */
  String getLegendPosition() const;
  void setLegendPosition(const String & position);

  /** Get the legend font size */
  Scalar getLegendFontSize() const;
  void setLegendFontSize(const Scalar legendFontSize);

  /** Check for legend position validity */
  static Bool IsValidLegendPosition(const String & position);

  /** Gives all the valid legend positions */
  static Description GetValidLegendPositions();

}; /* class Graph */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRAPH_HXX */
