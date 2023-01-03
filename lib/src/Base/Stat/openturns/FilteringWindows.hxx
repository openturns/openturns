//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a second order model
 *  This is the interface class
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FILTERINGWINDOWS_HXX
#define OPENTURNS_FILTERINGWINDOWS_HXX

#include "openturns/FilteringWindowsImplementation.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FilteringWindowsImplementation
 */

class OT_API FilteringWindows
  : public TypedInterfaceObject<FilteringWindowsImplementation>
{

  CLASSNAME

public:

  /** Default onstructor */
  FilteringWindows();

  /** Copy constructors */
  FilteringWindows(const FilteringWindowsImplementation & implementation);

  /** Constructor from implementation */
  FilteringWindows(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  FilteringWindows(FilteringWindowsImplementation * p_implementation);

#endif

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** functor - evaluation of filter using () */
  Scalar operator()(const Scalar t) const;

} ; /* class FilteringWindows */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FILTERINGWINDOWS_HXX */
