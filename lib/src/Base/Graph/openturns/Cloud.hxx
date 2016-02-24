//                                               -*- C++ -*-
/**
 *  @brief Cloud class for cloud plots
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
#ifndef OPENTURNS_CLOUD_HXX
#define OPENTURNS_CLOUD_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DrawableImplementation.hxx"
#include "openturns/NumericalSample.hxx"
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
  CLASSNAME;
public:

  typedef Collection<NumericalComplex>               NumericalComplexCollection;

  /** Default constructor */
  explicit Cloud(const NumericalSample & data,
                 const String & legend = "");

  /** Constructor from complex numbers */
  explicit Cloud(const NumericalComplexCollection & data,
                 const String & legend = "");

  /** Contructor from 2 data sets */
  Cloud(const NumericalSample & dataX,
        const NumericalSample & dataY,
        const String & legend = "");

  /** Contructor from 2 data sets */
  Cloud(const NumericalPoint & dataX,
        const NumericalPoint & dataY,
        const String & legend = "");

  /** Constructor with parameters */
  Cloud(const NumericalSample & data,
        const String & color,
        const String & pointStyle,
        const String & legend = "");

  /** String converter */
  String __repr__() const;

  /** Draw method */
  String draw() const;

  /** Clone method */
  virtual Cloud * clone() const;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check fo data validity */
  virtual void checkData(const NumericalSample & data) const;

private:

  Cloud() {};
  friend class Factory<Cloud>;

}; /* class Cloud */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_CLOUD_HXX */
