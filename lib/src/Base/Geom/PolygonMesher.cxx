//                                               -*- C++ -*-
/**
 *  @brief Polygon meshing algorithm
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
#include "openturns/PolygonMesher.hxx"
#include "openturns/OTconfig.hxx"

#include "earcut.hpp"

namespace mapbox {
namespace util {

template <>
struct nth<0, OT::Point> {
    inline static auto get(const OT::Point &t) {
        return t[0];
    };
};
template <>
struct nth<1, OT::Point> {
    inline static auto get(const OT::Point &t) {
        return t[1];
    };
};

} // namespace util
} // namespace mapbox

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PolygonMesher)
static const Factory<PolygonMesher> Factory_PolygonMesher;


/* Default constructor */
PolygonMesher::PolygonMesher()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
PolygonMesher * PolygonMesher::clone() const
{
  return new PolygonMesher(*this);
}

/* String converter */
String PolygonMesher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PolygonMesher::GetClassName();
  return oss;
}

/* String converter */
String PolygonMesher::__str__(const String & ) const
{
  return __repr__();
}

/* Here is the interface that all derived class must implement */

Mesh PolygonMesher::build(const Sample & points) const
{
  const UnsignedInteger dimension = points.getDimension();
  const UnsignedInteger size = points.getSize();

  if (dimension != 2)
    throw InvalidArgumentException(HERE) << "PolygonMesher expected points of dimension 2, got " << dimension;
  if (size < 3)
    throw InvalidArgumentException(HERE) << "PolygonMesher expected points of size >=3, got " << size;

  // The input points form a polygon line
  std::vector<Point> polyline;
  for (UnsignedInteger i = 0; i < size; ++ i)
    polyline.push_back(points[i]);

  // earcut also allows one to define following lines as holes (unused here)
  std::vector<std::vector<Point> > polygon(1, polyline);

  // Run tessellation
  std::vector<UnsignedInteger> indices(mapbox::earcut<UnsignedInteger>(polygon));
  IndicesCollection simplices(indices.size() / 3, 3);
  for (UnsignedInteger i = 0; i < indices.size(); ++ i)
    simplices(i / 3, i % 3) = indices[i];
  return Mesh(points, simplices);
}

END_NAMESPACE_OPENTURNS

