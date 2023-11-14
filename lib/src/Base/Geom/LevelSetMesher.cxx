//                                               -*- C++ -*-
/**
 *  @brief Meshing algorithm for levelSets
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

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LevelSetMesher)
static const Factory<LevelSetMesher> Factory_LevelSetMesher;


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
    if (!(discretization_[i] > 0)) throw InvalidArgumentException(HERE) << "Error: expected positive values for the discretization, here discretization[" << i << "]=" << discretization[i];
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
  if (!(dimension <= 3)) throw NotYetImplementedException(HERE) << "In LevelSetMesher::build(const LevelSet & levelSet, const Bool project) const";
  return build(levelSet, Interval(levelSet.getLowerBound(), levelSet.getUpperBound()), project);
}

Mesh LevelSetMesher::build(const LevelSet & levelSet,
                           const Interval & boundingBox,
                           const Bool project) const
{
  const UnsignedInteger dimension = levelSet.getDimension();
  if (discretization_.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the mesh factory is for levelSets of dimension=" << discretization_.getSize() << ", here dimension=" << dimension;
  if (boundingBox.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the bounding box is of dimension=" << boundingBox.getDimension() << ", expected dimension=" << dimension;
  // First, mesh the bounding box
  const Mesh boundingMesh(IntervalMesher(discretization_).build(boundingBox));
  Sample boundingVertices(boundingMesh.getVertices());
  const UnsignedInteger numVertices = boundingVertices.getSize();
  const IndicesCollection boundingSimplices(boundingMesh.getSimplices());
  const UnsignedInteger numSimplices = boundingSimplices.getSize();
  // Second, keep only the simplices with a majority of vertices in the level set
  const Function function(levelSet.getFunction());
  const Point values(function(boundingVertices).asPoint());
  const Scalar level = levelSet.getLevel();
  const ComparisonOperator comparison(levelSet.getOperator());
  Indices goodSimplices(0);
  Sample goodVertices(0, dimension);
  // Flags for the vertices to keep
  Indices flagGoodVertices(numVertices, 0);
  // Vertices that have moved
  Sample movedVertices(0, dimension);
  // Flag for the vertices that have moved
  Indices flagMovedVertices(0);
  // Prepare the optimization problem for the projection
  TranslationFunction shiftFunction((Point(dimension)));
  NearestPointProblem problem;
  problem.setLevelValue(level);
  // Create once some objects that will be reused a lot
  Sample localVertices(dimension + 1, dimension);
  Point localValues(dimension + 1);
  Indices simplicesToCheck(0);
  UnsignedInteger goodSimplicesNumber = 0;
  const Bool solveEquation = ResourceMap::GetAsBool("LevelSetMesher-SolveEquation");
  Bool minimizeDistance = !solveEquation;
  for (UnsignedInteger i = 0; i < numSimplices; ++i)
  {
    UnsignedInteger numGood = 0;
    // Count the vertices in the level set
    for (UnsignedInteger j = 0; j <= dimension; ++j)
    {
      const UnsignedInteger globalVertexIndex = boundingSimplices(i, j);
      if (comparison(values[globalVertexIndex], level))
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
          localValues[j] = values[index];
        }
        Point center(dimension);
        Scalar centerValue = 0.0;
        // First pass: compute the center of the good points
        for (UnsignedInteger j = 0; j <= dimension; ++j)
          if (comparison(localValues[j], level))
          {
            center += localVertices[j];
            centerValue += localValues[j];
          }
        center /= numGood;
        centerValue /= numGood;
        // Second pass, move the vertices that are outside of the level set
        // using a linear interpolation between the center and the vertex
        for (UnsignedInteger j = 0; j <= dimension; ++j)
        {
          const UnsignedInteger globalVertexIndex = boundingSimplices(i, j);
          // If the vertex has to be moved
          if ((flagGoodVertices[globalVertexIndex] == 0) && (!comparison(localValues[j], level)))
          {
            // C(v*) [inside], M(level) [on], B(v) [outside]
            // (M-C)/(B-C) = (level-v*)/(v-v*) = a
            // M-B=(v-level)/(v-v*)(C-B)
            const Point currentVertex(boundingVertices[globalVertexIndex]);
            const Point shift(center - currentVertex);
            const Scalar rho = (localValues[j] - level) / (localValues[j] - centerValue);
            const Point delta(shift * rho);
            flagMovedVertices.add(globalVertexIndex);
            // If no projection, just add the linear correction
            if (!project) movedVertices.add(currentVertex + delta);
            else
            {
              if (solveEquation)
              {
                const LinearFunction tToPoint(Point(1), currentVertex, Matrix(currentVertex.getDimension(), 1, shift));
                const ComposedFunction constraint(function, tToPoint);
                Brent solver;
                try
                {
                  const Scalar t = solver.solve(constraint, level, 0.0, 1.0);
                  LOGDEBUG(OSS() << "Projection of " << currentVertex << " gives t=" << t);
                  movedVertices.add(tToPoint(Point(1, t)));
                }
                catch(...)
                {
                  LOGDEBUG(OSS() << "Problem to project point=" << currentVertex << " with equation solver=" << solver << ", using minimization for the projection");
                  minimizeDistance = true;
                }
              } // solveEquation
              if (minimizeDistance)
              {
                // Project the vertices not in the level set on the boundary of the level set
                // Build the optimization problem argmin ||x - x_0||^2 such that level - f(x) >= 0, where x_0 is the current vertex
                shiftFunction.setConstant(currentVertex);
                ComposedFunction levelFunction(function, shiftFunction);
                problem.setLevelFunction(levelFunction);
                OptimizationAlgorithm solver(solver_);
                solver.setStartingPoint(delta);
                solver.setProblem(problem);
                OptimizationResult result;
                // Here we have to catch exceptions raised by the gradient
                try
                {
                  solver.run();
                  result = solver.getResult();
                }
                catch(...)
                {
                  LOGDEBUG(OSS() << "Problem to project point=" << currentVertex << " with solver=" << solver_ << ", using finite differences for gradient");
                  // Here we may have to fix the gradient eg in the case of analytical functions, when Ev3 does not handle the expression.
                  const Scalar epsilon = ResourceMap::GetAsScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon");
                  levelFunction.setGradient(CenteredFiniteDifferenceGradient((localVertices.getMin() - localVertices.getMax()) * epsilon + Point(dimension, epsilon), levelFunction.getEvaluation()).clone());
                  problem.setLevelFunction(levelFunction);
                  solver.setProblem(problem);
                  // Try with the new gradients
                  try
                  {
                    solver.run();
                    result = solver.getResult();
                  }
                  catch(...)
                  {
                    // There is definitely a problem with this vertex. Try a gradient-free solver
                    Cobyla solver2(solver.getProblem());
                    solver2.setStartingPoint(solver.getStartingPoint());
                    LOGDEBUG(OSS() << "Problem to project point=" << currentVertex << " with solver=" << solver << " and finite differences for gradient, switching to solver=" << solver2);
                    solver2.run();
                    result = solver2.getResult();
                  } // Even finite differences gradient failed?
                } // Gradient failed ?
                movedVertices.add(currentVertex + result.getOptimalPoint());
                minimizeDistance = !solveEquation;
              } // minimizeDistance
            } // project
            ++flagGoodVertices[globalVertexIndex];
          } // localValue[j] > level
        } // j = 0; j <= dimension; ++j
      } // numGood <= dimension
    } // numGood > 0
  } // i < numSimplices
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
