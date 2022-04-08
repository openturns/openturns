//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a second order model
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
#ifndef OPENTURNS_FILTERINGWINDOWSIMPLEMENTATION_HXX
#define OPENTURNS_FILTERINGWINDOWSIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FilteringWindowsImplementation
 */

class OT_API FilteringWindowsImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  typedef Pointer<FilteringWindowsImplementation>    Implementation;

  /** Default constructor without parameters */
  FilteringWindowsImplementation();

  /** Virtual copy constructor */
  FilteringWindowsImplementation * clone() const override;

  virtual Scalar operator()(const Scalar t) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

} ; /* class FilteringWindowsImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FILTERINWINDOWSIMPLEMENTATION_HXX */
