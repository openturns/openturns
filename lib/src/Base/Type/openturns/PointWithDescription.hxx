//                                               -*- C++ -*-
/**
 *  @brief PointWithDescription extends the classical mathematical point with a description of the coordinates
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_POINTWITHDESCRIPTION_HXX
#define OPENTURNS_POINTWITHDESCRIPTION_HXX

#include "openturns/Point.hxx"
#include "openturns/Description.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointWithDescription
 *
 * PointWithDescription extends the classical mathematical point with a description of the coordinates
 */

class OT_API PointWithDescription
  : public Point
{
  CLASSNAME

public:

  /** Default constructor */
  PointWithDescription();

  /** Constructor with size */
  explicit PointWithDescription(const UnsignedInteger size,
                                const Scalar value = 0.0);

  /** Constructor from a collection */
  PointWithDescription(const Collection<Scalar> & coll);

  /** Constructor from a base class */
  PointWithDescription(const Pointer<Point> & p_base);

  /** Virtual constructor */
  virtual PointWithDescription * clone() const;

  /** Destructor */
  virtual ~PointWithDescription() throw();

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** The description of all components */
  Description description_;

}; /* class PointWithDescription */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTWITHDESCRIPTION_HXX */
