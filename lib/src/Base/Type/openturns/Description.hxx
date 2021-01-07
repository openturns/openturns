//                                               -*- C++ -*-
/**
 *  @brief Description is a collection of string for human usage
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DESCRIPTION_HXX
#define OPENTURNS_DESCRIPTION_HXX

#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @typedef Description
 *
 * Description is a collection of string for human usage
 */

class OT_API Description :
  public PersistentCollection<String>
{
  CLASSNAME
public:
  typedef PersistentCollection<String> InternalType;

  /** Default constructor */
  Description();

  /** Constructor with size */
  explicit Description(const UnsignedInteger size);

  /** Constructor with size and default value */
  Description(const UnsignedInteger size,
              const String & value);

  /** Constructor from a collection */
  Description(const Collection<String> & coll);

  /** Initializer list constructor */
  Description(std::initializer_list<String> initList);

  /** Check if the content is blank */
  Bool isBlank() const;

  /** Destructor */
  virtual ~Description();

#ifdef SWIG
  /** @copydoc Object::__repr__() const */
  String __repr__() const override;

  /** @copydoc Object::__str__() const */
  String __str__(const String & offset = "") const override;
#endif

  void sort();

  /** Build a default description */
  static Description BuildDefault(const UnsignedInteger dimension,
                                  const String & prefix = "Component");


}; /* class Description */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DESCRIPTION_HXX */
