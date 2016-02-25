//                                               -*- C++ -*-
/**
 *  @brief Top-level class for meshing algorithms
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
 *  Id:      $Id$
 */
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MeshFactoryImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MeshFactoryImplementation);
static const Factory<MeshFactoryImplementation> RegisteredFactory;

/* Default constructor */
MeshFactoryImplementation::MeshFactoryImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
MeshFactoryImplementation * MeshFactoryImplementation::clone() const
{
  return new MeshFactoryImplementation(*this);
}

/* String converter */
String MeshFactoryImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MeshFactoryImplementation::GetClassName();
  return oss;
}

/* String converter */
String MeshFactoryImplementation::__str__(const String & offset) const
{
  return this->getClassName();
}

/* Here is the interface that all derived class must implement */

Mesh MeshFactoryImplementation::build(const Domain & domain) const
{
  throw NotYetImplementedException(HERE) << "In MeshFactoryImplementation::build(const Domain & domain) const";
}

END_NAMESPACE_OPENTURNS
