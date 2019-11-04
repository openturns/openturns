//                                               -*- C++ -*-
/**
 *  @brief GraphImplementation implements graphic devices for plotting through R
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
#ifndef OPENTURNS_GRAPHIMPLEMENTATION_HXX
#define OPENTURNS_GRAPHIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Drawable.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GraphImplementation
 *
 * GraphImplementation implements graphic devices for plotting through R,
 * and manages drawables to be plotted on the same window
 */

class OT_API GraphImplementation :
  public PersistentObject
{

  CLASSNAME

public:

  typedef Collection<Drawable>           DrawableCollection;
  typedef PersistentCollection<Drawable> DrawablePersistentCollection;

  enum Format { PNG = 1, EPS = 2, FIG = 4, PDF = 8, ALL = 255 };
  enum LogScale { NONE = 0, LOGX, LOGY, LOGXY };

  /** Default constructor */
  explicit GraphImplementation(const String & title = "");

  /** Constructor with parameters */
  GraphImplementation(const String & title,
                      const String & xTitle,
                      const String & yTitle,
                      const Bool showAxes,
                      const String & legendPosition = "",
                      const Scalar legendFontSize = ResourceMap::GetAsScalar("Graph-DefaultLegendFontSize"),
                      const LogScale logScale = NONE);

  /** Virtual constructor */
  virtual GraphImplementation * clone() const;

  /** String converter */
  String __repr__() const;

  /** Adds a drawable instance to the collection of drawables contained in GraphImplementation */
  void add(const Drawable & aDrawable);

  /** Adds a collection of drawable instances to the collection of drawables contained in GraphImplementation */
  void add(const DrawableCollection & drawableCollection);

  /** Adds all the drawables in the GraphImplementation to the collection of drawables contained in GraphImplementation */
  void add(const GraphImplementation & graphImplementation);

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

  /** Accessor for showAxes_ */
  Bool getAxes() const;

  /** Set log scale for x, y both or none axes */
  void setLogScale(const LogScale logScale);

  /** Accessor for logScale_ */
  LogScale getLogScale() const;

  /** Hide or show grid */
  void setGrid(const Bool showGrid);

  /** Accessor for showGrid_ */
  Bool getGrid() const;

  /** Grid color accessor */
  void setGridColor(const String & color);
  String getGridColor() const;

  /** Accesor for xTitle */
  String getXTitle() const;

  /** Accessor for xTitle */
  void setXTitle(const String & title);

  /** Accessor for yTitle */
  String getYTitle() const;

  /** Accessor for yTitle */
  void setYTitle(const String & title);

  /** Accesor for title */
  String getTitle() const;

  /** Accesor for title */
  void setTitle(const String & title);

  /** The method that generates the graphic files */
  void draw(const String & file,
            const Scalar width = ResourceMap::GetAsUnsignedInteger("Graph-DefaultWidth"),
            const Scalar height = ResourceMap::GetAsUnsignedInteger("Graph-DefaultHeight"),
            SignedInteger format = ALL);

  /** Get the R command corresponding to the graph */
  String getRCommand() const;

  /** Clean temporary files */
  void clean();

  /** Margin accessor */
  void setXMargin(const Scalar xMargin);
  void setYMargin(const Scalar yMargin);

  /** Get the bounding box of the whole plot */
  Interval getBoundingBox() const;

  /** Set the bounding box of the whole plot */
  void setBoundingBox(const Interval & boundingBox);

  /** Automatic bounding box accessor */
  Bool getAutomaticBoundingBox() const;
  void setAutomaticBoundingBox(const Bool automaticBoundingBox);

  /** Get the legend position */
  String getLegendPosition() const;

  /** Set the legend position */
  void setLegendPosition(const String & position);

  /** Get the legend font size */
  Scalar getLegendFontSize() const;

  /** Set the legend font size */
  void setLegendFontSize(const Scalar legendFontSize);

  /** Check for legend position validity */
  static Bool IsValidLegendPosition(const String & position);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Gives all the valid legend positions */
  static Description GetValidLegendPositions();

private:
  static Description ValidLegendPositions;
  static Bool IsFirstInitialization;

  /** Initialize valid legend positions **/
  static void InitializeValidLegendPositions();

  /** Initialize format enum/extension map */
  static std::map<SignedInteger, String> GetExtensionMap();

  /** Compute the best bounding box to enclose all the drawables */
  void computeBoundingBox() const;

  /** Make R legend command */
  String makeRLegendCommand() const;

  /** Make R header command */
  String makeRHeaderCommand() const;

  /** Make R core command */
  String makeRCoreCommand() const;

  /** GraphImplementationic main title */
  String title_;

  /** Legend position */
  String legendPosition_;

  /** Legend font size */
  Scalar legendFontSize_;

  /** x axis title */
  String xTitle_;

  /** y axis title */
  String yTitle_;

  /** if TRUE, displays the axes on the graphic plot. if FALSE, hides the axes */
  Bool showAxes_;

  /** Set the log scale for one, both or non of the axes */
  LogScale logScale_;

  /** if TRUE, displays a grid on the graphic plot. if FALSE, hides the grid */
  Bool showGrid_;

  /** Grid color */
  String gridColor_;

  /** Margins (ratio) */
  Scalar xMargin_;
  Scalar yMargin_;

  /** Is the bounding box automatically computed. */
  Bool automaticBoundingBox_;

  /** Current bounding box */
  mutable Interval boundingBox_;

  /** The drawables to be plotted */
  DrawablePersistentCollection drawablesCollection_;

}; /* class GraphImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRAPHIMPLEMENTATION_HXX */
