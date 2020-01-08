//                                               -*- C++ -*-
/**
 *  @brief The interface class that implements all process
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
#ifndef OPENTURNS_ARMAFACTORY_HXX
#define OPENTURNS_ARMAFACTORY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/ARMAFactoryImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ARMAFactory
 *
 * The class that implements all processs
 */
class OT_API ARMAFactory
  : public TypedInterfaceObject<ARMAFactoryImplementation>
{
  CLASSNAME

public:

  /* Some typedefs for easy reading */
  typedef ARMAFactoryImplementation::Implementation Implementation;

  /** Default constructor */
  ARMAFactory();

  /** Copy constructors */
  ARMAFactory(const ARMAFactoryImplementation & implementation);


  /** Constructor from implementation */
  ARMAFactory(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  ARMAFactory(ARMAFactoryImplementation * p_implementation);
#endif

  /** Virtual constructor */
  ARMAFactory * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Build method */
  ARMA build(const TimeSeries & timeSeries) const;

  /** Build method */
  ARMA build(const ProcessSample & sample) const;


}; /* class ARMAFactory */
END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ARMAFACTORY_HXX */
