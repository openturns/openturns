//                                               -*- C++ -*-
/**
 *  @brief Meshing algorithm for intervals
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
#include "openturns/IntervalMesher.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IntervalMesher);
static const Factory<IntervalMesher> RegisteredFactory;


/* Default constructor */
IntervalMesher::IntervalMesher()
  : PersistentObject()
  , discretization_(0)
{
  // Nothing to do
}

/* Parameter constructor */
IntervalMesher::IntervalMesher(const Indices & discretization)
  : PersistentObject()
  , discretization_(discretization)
{
  // Check if the discretization is valid
  for (UnsignedInteger i = 0; i < discretization.getSize(); ++i)
    if (discretization[i] == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive discretization, got " << discretization;
}

/* Virtual constructor */
IntervalMesher * IntervalMesher::clone() const
{
  return new IntervalMesher(*this);
}

/* String converter */
String IntervalMesher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IntervalMesher::GetClassName()
      << " discretization=" << discretization_;
  return oss;
}

/* String converter */
String IntervalMesher::__str__(const String & offset) const
{
  return __repr__();
}

/* Discretization accessors */
void IntervalMesher::setDiscretization(const Indices & discretization)
{
  // At least one slice per dimension
  for (UnsignedInteger i = 0; i < discretization.getSize(); ++i)
    if (discretization_[i] == 0) throw InvalidArgumentException(HERE) << "Error: expected positive values for the discretization, here discretization[" << i << "]=" << discretization[i];
  discretization_ = discretization;
}

Indices IntervalMesher::getDiscretization() const
{
  return discretization_;
}

/* Here is the interface that all derived class must implement */

Mesh IntervalMesher::build(const Interval & interval) const
{
  const UnsignedInteger dimension(interval.getDimension());
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for intervals of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  if (dimension > 3) throw NotYetImplementedException(HERE) << "In IntervalMesher::build(const Interval & interval, const Bool diamond) const";

  Mesh result;
  // Waiting for a generic implementation in higher dimension
  if (dimension == 1)
  {
    // We must insure that the interval bounds will be within the vertices
    const UnsignedInteger n(discretization_[0]);
    NumericalSample vertices(n + 1, 1);
    // First the vertices
    const NumericalScalar a(interval.getLowerBound()[0]);
    const NumericalScalar b(interval.getUpperBound()[0]);
    vertices[0][0] = a;
    vertices[n][0] = b;
    for (UnsignedInteger i = 1; i < n; ++i) vertices[i][0] = (i * b + (n - i) * a) / n;
    // Second the simplices
    Mesh::IndicesCollection simplices(n);
    Indices simplex(2);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      simplex[0] = i;
      simplex[1] = i + 1;
      simplices[i] = simplex;
    } // i
    result = Mesh(vertices, simplices);
  } // dimension == 1
  if (dimension == 2)
  {
    const UnsignedInteger m(discretization_[0]);
    const UnsignedInteger n(discretization_[1]);
    // First the vertices
    NumericalSample vertices(0, 2);
    NumericalPoint point(2);
    for (UnsignedInteger j = 0; j <= n; ++j)
    {
      point[1] = ((n - j) * interval.getLowerBound()[1] + j * interval.getUpperBound()[1]) / n;
      for (UnsignedInteger i = 0; i <= m; ++i)
      {
        point[0] = ((m - i) * interval.getLowerBound()[0] + i * interval.getUpperBound()[0]) / m;
        vertices.add(point);
      } // i
    } // j
    // Second the simplices
    Mesh::IndicesCollection simplices(0, Indices(3));
    UnsignedInteger vertexIndex(0);
    Indices index(3);
    for (UnsignedInteger j = 0; j < n; ++j)
    {
      for (UnsignedInteger i = 0; i < m; ++i)
      {
        index[0] = vertexIndex;
        index[1] = vertexIndex + 1;
        index[2] = vertexIndex + 1 + m;
        simplices.add(index);
        index[0] = vertexIndex + 2 + m;
        simplices.add(index);
        ++vertexIndex;
      } // i
      ++vertexIndex;
    } // j
    result = Mesh(vertices, simplices);
  } // dimension == 2
  if (dimension == 3)
  {
    const UnsignedInteger m(discretization_[0]);
    const UnsignedInteger n(discretization_[1]);
    const UnsignedInteger p(discretization_[2]);
    // First the vertices
    NumericalSample vertices(0, 3);
    NumericalPoint point(3);
    for (UnsignedInteger k = 0; k <= p; ++k)
    {
      point[2] = ((p - k) * interval.getLowerBound()[2] + k * interval.getUpperBound()[2]) / p;
      for (UnsignedInteger j = 0; j <= n; ++j)
      {
        point[1] = ((n - j) * interval.getLowerBound()[1] + j * interval.getUpperBound()[1]) / n;
        for (UnsignedInteger i = 0; i <= m; ++i)
        {
          point[0] = ((m - i) * interval.getLowerBound()[0] + i * interval.getUpperBound()[0]) / m;
          vertices.add(point);
        } // i
      } // j
    } // k
    // Second the simplices
    Mesh::IndicesCollection simplices(0, Indices(4));
    UnsignedInteger vertexIndex(0);
    const UnsignedInteger mp1(m + 1);
    const UnsignedInteger np1(n + 1);
    const UnsignedInteger mp1np1(mp1 * np1);
    Indices index(4);
    for (UnsignedInteger k = 0; k < p; ++k)
    {
      for (UnsignedInteger j = 0; j < n; ++j)
      {
        for (UnsignedInteger i = 0; i < m; ++i)
        {
          // The current cube has vertices indices
          // A = (     i*dx,      j*dy,      k*dz) -> vertexIndex
          // B = (dx + i*dx,      j*dy,      k*dz) -> vertexIndex + 1
          // C = (     i*dx, dy + j*dy,      k*dz) -> vertexIndex +     mp1
          // D = (dx + i*dx, dy + j*dy,      k*dz) -> vertexIndex + 1 + mp1
          // E = (     i*dx,      j*dy, dz + k*dz) -> vertexIndex +           mp1 * np1
          // F = (dx + i*dx,      j*dy, dz + k*dz) -> vertexIndex + 1 +       mp1 * np1
          // G = (     i*dx, dy + j*dy, dz + k*dz) -> vertexIndex +     mp1 + mp1 * np1
          // H = (dx + i*dx, dy + j*dy, dz + k*dz) -> vertexIndex + 1 + mp1 + mp1 * np1
          // The 6 simplices of the Kuhn triangulation are the shortest paths
          // from A to H
          // 1: ABDH
          index[0] = vertexIndex;
          index[1] = vertexIndex + 1;
          index[2] = vertexIndex + 1 + mp1;
          index[3] = vertexIndex + 1 + mp1 + mp1np1;
          simplices.add(index);
          // 2: ABFH
          index[2] = vertexIndex + 1 + mp1np1;
          simplices.add(index);
          // 3: AEFH
          index[1] = vertexIndex + mp1np1;
          simplices.add(index);
          // 4: AEGH
          index[2] = vertexIndex + mp1 + mp1np1;
          simplices.add(index);
          // 5: ACGH
          index[1] = vertexIndex + mp1;
          simplices.add(index);
          // 6: ACDH
          index[2] = vertexIndex + 1 + mp1;
          simplices.add(index);
          ++vertexIndex;
        } // i
        ++vertexIndex;
      } // j
      vertexIndex += mp1;
    } // k
    result = Mesh(vertices, simplices);
  } // dimension == 3
  return result;
}

END_NAMESPACE_OPENTURNS
