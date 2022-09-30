//                                               -*- C++ -*-
/**
 *  @brief GraphImplementation implements graphic devices for plotting through R
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
  enum TickLocation { TICKNONE, TICKX, TICKY, TICKXY };

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
  GraphImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

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
  virtual void setAxes(const Bool showAxes);
  Bool getAxes() const;

  /** Ticks location flag accessor */
  virtual void setTickLocation(const TickLocation tickLocation);
  TickLocation getTickLocation() const;

  /** integer x-tick flag accessor */
  virtual void setIntegerXTick(const Bool integerXTick);
  Bool getIntegerXTick() const;

  /** integer y-tick flag accessor */
  virtual void setIntegerYTick(const Bool integerYTick);
  Bool getIntegerYTick() const;

  /** Set log scale for x, y both or none axes */
  virtual void setLogScale(const LogScale logScale);
  LogScale getLogScale() const;

  /** Hide or show grid */
  virtual void setGrid(const Bool showGrid);
  Bool getGrid() const;

  /** Grid color accessor */
  virtual void setGridColor(const String & color);
  String getGridColor() const;

  /** Accessor for xTitle */
  String getXTitle() const;
  void setXTitle(const String & title);

  /** Accessor for yTitle */
  String getYTitle() const;
  void setYTitle(const String & title);

  /** Accessor for title */
  virtual String getTitle() const;
  void setTitle(const String & title);

  /** @deprecated The method that generates the graphic files */
  void draw(const String & file,
            const Scalar width = ResourceMap::GetAsUnsignedInteger("Graph-DefaultWidth"),
            const Scalar height = ResourceMap::GetAsUnsignedInteger("Graph-DefaultHeight"),
            SignedInteger format = ALL);

  /** @deprecated Get the R command corresponding to the graph */
  String getRCommand() const;

  /** @deprecated Clean temporary files */
  void clean();

  /** Margin accessor */
  virtual void setXMargin(const Scalar xMargin);
  virtual void setYMargin(const Scalar yMargin);

  /** Get the bounding box of the whole plot */
  virtual Interval getBoundingBox() const;
  virtual void setBoundingBox(const Interval & boundingBox);

  /** Automatic bounding box accessor */
  virtual Bool getAutomaticBoundingBox() const;
  virtual void setAutomaticBoundingBox(const Bool automaticBoundingBox);

  /** Legend position accessor */
  virtual void setLegendPosition(const String & position);
  String getLegendPosition() const;

  /** Get the legend font size */
  Scalar getLegendFontSize() const;
  void setLegendFontSize(const Scalar legendFontSize);

  /** Check for legend position validity */
  static Bool IsValidLegendPosition(const String & position);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

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

  /** main title */
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
  Bool showAxes_ = false;

  /** Ticks location flag */
  TickLocation tickLocation_ = TICKXY;

  /** Ticks values */
  Bool integerXTick_ = false;
  Bool integerYTick_ = false;

  /** Set the log scale for one, both or non of the axes */
  LogScale logScale_ = NONE;

  /** if TRUE, displays a grid on the graphic plot. if FALSE, hides the grid */
  Bool showGrid_ = false;

  /** Grid color */
  String gridColor_ = "gray";

  /** Margins (ratio) */
  Scalar xMargin_ = 0.0;
  Scalar yMargin_ = 0.0;

  /** Is the bounding box automatically computed. */
  Bool automaticBoundingBox_ = true;

  /** Current bounding box */
  mutable Interval boundingBox_;

  /** The drawables to be plotted */
  DrawablePersistentCollection drawablesCollection_;

}; /* class GraphImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRAPHIMPLEMENTATION_HXX */
