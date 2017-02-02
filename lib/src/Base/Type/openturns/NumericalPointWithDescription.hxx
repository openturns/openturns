//                                               -*- C++ -*-
/**
 *  @brief NumericalPointWithDescription extends the classical mathematical point with a description of the coordinates
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_NUMERICALPOINTWITHDESCRIPTION_HXX
#define OPENTURNS_NUMERICALPOINTWITHDESCRIPTION_HXX

#include "openturns/NumericalPoint.hxx"
#include "openturns/Description.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NumericalPointWithDescription
 *
 * NumericalPointWithDescription extends the classical mathematical point with a description of the coordinates
 */

class OT_API NumericalPointWithDescription
  : public NumericalPoint
{
  CLASSNAME;

public:

  /** Default constructor */
  NumericalPointWithDescription();

  /** Constructor with size */
  explicit NumericalPointWithDescription(const UnsignedInteger size,
                                         const NumericalScalar value = 0.0);

  /** Constructor from a collection */
  NumericalPointWithDescription(const Collection<NumericalScalar> & coll);

  /** Constructor from a base class */
  NumericalPointWithDescription(const Pointer<NumericalPoint> & p_base);

  /** Virtual constructor */
  virtual NumericalPointWithDescription * clone() const;

  /** Destructor */
  virtual ~NumericalPointWithDescription() throw();

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

}; /* class NumericalPointWithDescription */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NUMERICALPOINTWITHDESCRIPTION_HXX */
