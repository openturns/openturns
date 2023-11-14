//                                               -*- C++ -*-
/**
 *  @brief Staircase class for handling staircase plots
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_STAIRCASE_HXX
#define OPENTURNS_STAIRCASE_HXX

#include "openturns/DrawableImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Staircase
 *
 * The class describing a staircase plot
 * Instance of Drawable
 */

class OT_API Staircase : public DrawableImplementation
{

  CLASSNAME

public:


  /** Default onstructor */
  Staircase();

  /** Constructor with parameters */
  explicit Staircase(const Sample & data,
                     const String & legend = "");

  /** Constructor with parameters */
  Staircase(const Sample & data,
            const String & color,
            const String & lineStyle,
            const Scalar lineWidth,
            const String & pattern,
            const String & legend = "");

  /** Constructor with old parameters */
  Staircase(const Sample & data,
            const String & color,
            const String & lineStyle,
            const String & pattern,
            const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Clone method */
  Staircase * clone() const override;

  /** Accessor for pattern */
  String getPattern() const override;
  void setPattern(const String style) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check fo data validity */
  void checkData(const Sample & data) const override;

private:
  /** Pattern for staircase plotting */
  String pattern_; // should be either "S" or "s"

}; /* class Staircase */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STAIRCASE_HXX */
