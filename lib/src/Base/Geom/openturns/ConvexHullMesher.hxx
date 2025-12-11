//                                               -*- C++ -*-
/**
 *  @brief Meshing algorithm for points
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CONVEXHULLMESHER_HXX
#define OPENTURNS_CONVEXHULLMESHER_HXX

#include "openturns/Mesh.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ConvexHullMesher
 */
class OT_API ConvexHullMesher
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  ConvexHullMesher();

   /** Virtual constructor */
  ConvexHullMesher * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */
  virtual Mesh build(const Sample & points) const;

protected:

private:

}; /* class ConvexHullMesher */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTERVALMESHER_HXX */
