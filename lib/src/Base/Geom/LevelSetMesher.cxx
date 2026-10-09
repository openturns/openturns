//                                               -*- C++ -*-
/**
 *  @brief Meshing algorithm for levelSets
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
#include "openturns/LevelSetMesher.hxx"
#include "openturns/IntervalMesher.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/NearestPointProblem.hxx"
#include "openturns/TranslationFunction.hxx"
#include "openturns/AbdoRackwitz.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/Brent.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/NLopt.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SpecFunc.hxx"
#include <map>
#include <vector>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LevelSetMesher)
static const Factory<LevelSetMesher> Factory_LevelSetMesher;

/* Inside test for the intersection of a collection of level sets */
static Bool IsInsideIntersection(const Collection<ComparisonOperator> & operators,
                                 const Point & levels,
                                 const Sample & values,
                                 const UnsignedInteger vertexIndex)
{
  const UnsignedInteger size = operators.getSize();
  for (UnsignedInteger k = 0; k < size; ++ k)
    if (!operators[k](values(vertexIndex, k), levels[k]))
      return false;
  return true;
}

/* QEF minimizer from Hermite planes: false unless the planes constrain all
 * directions (at least dimension planes, non-singular normal equations).
 * ATA is a SquareMatrix so solveLinearSystem throws on singular systems;
 * the least-squares Matrix variant must not be used here as it silently
 * returns a minimum-norm solution instead. */
static Bool ComputeQEFMinimizer(const UnsignedInteger dimension,
                                const Collection<Point> & crossings,
                                const Collection<Point> & normals,
                                const Indices & planes,
                                Point & minimizer)
{
  if (planes.getSize() < dimension)
    return false;
  SquareMatrix ATA(dimension, dimension);
  Point ATb(dimension);
  for (UnsignedInteger p = 0; p < planes.getSize(); ++ p)
  {
    const Point crossing(crossings[planes[p]]);
    const Point normal(normals[planes[p]]);
    const Scalar rhs = normal.dot(crossing);
    for (UnsignedInteger r = 0; r < dimension; ++ r)
    {
      ATb[r] += rhs * normal[r];
      for (UnsignedInteger c = 0; c < dimension; ++ c)
        ATA(r, c) += normal[r] * normal[c];
    }
  }
  try
  {
    minimizer = ATA.solveLinearSystem(ATb);
  }
  catch (const Exception &)
  {
    // singular system (locally smooth boundary): no QEF placement
    return false;
  }
  return true;
}


/* Default constructor */
LevelSetMesher::LevelSetMesher()
  : PersistentObject()
  , discretization_(0)
  , solver_(AbdoRackwitz())
{
  // Nothing to do
}

/* Parameter constructor */
LevelSetMesher::LevelSetMesher(const Indices & discretization,
                               const OptimizationAlgorithm & solver)
  : PersistentObject()
  , discretization_(discretization)
  , solver_(solver)
{
  // Check if the discretization is valid
  for (UnsignedInteger i = 0; i < discretization.getSize(); ++i)
    if (!(discretization[i] > 0)) throw InvalidArgumentException(HERE) << "Error: expected a positive discretization, got " << discretization;
}

/* Virtual constructor */
LevelSetMesher * LevelSetMesher::clone() const
{
  return new LevelSetMesher(*this);
}

/* String converter */
String LevelSetMesher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LevelSetMesher::GetClassName()
      << " discretization=" << discretization_;
  return oss;
}

/* String converter */
String LevelSetMesher::__str__(const String & ) const
{
  return __repr__();
}

/* Optimization solver accessor */
void LevelSetMesher::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

OptimizationAlgorithm LevelSetMesher::getOptimizationAlgorithm() const
{
  return solver_;
}


/* Discretization accessors */
void LevelSetMesher::setDiscretization(const Indices & discretization)
{
  // At least one slice per dimension
  for (UnsignedInteger i = 0; i < discretization.getSize(); ++i)
    if (!(discretization[i] > 0)) throw InvalidArgumentException(HERE) << "Error: expected positive values for the discretization, here discretization[" << i << "]=" << discretization[i];
  discretization_ = discretization;
}

Indices LevelSetMesher::getDiscretization() const
{
  return discretization_;
}

/* Here is the interface that all derived class must implement */

