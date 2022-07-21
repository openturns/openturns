//                                               -*- C++ -*-
/**
 *  @brief Cloud class for cloud plots
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
#ifndef OPENTURNS_CLOUD_HXX
#define OPENTURNS_CLOUD_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DrawableImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Cloud
 *
 * The class describing a cloud plot
 * Instance of Drawable
 */

class OT_API Cloud : public DrawableImplementation
{
  CLASSNAME
public:

  typedef Collection<Complex>               ComplexCollection;

  /** Default constructor */
  Cloud();

  /** Default constructor */
  explicit Cloud(const Sample & data,
                 const String & legend = "");

  /** Constructor from complex numbers */
  explicit Cloud(const ComplexCollection & data,
                 const String & legend = "");

  /** Constructor from 2 data sets */
  Cloud(const Sample & dataX,
        const Sample & dataY,
        const String & legend = "");

  /** Constructor from 2 data sets */
  Cloud(const Point & dataX,
        const Point & dataY,
        const String & legend = "");

  /** Constructor with parameters */
  Cloud(const Sample & data,
        const String & color,
        const String & pointStyle,
        const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Draw method */
  String draw() const override;

  /** Clone method */
  Cloud * clone() const override;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check for data validity */
  void checkData(const Sample & data) const override;

}; /* class Cloud */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_CLOUD_HXX */
