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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ConvexHullMesher.hxx"
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_HAVE_QHULL
#include "libqhull_r/qhull_ra.h"
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConvexHullMesher)
static const Factory<ConvexHullMesher> Factory_ConvexHullMesher;


/* Default constructor */
ConvexHullMesher::ConvexHullMesher()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
ConvexHullMesher * ConvexHullMesher::clone() const
{
  return new ConvexHullMesher(*this);
}

/* String converter */
String ConvexHullMesher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConvexHullMesher::GetClassName();
  return oss;
}

/* String converter */
String ConvexHullMesher::__str__(const String & ) const
{
  return __repr__();
}

/* Here is the interface that all derived class must implement */

Mesh ConvexHullMesher::build(const Sample & points) const
{
#ifdef OPENTURNS_HAVE_QHULL
  const UnsignedInteger dimension = points.getDimension();
  const UnsignedInteger size = points.getSize();

  if (dimension < 2)
    throw InvalidArgumentException(HERE) << "ConvexHullMesher expected points of dimension >=2, got " << dimension;

  // Create qhull context
  qhT qh_qh;  
  qhT *qh = &qh_qh;
  QHULL_LIB_CHECK
  qh_zero(qh, stderr);

  // Run Qhull
  const String qhull_cmd("qhull Qt Qx");
  int rc = qh_new_qhull(qh, dimension, size,
                        const_cast<Scalar*>(points.getImplementation()->data()),
                        False, /* ismalloc */
                        const_cast<char*>(qhull_cmd.c_str()), /* Qhull options: triangulated hull + exact */
                        NULL, NULL);

  if (rc != 0)
  {
    qh_freeqhull(qh, !qh_ALL);
    throw InternalException(HERE) << "qh_new_qhull exit code: " << rc;
  }

  // build the vertices
  Indices inputIndexToHullIndex(size, size);
  Sample vertices(0, dimension);
  vertexT *vertex = NULL, **vertexp = NULL;
  UnsignedInteger i = 0;
  FORALLvertices
  {
    if (!vertex->deleted)
    {
      Point point(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++ j)
        point[j] = vertex->point[j];
      vertices.add(point);

      // qh_pointid gives indices wrt the original input sample
      const int inputIdx = qh_pointid(qh, vertex->point);
      inputIndexToHullIndex[inputIdx] = i;
      ++ i;
    }
  }

  // build the faces
  facetT *facet = NULL;
  UnsignedInteger facetCount = 0;
  FORALLfacets
  {
    if (!facet->upperdelaunay) // For 2D, this removes the "top" duplicate
      ++ facetCount;
  }
  IndicesCollection simplices(facetCount, dimension + 1);
  UnsignedInteger iS = 0;
  FORALLfacets
  {
    if (!facet->upperdelaunay) /* skip upper facets in 2D */
    {
      UnsignedInteger j = 0;
      FOREACHvertex_(facet->vertices)
      {
        const int hullIdx = qh_pointid(qh, vertex->point);
        simplices(iS, j) = inputIndexToHullIndex[hullIdx];
        ++ j;
      }

      // repeat the last value to mark the intrinsic dimension
      simplices(iS, dimension) = simplices(iS, dimension - 1);
    }
    ++ iS;
  }

  // cleanup
  qh_freeqhull(qh, !qh_ALL);
  int curlong = 0, totlong = 0;
  qh_memfreeshort(qh, &curlong, &totlong);
  if (curlong || totlong)
    throw InternalException(HERE) << "qh_memfreeshort: did not free " << totlong <<" bytes (" << curlong << " blocks)";

  return Mesh(vertices, simplices);
#else
  (void)points;
  throw NotYetImplementedException(HERE) << "No QHull support";
#endif
}

END_NAMESPACE_OPENTURNS

