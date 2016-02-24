//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
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
#ifndef OPENTURNS_MESHFACTORY_HXX
#define OPENTURNS_MESHFACTORY_HXX

#include "openturns/MeshFactoryImplementation.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class  MeshFactory
 */
class OT_API MeshFactory
  : public TypedInterfaceObject<MeshFactoryImplementation>
{
  CLASSNAME;
public:

  typedef MeshFactoryImplementation::Implementation    Implementation;

  /** Default constructor */
  MeshFactory();

  /** Copy constructors */
  MeshFactory(const MeshFactoryImplementation & implementation);

  /** Constructor from implementation */
  MeshFactory(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  MeshFactory(MeshFactoryImplementation * p_implementation);
#endif

  /** Virtual constructor */
  virtual  MeshFactory * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */

  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */
  /** Build a mesh based on a domain */
  virtual Mesh build(const Domain & domain) const;

}; /* class  MeshFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MESHFACTORY_HXX */
