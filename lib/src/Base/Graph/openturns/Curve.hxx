//                                               -*- C++ -*-
/**
 *  @brief Curve class for curve plots
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
#ifndef OPENTURNS_CURVE_HXX
#define OPENTURNS_CURVE_HXX

#include "openturns/DrawableImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Curve
 *
 * The class describing a curve plot
 * Instance of Drawable
 */


class OT_API Curve : public DrawableImplementation
{

  CLASSNAME

public:


  /** Default constructor */
  explicit Curve(const String & legend = "");

  /** Default constructor */
  explicit Curve(const Sample & data,
                 const String & legend = "");

  /** Constructor from 2 data sets */
  Curve(const Sample & dataX,
        const Sample & dataY,
        const String & legend = "");

  /** Constructor from 2 data sets */
  Curve(const Point & dataX,
        const Point & dataY,
        const String & legend = "");

  /** Constructor with parameters */
  Curve(const Sample & data,
        const String & color,
        const String & lineStyle,
        const Scalar lineWidth = 1.0,
        const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Clone method */
  Curve * clone() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check fo data validity */
  void checkData(const Sample & data) const override;

private:

}; /* class Curve */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CURVE_HXX */
