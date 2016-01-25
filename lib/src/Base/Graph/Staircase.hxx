//                                               -*- C++ -*-
/**
 *  @brief Staircase class for handling staircase plots
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
#ifndef OPENTURNS_STAIRCASE_HXX
#define OPENTURNS_STAIRCASE_HXX

#include "DrawableImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Staircase
 *
 * The class describing a staircase plot
 * Instance of Drawable
 */

class OT_API Staircase : public DrawableImplementation
{

  CLASSNAME;

public:


  /** Default onstructor */
  Staircase();

  /** Constructor with parameters */
  explicit Staircase(const NumericalSample & data,
                     const String & legend = "");

  /** Constructor with parameters */
  Staircase(const NumericalSample & data,
            const String & color,
            const String & lineStyle,
            const UnsignedInteger lineWidth,
            const String & pattern,
            const String & legend = "");

  /** Constructor with old parameters */
  Staircase(const NumericalSample & data,
            const String & color,
            const String & lineStyle,
            const String & pattern,
            const String & legend = "");

  /** String converter */
  String __repr__() const;

  /** Draw method */
  String draw() const;

  /** Clone method */
  virtual Staircase * clone() const;

  /** Accessor for pattern */
  String getPattern() const;
  void setPattern(const String style);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check fo data validity */
  virtual void checkData(const NumericalSample & data) const;

private:
  /** Pattern for staircase plotting */
  String pattern_; // should be either "S" or "s"

}; /* class Staircase */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STAIRCASE_HXX */
