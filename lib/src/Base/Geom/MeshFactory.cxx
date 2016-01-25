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
#include "MeshFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MeshFactory);

/* Default constructor */
MeshFactory::MeshFactory()
  : TypedInterfaceObject<MeshFactoryImplementation>(new MeshFactoryImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
MeshFactory::MeshFactory(const MeshFactoryImplementation & implementation)
  : TypedInterfaceObject<MeshFactoryImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
MeshFactory::MeshFactory(const Implementation & p_implementation)
  : TypedInterfaceObject<MeshFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
MeshFactory::MeshFactory(MeshFactoryImplementation * p_implementation)
  : TypedInterfaceObject<MeshFactoryImplementation>(p_implementation)
{
  // Nothing to do
}


/* Virtual constructor */
MeshFactory * MeshFactory::clone() const
{
  return new MeshFactory(*this);
}

/* String converter */
String MeshFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MeshFactory::GetClassName()
      << " implementation = " << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String MeshFactory::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

Mesh MeshFactory::build(const Domain & domain) const
{
  return getImplementation()->build(domain);
}

END_NAMESPACE_OPENTURNS
