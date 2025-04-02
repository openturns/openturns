//                                               -*- C++ -*-
/**
 *  @brief Contour class for piechart plots
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CONTOUR_HXX
#define OPENTURNS_CONTOUR_HXX

#include "openturns/DrawableImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Contour
 *
 * The class describing a contour chart
 * Instance of Drawable
 */


class OT_API Contour : public DrawableImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  Contour();

  /** Constructor with parameters */
  Contour(const Sample & x,
          const Sample & y,
          const Sample & data);

  /** Constructor with parameters */
  Contour(const UnsignedInteger dimX,
          const UnsignedInteger dimY,
          const Sample & data);

  /** String converter */
  String __repr__() const override;

  /** Accessor for color overridden to clear colorMap */
  void setColor(const String& color) override;

  /** Accessor for first coordinate */
  Sample getX() const override;
  void setX(const Sample & x) override;

  /** Accessor for second coordinate */
  Sample getY() const override;
  void setY(const Sample & y) override;

  /** Accessor for levels */
  Point getLevels() const override;
  void setLevels(const Point & levels) override;

  /** Accessor for labels */
  Description getLabels() const override;
  void setLabels(const Description & labels) override;

  /** Accessor for drawLabels */
  Bool getDrawLabels() const override;
  void setDrawLabels(const Bool & drawLabels) override;

  /** Accessor for isFilled */
  Bool isFilled() const;
  void setIsFilled(Bool isFilled);

  /** Accessor for colorBarPosition */
  String getColorBarPosition() const;
  void setColorBarPosition(const String & colorBarPosition);

  /** Accessor for isVminUsed */
  Bool isVminUsed() const;
  void setIsVminUsed(const Bool used);

  /** Accessor for vmin */
  Scalar getVmin() const;
  void setVmin(const Scalar vmin);

  /** Accessor for isVmaxUsed */
  Bool isVmaxUsed() const;
  void setIsVmaxUsed(const Bool used);

  /** Accessor for vmax */
  Scalar getVmax() const;
  void setVmax(const Scalar vmax);

  /** Accessor for colorMap */
  String getColorMap() const;
  void setColorMap(const String & colorMap);

  /** Accessor for norm */
  String getColorMapNorm() const;
  void setColorMapNorm(const String & norm);

  /** Accessor for extend */
  String getExtend() const;
  void setExtend(const String & extend);

  /** Accessor for hatches */
  Description getHatches() const;
  void setHatches(const Description & hatches);

  /** Accessor for boundingbox */
  Interval getBoundingBox() const override;

  /** Clone method */
  Contour * clone() const override;

  /** Build default levels using quantiles associated with regularly spaced probability levels
   */
  void buildDefaultLevels(const UnsignedInteger number = ResourceMap::GetAsUnsignedInteger( "Contour-DefaultLevelsNumber" ));

  /** Build default labels by taking the level values */
  void buildDefaultLabels();

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check fo data validity */
  void checkData(const Sample & data) const override;

private:

  /** Sample of first coordinate */
  Sample x_;

  /** Sample of second coordinate */
  Sample y_;

  /** Values of the level sets */
  Point levels_;

  /** Labels of the level sets. If none is given, it defaults to the level values. */
  Description labels_;

  /** Flag for drawing the labels */
  Bool drawLabels_;

  /** Flag for filling the contours */
  Bool isFilled_;

  /** Color bar position */
  String colorBarPosition_;

  /** use of vmin value */
  Bool isVminUsed_ = false;

  /** Min value of color map */
  Scalar vmin_ = 0.0;

  /** use of vmax value */
  Bool isVmaxUsed_ = false;

  /** Max value of color map */
  Scalar vmax_ = 0.0;

  /** Name of the color map */
  String colorMap_;

  /** Name of the scale normalization method */
  String norm_;

  /** Extension of coloring beyond extreme levels */
  String extend_;

  /** Hatch patterns to use on the filled areas */
  Description hatches_;

}; /* class Contour */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONTOUR_HXX */
