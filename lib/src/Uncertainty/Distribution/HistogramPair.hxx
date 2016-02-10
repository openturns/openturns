//                                               -*- C++ -*-
/**
 *  @brief The HistogramPair class.
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
#ifndef OPENTURNS_HISTOGRAMPAIR_HXX
#define OPENTURNS_HISTOGRAMPAIR_HXX

#include "OTprivate.hxx"
#include "StorageManager.hxx"
#include "PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class HistogramPair
 *
 * The HistogramPair class.
 */
class OT_API HistogramPair
  : public PersistentObject
{
  CLASSNAME;
public:

  /** Default constructor */
  HistogramPair();

  /** Parameters constructor */
  HistogramPair(const NumericalScalar width,
                const NumericalScalar height);

  /** Virtual constructor */
  virtual HistogramPair * clone() const;

  Bool operator == (const HistogramPair & other) const;

  void setWidth(const NumericalScalar width);

  void setHeight(const NumericalScalar height);

  NumericalScalar getWidth() const;

  NumericalScalar getHeight() const;

  NumericalScalar getSurface() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  virtual String __repr__() const;

private:

  /** Width of the bin */
  NumericalScalar width_;
  /** Height of the bin */
  NumericalScalar height_;
  /** Surface of the bin */
  NumericalScalar surface_;

}; /* end class HistogramPair */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HISTOGRAMPAIR_HXX */