Mesh LevelSetMesher::build(const LevelSet & levelSet,
                           const Bool project) const
{
  const UnsignedInteger dimension = levelSet.getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for levelSets of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  return build(levelSet, Interval(levelSet.getLowerBound(), levelSet.getUpperBound()), project);
}

Mesh LevelSetMesher::build(const LevelSet & levelSet,
                           const Interval & boundingBox,
                           const Bool project) const
{
  const UnsignedInteger dimension = levelSet.getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for levelSets of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  if (boundingBox.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the bounding box is of dimension=" << boundingBox.getDimension() << ", expected dimension=" << dimension;
  const Mesh boundingMesh(IntervalMesher(discretization_).build(boundingBox));
  const Function function(levelSet.getFunction());
  const Field field(boundingMesh, function(boundingMesh.getVertices()));
  return build(levelSet, field, project);
}

Mesh LevelSetMesher::build(const LevelSet & levelSet,
                           const Field & field,
                           const Bool project) const
{
  // Single level set: delegate to the collection (intersection) core,
  // which coincides with the scalar algorithm for one constraint
  Collection<LevelSet> collection(0);
  collection.add(levelSet);
  return build(collection, field, project);
}

/* Intersection of a collection of level sets: bounds overloads first */

Mesh LevelSetMesher::build(const Collection<LevelSet> & collection,
                           const Bool project) const
{
  const UnsignedInteger size = collection.getSize();
  if (!size) throw InvalidArgumentException(HERE) << "Error: expected a non-empty collection of level sets";
  const UnsignedInteger dimension = collection[0].getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for levelSets of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  Point lower(collection[0].getLowerBound());
  Point upper(collection[0].getUpperBound());
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    if (collection[i].getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected level sets of dimension=" << dimension << ", here dimension=" << collection[i].getDimension();
    const Point lowI(collection[i].getLowerBound());
    const Point uppI(collection[i].getUpperBound());
    for (UnsignedInteger k = 0; k < dimension; ++ k)
    {
      lower[k] = std::max(lower[k], lowI[k]);
      upper[k] = std::min(upper[k], uppI[k]);
    }
  }
  for (UnsignedInteger k = 0; k < dimension; ++ k)
    if (!(lower[k] <= upper[k]))
      return Mesh(Sample(0, dimension));
  return build(collection, Interval(lower, upper), project);
}

Mesh LevelSetMesher::build(const Collection<LevelSet> & collection,
                           const Interval & boundingBox,
                           const Bool project) const
{
  const UnsignedInteger size = collection.getSize();
  if (!size) throw InvalidArgumentException(HERE) << "Error: expected a non-empty collection of level sets";
  const UnsignedInteger dimension = collection[0].getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for levelSets of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  if (boundingBox.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the bounding box is of dimension=" << boundingBox.getDimension() << ", expected dimension=" << dimension;
  const Mesh boundingMesh(IntervalMesher(discretization_).build(boundingBox));
  const Sample boundingVertices(boundingMesh.getVertices());
  const UnsignedInteger numVertices = boundingVertices.getSize();
  Sample values(numVertices, size);
  for (UnsignedInteger k = 0; k < size; ++ k)
  {
    if (collection[k].getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected level sets of dimension=" << dimension << ", here dimension=" << collection[k].getDimension();
    const Sample valuesK(collection[k].getFunction()(boundingVertices));
    for (UnsignedInteger i = 0; i < numVertices; ++ i)
      values(i, k) = valuesK(i, 0);
  }
  return build(collection, Field(boundingMesh, values), project);
}

Mesh LevelSetMesher::build(const Collection<LevelSet> & collection,
                           const Field & field,
                           const Bool project) const
{
  const UnsignedInteger constraintNumber = collection.getSize();
  if (!constraintNumber) throw InvalidArgumentException(HERE) << "Error: expected a non-empty collection of level sets";
  const UnsignedInteger dimension = collection[0].getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for levelSets of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  if (field.getMesh().getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the field is of mesh dimension=" << field.getMesh().getDimension() << ", expected input dimension=" << dimension;
  Collection<Function> functions(0);
  Collection<ComparisonOperator> operators(0);
  Point levels(constraintNumber);
  for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
  {
    if (collection[k].getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected level sets of dimension=" << dimension << ", here dimension=" << collection[k].getDimension();
    functions.add(collection[k].getFunction());
    operators.add(collection[k].getOperator());
    levels[k] = collection[k].getLevel();
  }
  // Extract the mesh and vertices from the field
  const Mesh boundingMesh(field.getMesh());
  Sample boundingVertices(boundingMesh.getVertices());
  const UnsignedInteger numVertices = boundingVertices.getSize();
  const IndicesCollection boundingSimplices(boundingMesh.getSimplices());
  const UnsignedInteger numSimplices = boundingSimplices.getSize();
  // Values of each level function at the background vertices: field columns
  // when they match the collection, fresh evaluations otherwise
  Sample values(numVertices, constraintNumber);
  if (field.getDimension() == constraintNumber)
    values = field.getValues();
  else
  {
    LOGWARN(OSS() << "The field output dimension=" << field.getDimension() << " is different from the collection size=" << constraintNumber << ". The functions defining the level sets will be evaluated over the vertices of the mesh to get the values.");
    for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
    {
      const Sample valuesK(functions[k](boundingVertices));
      for (UnsignedInteger i = 0; i < numVertices; ++ i)
        values(i, k) = valuesK(i, 0);
    }
  }
  Indices goodSimplices(0);
  Sample goodVertices(0, dimension);
  // Flags for the vertices to keep
  Indices flagGoodVertices(numVertices, 0);
  // Vertices that have moved
  Sample movedVertices(0, dimension);
  // Flag for the vertices that have moved
  Indices flagMovedVertices(0);
  // QEF target flag and moved-vertex positions (QEF re-move rule)
  Indices flagQEFTarget(numVertices, 0);
  Indices movedPosition(numVertices, numVertices);
  // Prepare the optimization problem for the projection
  TranslationFunction shiftFunction((Point(dimension)));
  NearestPointProblem problem;
  // Create once some objects that will be reused a lot
  Sample localVertices(dimension + 1, dimension);
  Collection<Point> localValues(dimension + 1, Point(constraintNumber));
  Indices simplicesToCheck(0);
  UnsignedInteger goodSimplicesNumber = 0;
  const Bool solveEquation = ResourceMap::GetAsBool("LevelSetMesher-SolveEquation");
  const Bool useQEF = project && (ResourceMap::GetAsString("LevelSetMesher-Algorithm") == "QEF");
  typedef std::pair<UnsignedInteger, UnsignedInteger> EdgeKey;
  std::vector< std::map<EdgeKey, UnsignedInteger> > edgePlaneIndices(constraintNumber);
  Collection<Gradient> gradients(0);
  for (UnsignedInteger k = 0; k < constraintNumber; ++ k) gradients.add(Gradient());
  Collection<Point> crossings;
  Collection<Point> normals;
  if (useQEF)
  {
    // Hermite data on cut edges, one cache per (smooth) level function
    for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
    {
      try
      {
        gradients[k] = functions[k].getGradient();
      }
      catch (const Exception &)
      {
        const Scalar epsilon = std::sqrt(SpecFunc::Precision);
        gradients[k] = Gradient(CenteredFiniteDifferenceGradient(epsilon, functions[k].getEvaluation()));
      }
    }
    for (UnsignedInteger i = 0; i < numSimplices; ++ i)
    {
      UnsignedInteger numGood = 0;
      for (UnsignedInteger j = 0; j <= dimension; ++ j)
        if (IsInsideIntersection(operators, levels, values, boundingSimplices(i, j)))
          ++ numGood;
      if ((numGood == 0) || (numGood > dimension))
        continue;
      for (UnsignedInteger j1 = 0; j1 <= dimension; ++ j1)
        for (UnsignedInteger j2 = j1 + 1; j2 <= dimension; ++ j2)
        {
          const UnsignedInteger globalA = boundingSimplices(i, j1);
          const UnsignedInteger globalB = boundingSimplices(i, j2);
          const EdgeKey key(std::min(globalA, globalB), std::max(globalA, globalB));
          for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
          {
            const Bool aInside = operators[k](values(globalA, k), levels[k]);
            if (aInside == operators[k](values(globalB, k), levels[k]))
              continue;
            if (edgePlaneIndices[k].find(key) != edgePlaneIndices[k].end())
              continue;
            const Point edgeA(boundingVertices[globalA]);
            const Point edgeB(boundingVertices[globalB]);
            const Point base(aInside ? edgeA : edgeB);
            const Point edgeShift(aInside ? edgeB - base : edgeA - base);
            const LinearFunction tToEdge(Point(1), base, Matrix(dimension, 1, edgeShift));
            const ComposedFunction edgeConstraint(functions[k], tToEdge);
            Brent brent;
            try
            {
              const Scalar t = brent.solve(edgeConstraint, levels[k], 0.0, 1.0);
              const Point crossing(tToEdge(Point(1, t)));
              const Matrix gradMatrix(gradients[k].gradient(crossing));
              if (gradMatrix.getNbColumns() != 1)
                continue;
              Point normal(dimension);
              for (UnsignedInteger d = 0; d < dimension; ++ d)
                normal[d] = gradMatrix(d, 0);
              if (!(normal.norm() > 0.0))
                continue;
              edgePlaneIndices[k][key] = crossings.getSize();
              crossings.add(crossing);
              normals.add(normal);
            }
            catch (const Exception &)
            {
              // no Hermite data on this edge for this constraint
            }
          } // k
        } // edges
    } // simplices
  }
  for (UnsignedInteger i = 0; i < numSimplices; ++i)
  {
    UnsignedInteger numGood = 0;
    // Count the vertices in the intersection
    for (UnsignedInteger j = 0; j <= dimension; ++j)
    {
      const UnsignedInteger globalVertexIndex = boundingSimplices(i, j);
      if (IsInsideIntersection(operators, levels, values, globalVertexIndex))
      {
        ++numGood;
        ++flagGoodVertices[globalVertexIndex];
      }
    }
    // If enough vertices, keep the simplex and flag all the vertices
    if (numGood > 0)
    {
      goodSimplices.add(Collection<UnsignedInteger>(boundingSimplices.cbegin_at(i), boundingSimplices.cend_at(i)));
      ++goodSimplicesNumber;
      // Check if we have to move some vertices
      if (numGood <= dimension)
      {
        // If at least one vertex moves, the orientation can change
        simplicesToCheck.add(goodSimplicesNumber - 1);
        for (UnsignedInteger j = 0; j <= dimension; ++j)
        {
          const UnsignedInteger index = boundingSimplices(i, j);
          localVertices[j] = boundingVertices[index];
          for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
            localValues[j][k] = values(index, k);
        }
        Point center(dimension);
        Point centerValues(constraintNumber);
        Bool simplexHasQEF = false;
        // First pass: compute the center of the good points
        for (UnsignedInteger j = 0; j <= dimension; ++j)
          if (IsInsideIntersection(operators, levels, values, boundingSimplices(i, j)))
          {
            center += localVertices[j];
            for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
              centerValues[k] += localValues[j][k];
          }
        center /= numGood;
        centerValues /= 1.0 * numGood;
        // QEF ray target over all constraints (each level function is smooth)
        if (useQEF)
        {
          Indices simplexPlanes;
          for (UnsignedInteger j1 = 0; j1 <= dimension; ++ j1)
            for (UnsignedInteger j2 = j1 + 1; j2 <= dimension; ++ j2)
            {
              const UnsignedInteger globalA = boundingSimplices(i, j1);
              const UnsignedInteger globalB = boundingSimplices(i, j2);
              const EdgeKey key(std::min(globalA, globalB), std::max(globalA, globalB));
              for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
              {
                const std::map<EdgeKey, UnsignedInteger>::const_iterator it = edgePlaneIndices[k].find(key);
                if (it != edgePlaneIndices[k].end())
                  simplexPlanes.add(it->second);
              }
            }
          Point qefTarget(dimension);
          if (ComputeQEFMinimizer(dimension, crossings, normals, simplexPlanes, qefTarget))
          {
            // use the feature point if inside the intersection or numerically
            // on its boundary (QEF targets carry finite-difference noise)
            try
            {
              Bool qefInside = true;
              Bool qefOnBoundary = true;
              for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
              {
                const Scalar qefValue = functions[k](qefTarget)[0];
                const Scalar boundaryTolerance = std::sqrt(SpecFunc::Precision) * (1.0 + std::abs(levels[k]));
                if (!operators[k](qefValue, levels[k]) && (std::abs(qefValue - levels[k]) > boundaryTolerance))
                  qefInside = false;
                if (std::abs(qefValue - levels[k]) > boundaryTolerance)
                  qefOnBoundary = false;
                centerValues[k] = qefValue;
              }
              if (qefInside || qefOnBoundary)
              {
                center = qefTarget;
                simplexHasQEF = true;
              }
            }
            catch (const Exception &)
            {
              // keep the legacy center
            }
          }
        }
        // Second pass, move the vertices that are outside of the intersection
        // using a linear interpolation between the center and the vertex
        for (UnsignedInteger j = 0; j <= dimension; ++j)
        {
          const UnsignedInteger globalVertexIndex = boundingSimplices(i, j);
          // If the vertex has to be moved (same re-move rule as scalar build)
          const Bool neverSeen = (flagGoodVertices[globalVertexIndex] == 0);
          const Bool qefRemove = useQEF && simplexHasQEF && !neverSeen && !flagQEFTarget[globalVertexIndex] && (movedPosition[globalVertexIndex] < flagMovedVertices.getSize());
          if (!(neverSeen || qefRemove))
            continue;
          Indices violated(0);
          for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
            if (!operators[k](localValues[j][k], levels[k]))
              violated.add(k);
          if (!violated.getSize())
            continue;
          const Point currentVertex(boundingVertices[globalVertexIndex]);
          const Point shift(center - currentVertex);
          // linear guess dominated by the most violated constraint
          Scalar rho = 0.0;
          Bool rayValid = false;
          UnsignedInteger mostViolated = violated[0];
          Scalar largestGap = -SpecFunc::Infinity;
          for (UnsignedInteger v = 0; v < violated.getSize(); ++ v)
          {
            const UnsignedInteger k = violated[v];
            const Scalar gap = std::abs(localValues[j][k] - levels[k]);
            if (gap > largestGap)
            {
              largestGap = gap;
              mostViolated = k;
            }
            const Scalar denom = localValues[j][k] - centerValues[k];
            if (denom != 0.0)
            {
              rayValid = true;
              rho = std::max(rho, (localValues[j][k] - levels[k]) / denom);
            }
          }
          const Bool isRemove = !neverSeen;
          if (!isRemove)
          {
            movedPosition[globalVertexIndex] = flagMovedVertices.getSize();
            flagMovedVertices.add(globalVertexIndex);
          }
          if (simplexHasQEF)
            flagQEFTarget[globalVertexIndex] = 1;
          // append slot for a first move, in-place slot for a QEF re-move
          const UnsignedInteger moveSlot = isRemove ? movedPosition[globalVertexIndex] : movedVertices.getSize();
          const Point delta(shift * rho);
          // If no projection, just add the linear correction
          if (!project)
          {
            const Point newPosition(currentVertex + delta);
            if (isRemove) movedVertices[moveSlot] = newPosition; else movedVertices.add(newPosition);
          }
          else
          {
            Bool minimizeDistance = !solveEquation || !rayValid;
            Scalar tStar = 0.0;
            if (solveEquation && rayValid)
            {
              const LinearFunction tToPoint(Point(1), currentVertex, Matrix(currentVertex.getDimension(), 1, shift));
              // feasible point along the ray: largest root over violated constraints
              for (UnsignedInteger v = 0; v < violated.getSize(); ++ v)
              {
                const UnsignedInteger k = violated[v];
                const ComposedFunction constraint(functions[k], tToPoint);
                Brent brent;
                try
                {
                  const Scalar t = brent.solve(constraint, levels[k], 0.0, 1.0);
                  LOGDEBUG(OSS() << "Projection of " << currentVertex << " gives t=" << t << " for constraint " << v);
                  tStar = std::max(tStar, t);
                }
                catch (const Exception &)
                {
                  LOGDEBUG(OSS() << "Problem to project point=" << currentVertex << " with equation solver=" << brent << " for constraint " << v << ", using minimization for the projection");
                  minimizeDistance = true;
                  break;
                }
              }
            } // solveEquation
            if (!minimizeDistance)
            {
              const LinearFunction tToPoint(Point(1), currentVertex, Matrix(currentVertex.getDimension(), 1, shift));
              const Point newPosition(tToPoint(Point(1, tStar)));
              if (isRemove) movedVertices[moveSlot] = newPosition; else movedVertices.add(newPosition);
            }
            else
            {
              // Project on the boundary of the most violated constraint:
              // argmin ||x - x_0||^2 such that level - f(x) >= 0
              shiftFunction.setConstant(currentVertex);
              ComposedFunction levelFunction(functions[mostViolated], shiftFunction);
              problem.setLevelFunction(levelFunction);
              problem.setLevelValue(levels[mostViolated]);
              OptimizationAlgorithm solver(solver_);
              solver.setStartingPoint(delta);
              solver.setProblem(problem);
              // Here we have to catch exceptions raised by the algorithm (may be due to e.g the gradient)
              try
              {
                solver.run();
                const Point newPosition(currentVertex + solver.getResult().getOptimalPoint());
                if (isRemove) movedVertices[moveSlot] = newPosition; else movedVertices.add(newPosition);
              }
              catch (const Exception &)
              {
                // There is a problem with this vertex. Try a gradient-free solver
                Cobyla cobyla(solver.getProblem());
                cobyla.setStartingPoint(delta);
                LOGDEBUG(OSS() << "Problem to project point=" << currentVertex << " with solver=" << solver << " and finite differences for gradient, switching to solver=" << cobyla);
                try
                {
                  cobyla.run();
                  const Point newPosition(currentVertex + cobyla.getResult().getOptimalPoint());
                  if (isRemove) movedVertices[moveSlot] = newPosition; else movedVertices.add(newPosition);
                }
                catch (const Exception &)
                {
                  LOGDEBUG(OSS() << "Problem to project point=" << currentVertex << " with solver=" << cobyla << ", use basic linear interpolation");
                  const Point newPosition(currentVertex + delta);
                  if (isRemove) movedVertices[moveSlot] = newPosition; else movedVertices.add(newPosition);
                }
              } // User-defined solver failed ?
            } // minimizeDistance
          } // project
          ++flagGoodVertices[globalVertexIndex];
        } // j = 0; j <= dimension; ++j
      } // numGood <= dimension
    } // numGood > 0
  } // i < numSimplices
  // Per-vertex QEF override over all constraints (max-gap fit)
  if (useQEF && (flagMovedVertices.getSize() > 0))
  {
    std::map<UnsignedInteger, Indices> vertexPlanes;
    for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
      for (std::map<EdgeKey, UnsignedInteger>::const_iterator it = edgePlaneIndices[k].begin(); it != edgePlaneIndices[k].end(); ++ it)
      {
        vertexPlanes[it->first.first].add(it->second);
        vertexPlanes[it->first.second].add(it->second);
      }
    for (UnsignedInteger m = 0; m < flagMovedVertices.getSize(); ++ m)
    {
      const std::map<UnsignedInteger, Indices>::const_iterator it = vertexPlanes.find(flagMovedVertices[m]);
      if (it == vertexPlanes.end())
        continue;
      Point candidate(dimension);
      if (!ComputeQEFMinimizer(dimension, crossings, normals, it->second, candidate))
        continue;
      try
      {
        Scalar legacyGap = 0.0;
        Scalar candidateGap = 0.0;
        for (UnsignedInteger k = 0; k < constraintNumber; ++ k)
        {
          legacyGap = std::max(legacyGap, std::abs(functions[k](movedVertices[m])[0] - levels[k]));
          candidateGap = std::max(candidateGap, std::abs(functions[k](candidate)[0] - levels[k]));
        }
        if (candidateGap <= legacyGap)
          movedVertices[m] = candidate;
      }
      catch (const Exception &)
      {
        // keep legacy position
      }
    }
  }
  // Insert the vertices that have moved
  for (UnsignedInteger i = 0; i < flagMovedVertices.getSize(); ++i)
    boundingVertices[flagMovedVertices[i]] = movedVertices[i];
  // Extract the vertices to keep and reuse the flag to store vertices
  // indices shifts
  for (UnsignedInteger i = 0; i < numVertices; ++i)
  {
    if (flagGoodVertices[i] > 0) goodVertices.add(boundingVertices[i]);
    flagGoodVertices[i] = i + 1 - goodVertices.getSize();
  }
  // Shift the vertices indices into the good simplices
  for (UnsignedInteger i = 0; i < goodSimplices.getSize(); ++i)
    goodSimplices[i] -= flagGoodVertices[goodSimplices[i]];
  Mesh result(goodVertices, IndicesCollection(goodSimplices.getSize() / (dimension + 1), dimension + 1, goodSimplices), false);
  // Fix the orientation of the simplices with moved vertices
  SquareMatrix matrix(dimension + 1);
  for (UnsignedInteger i = 0; i < simplicesToCheck.getSize(); ++i)
    result.fixOrientation(simplicesToCheck[i], matrix);
  return result;
}

END_NAMESPACE_OPENTURNS
