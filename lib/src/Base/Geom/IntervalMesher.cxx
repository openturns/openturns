//                                               -*- C++ -*-
/**
 *  @brief Meshing algorithm for intervals
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IntervalMesher.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/KPermutations.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IntervalMesher)
static const Factory<IntervalMesher> Factory_IntervalMesher;


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
    if (!(discretization[i] > 0)) throw InvalidArgumentException(HERE) << "Error: expected a positive discretization, got " << discretization;
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
String IntervalMesher::__str__(const String & ) const
{
  return __repr__();
}

/* Discretization accessors */
void IntervalMesher::setDiscretization(const Indices & discretization)
{
  // At least one slice per dimension
  for (UnsignedInteger i = 0; i < discretization.getSize(); ++i)
    if (!(discretization_[i] > 0)) throw InvalidArgumentException(HERE) << "Error: expected positive values for the discretization, here discretization[" << i << "]=" << discretization[i];
  discretization_ = discretization;
}

Indices IntervalMesher::getDiscretization() const
{
  return discretization_;
}

/* Here is the interface that all derived class must implement */

Mesh IntervalMesher::build(const Interval & interval,
                           const Bool diamond) const
{
  const UnsignedInteger dimension = interval.getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for intervals of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  const Point lowerBound(interval.getLowerBound());
  const Point upperBound(interval.getUpperBound());

  // Waiting for a generic implementation in higher dimension
  if (dimension == 1)
  {
    // We must insure that the interval bounds will be within the vertices
    const UnsignedInteger n = diamond ? 2 * discretization_[0] - 1 : discretization_[0];
    Sample vertices(n + 1, 1);
    // First the vertices
    const Scalar a = lowerBound[0];
    const Scalar b = upperBound[0];
    vertices(0, 0) = a;
    vertices(n, 0) = b;
    for (UnsignedInteger i = 1; i < n; ++i) vertices(i, 0) = (i * b + (n - i) * a) / n;
    // Second the simplices
    IndicesCollection simplices(n, 2);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      simplices(i, 0) = i;
      simplices(i, 1) = i + 1;
    } // i
    return Mesh(vertices, simplices);
  } // dimension == 1
  if (dimension == 2)
  {
    const UnsignedInteger m = discretization_[0];
    const UnsignedInteger n = discretization_[1];
    // First the vertices
    Sample vertices((m + 1) * (n + 1) + (diamond ? m * n : 0), 2);
    Point discretizedX(m + 1);
    for (UnsignedInteger i = 0; i <= m; ++i)
      discretizedX[i] = ((m - i) * lowerBound[0] + i * upperBound[0]) / m;
    Point discretizedY(n + 1);
    for (UnsignedInteger j = 0; j <= n; ++j)
      discretizedY[j] = ((n - j) * lowerBound[1] + j * upperBound[1]) / n;

    UnsignedInteger vertexIndex = 0;
    for (UnsignedInteger j = 0; j <= n; ++j)
    {
      for (UnsignedInteger i = 0; i <= m; ++i, ++vertexIndex)
      {
        vertices(vertexIndex, 0) = discretizedX[i];
        vertices(vertexIndex, 1) = discretizedY[j];
      } // i
    } // j
    // Second the simplices
    IndicesCollection simplices((diamond ? 4 : 2 ) * m * n, 3);
    UnsignedInteger cellIndex = 0;
    UnsignedInteger simplexIndex = 0;
    for (UnsignedInteger j = 0; j < n; ++j)
    {
      for (UnsignedInteger i = 0; i < m; ++i)
      {
        // The current cell is
        // c--d
        // |  |
        // a--b
        const UnsignedInteger a = cellIndex;
        const UnsignedInteger b = cellIndex + 1;
        const UnsignedInteger c = cellIndex + 1 + m;
        const UnsignedInteger d = cellIndex + 2 + m;
        if (diamond)
        {
          vertices(vertexIndex, 0) = (vertices(a, 0) + vertices(b, 0) + vertices(c, 0) + vertices(d, 0)) * 0.25;
          vertices(vertexIndex, 1) = (vertices(a, 1) + vertices(b, 1) + vertices(c, 1) + vertices(d, 1)) * 0.25;
          simplices(simplexIndex, 0) = a;
          simplices(simplexIndex, 1) = b;
          simplices(simplexIndex, 2) = vertexIndex;
          ++simplexIndex;
          simplices(simplexIndex, 0) = b;
          simplices(simplexIndex, 1) = d;
          simplices(simplexIndex, 2) = vertexIndex;
          ++simplexIndex;
          simplices(simplexIndex, 0) = d;
          simplices(simplexIndex, 1) = c;
          simplices(simplexIndex, 2) = vertexIndex;
          ++simplexIndex;
          simplices(simplexIndex, 0) = c;
          simplices(simplexIndex, 1) = a;
          simplices(simplexIndex, 2) = vertexIndex;
          ++simplexIndex;
          ++vertexIndex;
        }
        else
        {
          simplices(simplexIndex, 0) = a;
          simplices(simplexIndex, 1) = b;
          simplices(simplexIndex, 2) = c;
          ++simplexIndex;
          simplices(simplexIndex, 0) = b;
          simplices(simplexIndex, 1) = d;
          simplices(simplexIndex, 2) = c;
          ++simplexIndex;
        }
        ++cellIndex;
      } // i
      ++cellIndex;
    } // j
    return Mesh(vertices, simplices);
  } // dimension == 2
  if (dimension == 3)
  {
    // FIXME: https://github.com/openturns/openturns/issues/1670
    if (diamond)
      throw NotYetImplementedException(HERE) << "In IntervalMesher::build with 3-d/diamond=true";

    const UnsignedInteger m = discretization_[0];
    const UnsignedInteger n = discretization_[1];
    const UnsignedInteger p = discretization_[2];
    // First the vertices
    Sample vertices((m + 1) * (n + 1) * (p + 1) + (diamond ? m * n * p : 0), 3);
    Point discretizedX(m + 1);
    for (UnsignedInteger i = 0; i <= m; ++i)
      discretizedX[i] = ((m - i) * lowerBound[0] + i * upperBound[0]) / m;
    Point discretizedY(n + 1);
    for (UnsignedInteger j = 0; j <= n; ++j)
      discretizedY[j] = ((n - j) * lowerBound[1] + j * upperBound[1]) / n;
    Point discretizedZ(p + 1);
    for (UnsignedInteger k = 0; k <= p; ++k)
      discretizedZ[k] = ((p - k) * lowerBound[2] + k * upperBound[2]) / p;

    UnsignedInteger vertexIndex = 0;
    for (UnsignedInteger k = 0; k <= p; ++k)
    {
      for (UnsignedInteger j = 0; j <= n; ++j)
      {
        for (UnsignedInteger i = 0; i <= m; ++i, ++vertexIndex)
        {
          vertices(vertexIndex, 0) = discretizedX[i];
          vertices(vertexIndex, 1) = discretizedY[j];
          vertices(vertexIndex, 2) = discretizedZ[k];
        } // i
      } // j
    } // k
    // Second the simplices
    IndicesCollection simplices( (diamond ? 24 : 6) * m * n * p, 4);
    const UnsignedInteger mp1 = m + 1;
    const UnsignedInteger np1 = n + 1;
    const UnsignedInteger mp1np1 = mp1 * np1;
    UnsignedInteger simplexIndex = 0;
    UnsignedInteger cellIndex = 0;
    for (UnsignedInteger k = 0; k < p; ++k)
    {
      for (UnsignedInteger j = 0; j < n; ++j)
      {
        for (UnsignedInteger i = 0; i < m; ++i)
        {
          // The current cube has vertices indices
          // A = (     i*dx,      j*dy,      k*dz) -> cellIndex
          // B = (dx + i*dx,      j*dy,      k*dz) -> cellIndex + 1
          // C = (     i*dx, dy + j*dy,      k*dz) -> cellIndex +     mp1
          // D = (dx + i*dx, dy + j*dy,      k*dz) -> cellIndex + 1 + mp1
          // E = (     i*dx,      j*dy, dz + k*dz) -> cellIndex +           mp1 * np1
          // F = (dx + i*dx,      j*dy, dz + k*dz) -> cellIndex + 1 +       mp1 * np1
          // G = (     i*dx, dy + j*dy, dz + k*dz) -> cellIndex +     mp1 + mp1 * np1
          // H = (dx + i*dx, dy + j*dy, dz + k*dz) -> cellIndex + 1 + mp1 + mp1 * np1
          // Its faces are
          //   G----H
          //  /|   /|
          // E----F |
          // | C--|-D
          // |/   |/
          // A----B
          // ABDC/EFHG/ACGE/BDHF/ABFE/CDHG
          const UnsignedInteger a = cellIndex;
          const UnsignedInteger b = cellIndex + 1;
          const UnsignedInteger c = cellIndex + mp1;
          const UnsignedInteger d = cellIndex + 1 + mp1;
          const UnsignedInteger e = cellIndex + mp1np1;
          const UnsignedInteger f = cellIndex + 1 + mp1np1;
          const UnsignedInteger g = cellIndex + mp1 + mp1np1;
          const UnsignedInteger h = cellIndex + 1 + mp1 + mp1np1;
          if (diamond)
          {
            // Center of the cube (shortcut I)
            const UnsignedInteger centerIndex = vertexIndex;
            vertices(centerIndex, 0) = (vertices(a, 0) + vertices(b, 0) + vertices(c, 0) + vertices(d, 0) + vertices(e, 0) + vertices(f, 0) + vertices(g, 0) + vertices(h, 0)) * 0.125;
            vertices(centerIndex, 1) = (vertices(a, 1) + vertices(b, 1) + vertices(c, 1) + vertices(d, 1) + vertices(e, 1) + vertices(f, 1) + vertices(g, 1) + vertices(h, 1)) * 0.125;
            vertices(centerIndex, 2) = (vertices(a, 2) + vertices(b, 2) + vertices(c, 2) + vertices(d, 2) + vertices(e, 2) + vertices(f, 2) + vertices(g, 2) + vertices(h, 2)) * 0.125;

            const UnsignedInteger centerABDCIndex = vertexIndex + 1;
            vertices(centerABDCIndex, 0) = (vertices(a, 0) + vertices(b, 0) + vertices(c, 0) + vertices(d, 0)) * 0.25;
            vertices(centerABDCIndex, 1) = (vertices(a, 1) + vertices(b, 1) + vertices(c, 1) + vertices(d, 1)) * 0.25;
            vertices(centerABDCIndex, 2) = (vertices(a, 2) + vertices(b, 2) + vertices(c, 2) + vertices(d, 2)) * 0.25;
            // ABDC->c*BAI/c*DBI/c*CDI/c*ACI
            simplices(simplexIndex, 0) = centerABDCIndex;
            simplices(simplexIndex, 1) = b;
            simplices(simplexIndex, 2) = a;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerABDCIndex;
            simplices(simplexIndex, 1) = d;
            simplices(simplexIndex, 2) = b;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerABDCIndex;
            simplices(simplexIndex, 1) = c;
            simplices(simplexIndex, 2) = d;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerABDCIndex;
            simplices(simplexIndex, 1) = a;
            simplices(simplexIndex, 2) = c;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;

            const UnsignedInteger centerEFHGIndex = vertexIndex + 2;
            vertices(centerEFHGIndex, 0) = (vertices(e, 0) + vertices(f, 0) + vertices(g, 0) + vertices(h, 0)) * 0.25;
            vertices(centerEFHGIndex, 1) = (vertices(e, 1) + vertices(f, 1) + vertices(g, 1) + vertices(h, 1)) * 0.25;
            vertices(centerEFHGIndex, 2) = (vertices(e, 2) + vertices(f, 2) + vertices(g, 2) + vertices(h, 2)) * 0.25;
            // EFHG->c*EFI/c*FHI/c*HGI/c*GEI
            simplices(simplexIndex, 0) = centerEFHGIndex;
            simplices(simplexIndex, 1) = e;
            simplices(simplexIndex, 2) = f;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerEFHGIndex;
            simplices(simplexIndex, 1) = f;
            simplices(simplexIndex, 2) = h;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerEFHGIndex;
            simplices(simplexIndex, 1) = h;
            simplices(simplexIndex, 2) = g;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerEFHGIndex;
            simplices(simplexIndex, 1) = g;
            simplices(simplexIndex, 2) = e;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;

            const UnsignedInteger centerACGEIndex = vertexIndex + 3;
            vertices(centerACGEIndex, 0) = (vertices(a, 0) + vertices(c, 0) + vertices(e, 0) + vertices(g, 0)) * 0.25;
            vertices(centerACGEIndex, 1) = (vertices(a, 1) + vertices(c, 1) + vertices(e, 1) + vertices(g, 1)) * 0.25;
            vertices(centerACGEIndex, 2) = (vertices(a, 2) + vertices(c, 2) + vertices(e, 2) + vertices(g, 2)) * 0.25;
            // ACGE->c*CAI/c*GCI/c*EGI/c*AEI
            simplices(simplexIndex, 0) = centerACGEIndex;
            simplices(simplexIndex, 1) = c;
            simplices(simplexIndex, 2) = a;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerACGEIndex;
            simplices(simplexIndex, 1) = g;
            simplices(simplexIndex, 2) = c;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerACGEIndex;
            simplices(simplexIndex, 1) = e;
            simplices(simplexIndex, 2) = g;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerACGEIndex;
            simplices(simplexIndex, 1) = a;
            simplices(simplexIndex, 2) = e;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;

            const UnsignedInteger centerBDHFIndex = vertexIndex + 4;
            vertices(centerBDHFIndex, 0) = (vertices(b, 0) + vertices(d, 0) + vertices(f, 0) + vertices(h, 0)) * 0.25;
            vertices(centerBDHFIndex, 1) = (vertices(b, 1) + vertices(d, 1) + vertices(f, 1) + vertices(h, 1)) * 0.25;
            vertices(centerBDHFIndex, 2) = (vertices(b, 2) + vertices(d, 2) + vertices(f, 2) + vertices(h, 2)) * 0.25;
            // BDHF->c*BDI/c*DHI/c*HFI/c*FBI
            simplices(simplexIndex, 0) = centerBDHFIndex;
            simplices(simplexIndex, 1) = b;
            simplices(simplexIndex, 2) = d;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerBDHFIndex;
            simplices(simplexIndex, 1) = d;
            simplices(simplexIndex, 2) = h;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerBDHFIndex;
            simplices(simplexIndex, 1) = h;
            simplices(simplexIndex, 2) = f;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerBDHFIndex;
            simplices(simplexIndex, 1) = f;
            simplices(simplexIndex, 2) = b;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;

            const UnsignedInteger centerABFEIndex = vertexIndex + 5;
            vertices(centerABFEIndex, 0) = (vertices(a, 0) + vertices(b, 0) + vertices(e, 0) + vertices(f, 0)) * 0.25;
            vertices(centerABFEIndex, 1) = (vertices(a, 1) + vertices(b, 1) + vertices(e, 1) + vertices(f, 1)) * 0.25;
            vertices(centerABFEIndex, 2) = (vertices(a, 2) + vertices(b, 2) + vertices(e, 2) + vertices(f, 2)) * 0.25;
            // ABFE->c*ABI/c*BFI/c*FEI/c*EAI
            simplices(simplexIndex, 0) = centerABFEIndex;
            simplices(simplexIndex, 1) = a;
            simplices(simplexIndex, 2) = b;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerABFEIndex;
            simplices(simplexIndex, 1) = b;
            simplices(simplexIndex, 2) = f;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerABFEIndex;
            simplices(simplexIndex, 1) = f;
            simplices(simplexIndex, 2) = e;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerABFEIndex;
            simplices(simplexIndex, 1) = e;
            simplices(simplexIndex, 2) = a;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;

            const UnsignedInteger centerCDHGIndex = vertexIndex + 6;
            vertices(centerCDHGIndex, 0) = (vertices(c, 0) + vertices(d, 0) + vertices(g, 0) + vertices(h, 0)) * 0.25;
            vertices(centerCDHGIndex, 1) = (vertices(c, 1) + vertices(d, 1) + vertices(g, 1) + vertices(h, 1)) * 0.25;
            vertices(centerCDHGIndex, 2) = (vertices(c, 2) + vertices(d, 2) + vertices(g, 2) + vertices(h, 2)) * 0.25;
            // CDHG->c*DCI/c*HDI/c*GHI/c*CGI
            simplices(simplexIndex, 0) = centerCDHGIndex;
            simplices(simplexIndex, 1) = d;
            simplices(simplexIndex, 2) = c;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerCDHGIndex;
            simplices(simplexIndex, 1) = h;
            simplices(simplexIndex, 2) = d;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerCDHGIndex;
            simplices(simplexIndex, 1) = g;
            simplices(simplexIndex, 2) = h;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            simplices(simplexIndex, 0) = centerCDHGIndex;
            simplices(simplexIndex, 1) = c;
            simplices(simplexIndex, 2) = g;
            simplices(simplexIndex, 3) = centerIndex;
            ++simplexIndex;
            vertexIndex += 7;
          }
          else
          {
            // The 6 simplices of the Kuhn triangulation are the shortest paths
            // from A to H
            // 1: ABFH
            simplices(simplexIndex, 0) = a;
            simplices(simplexIndex, 1) = b;
            simplices(simplexIndex, 2) = f;
            simplices(simplexIndex, 3) = h;
            ++simplexIndex;
            // 2: ADBH
            simplices(simplexIndex, 0) = a;
            simplices(simplexIndex, 1) = d;
            simplices(simplexIndex, 2) = b;
            simplices(simplexIndex, 3) = h;
            ++simplexIndex;
            // 3: AFEH
            simplices(simplexIndex, 0) = a;
            simplices(simplexIndex, 1) = f;
            simplices(simplexIndex, 2) = e;
            simplices(simplexIndex, 3) = h;
            ++simplexIndex;
            // 4: AEGH
            simplices(simplexIndex, 0) = a;
            simplices(simplexIndex, 1) = e;
            simplices(simplexIndex, 2) = g;
            simplices(simplexIndex, 3) = h;
            ++simplexIndex;
            // 5: AGCH
            simplices(simplexIndex, 0) = a;
            simplices(simplexIndex, 1) = g;
            simplices(simplexIndex, 2) = c;
            simplices(simplexIndex, 3) = h;
            ++simplexIndex;
            // 6: ACDH
            simplices(simplexIndex, 0) = a;
            simplices(simplexIndex, 1) = c;
            simplices(simplexIndex, 2) = d;
            simplices(simplexIndex, 3) = h;
            ++simplexIndex;
          }
          ++cellIndex;
        } // i
        ++cellIndex;
      } // j
      cellIndex += mp1;
    } // k
    return Mesh(vertices, simplices);
  } // dimension == 3
  // Dimension > 3
  if (diamond)
    throw NotYetImplementedException(HERE) << "In IntervalMesher::build with n-d/diamond=true";
  Indices verticesDiscretization(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    verticesDiscretization[i] = discretization_[i] + 1;
  // Generate vertices
  const IndicesCollection allVerticesTuples(Tuples(verticesDiscretization).generate());
  const UnsignedInteger numVertices = allVerticesTuples.getSize();
  Sample vertices(numVertices, dimension);
  for (UnsignedInteger i = 0; i < numVertices; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar s = (1.0 * allVerticesTuples(i, j)) / discretization_[j];
      vertices(i, j) = lowerBound[j] * (1.0 - s) + upperBound[j] * s;
    } // j
  } // i
  // Generate simplices:
  const IndicesCollection allHypercubesTuples(Tuples(discretization_).generate());
  const UnsignedInteger numHypercubes = allHypercubesTuples.getSize();
  const IndicesCollection allSimplicesPermutations(KPermutations(dimension, dimension).generate());
  const UnsignedInteger numSimplicesPermutations = allSimplicesPermutations.getSize();
  const UnsignedInteger numSimplices = numHypercubes * numSimplicesPermutations;
  IndicesCollection simplices(numSimplices, dimension + 1);
  // Reference simplex
  IndicesCollection referenceSimplices(numSimplicesPermutations, dimension + 1);
  Indices base(dimension);
  UnsignedInteger product = 1;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    base[i] = product;
    product *= verticesDiscretization[i];
  }

  // Generate all the increasing sequences of 0 and 1 of size (dimension+1) encoded in base 2
  Indices standardSimplex(dimension + 1, 0);
  for (UnsignedInteger j = 0; j <= dimension; ++j)
  {
    // Interpret these sequences as a binary representation of an integer
    for (UnsignedInteger k = 0; k < j; ++k)
      standardSimplex[j] += 1 << k;
  }
  for (UnsignedInteger i = 0; i < numSimplicesPermutations; ++i)
  {
    for (UnsignedInteger j = 0; j <= dimension; ++j)
    {
      // Translate these sequences into integers using the mixed base verticesDiscretization
      UnsignedInteger component = standardSimplex[j];
      for (UnsignedInteger k = 0; k < dimension; ++k)
      {
        referenceSimplices(i, j) += (component % 2) * base[allSimplicesPermutations(i, k)];
        component /= 2;
      } // k
    } // j
  } // i
  // For each hypercube add the reference simplex and all its permutations with
  // the proper translation
  UnsignedInteger simplexIndex = 0;
  for (UnsignedInteger i = 0; i < numHypercubes; ++i)
  {
    // Compute the translation associated to this hypercube
    UnsignedInteger translation = 0;
    for (UnsignedInteger k = 0; k < dimension; ++k)
      translation += allHypercubesTuples(i, k) * base[k];
    for (UnsignedInteger j = 0; j < numSimplicesPermutations; ++j)
    {
      for (UnsignedInteger k = 0; k <= dimension; ++k)
      {
        simplices(simplexIndex, k) = referenceSimplices(j, k) + translation;
      } // k
      ++simplexIndex;
    } // j
  } // i
  return Mesh(vertices, simplices);
}

END_NAMESPACE_OPENTURNS

