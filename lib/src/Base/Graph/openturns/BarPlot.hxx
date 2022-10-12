//                                               -*- C++ -*-
/**
 *  @brief BarPlot class for barplots
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
#ifndef OPENTURNS_BARPLOT_HXX
#define OPENTURNS_BARPLOT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DrawableImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BarPlot
 *
 * The class describing a curve plot
 * Instance of Drawable
 */


class OT_API BarPlot : public DrawableImplementation
{

  CLASSNAME

public:
  /** Default constructor */
  BarPlot();

  /** Default constructor */
  BarPlot(const Sample & data,
          const Scalar origin,
          const String & legend = "");

  /** Constructor with parameters */
  BarPlot(const Sample & data,
          const Scalar origin,
          const String & color,
          const String & fillStyle,
          const String & lineStyle,
          const Scalar lineWidth,
          const String & legend = "");

  /** Constructor with old parameters */
  BarPlot(const Sample & data,
          const Scalar origin,
          const String & color,
          const String & fillStyle,
          const String & lineStyle,
          const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Accessor for origin */
  Scalar getOrigin() const override;
  void setOrigin(const Scalar origin) override;

  /** Accessor for boundingbox */
  Interval getBoundingBox() const override;

  /** Clone method */
  BarPlot * clone() const override;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check fo data validity */
  void checkData(const Sample & data) const override;

private:

  /** Origin of the BarPlot */
  Scalar origin_;

}; /* class BarPlot */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BARPLOT_HXX */
