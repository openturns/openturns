//                                               -*- C++ -*-
/**
 *  @brief Predictor-corrector continuation of a one dimensional fibre
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
#ifndef OPENTURNS_LEVELSETCONTINUATION_HXX
#define OPENTURNS_LEVELSETCONTINUATION_HXX

#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LevelSetContinuation
 *
 * Continue a regular one dimensional fibre of a map from R^n to R^{n-1},
 * starting from segments supplied by a background mesh. The class only
 * depends on the map, the density along the fibre, the search box and the
 * requested level. The mesh supplies the topology and one seed per connected
 * component; the predictor-corrector supplies the refined geometry and the
 * density integral. The mesh itself is not built here, so the dimension n is
 * only used by the local linear algebra and by the spatial hash.
 */
class OT_API LevelSetContinuation
{
public:

  /** One segment of the mesh approximation of the fibre */
  struct Segment
  {
    Point first;
    Point second;
  };

  enum Status
  {
    SUCCESS,
    FAILURE,
    SINGULAR
  };

  struct Result
  {
    Status status;
    Scalar density;
    Result() : status(FAILURE), density(0.0) {}
  };

  /** Parameter constructor */
  LevelSetContinuation(const Function & function,
                       const Function & density,
                       const Interval & searchInterval,
                       const Point & level);

  /** Refine the fibre described by the mesh segments and integrate the
      density along it. */
  Result computeDensity(const Collection<Segment> & segments,
                        Scalar meshStep,
                        Scalar endpointTolerance) const;

  /** Jacobian of the map at the given point */
  Matrix getJacobian(const Point & point, Bool & available) const;

  /** Co-area factor of the one dimensional fibre */
  Scalar getCoAreaFactor(const Matrix & jacobian) const;

  /** Unit tangent of the one dimensional fibre */
  Point getTangent(const Matrix & jacobian, Bool & available) const;

  /** Generalized cross product of the n - 1 columns of an n x (n - 1)
      Jacobian. Its norm is the co-area factor and its direction is the
      tangent of the one dimensional fibre. */
  static Point computeCoAreaVector(const Matrix & jacobian);

  /** Norm of the generalized cross product, i.e. the co-area factor */
  static Scalar computeCoAreaFactor(const Matrix & jacobian);

  /** Project a point onto the fibre */
  Bool project(Point & point) const;

private:

  /** Right-hand side of the arc-length ODE */
  Bool evaluateRHS(const Point & point,
                   Point & tangent,
                   Scalar & integrand,
                   Bool & singular) const;

  /** One RK4 step followed by a Newton projection */
  Bool RK4Step(Point & point,
               Scalar & integral,
               Scalar step,
               Scalar orientation,
               Point & tangentEnd,
               Bool & singular) const;

  /** Trace one direction from a seed */
  Result traceDirection(const Point & seed,
                        Scalar orientation,
                        Scalar step,
                        Scalar minimumStep,
                        Bool & closed,
                        Scalar & length,
                        Collection<Point> & path) const;

  /** Trace one connected component from one of its mesh segments */
  Status traceComponent(const Segment & segment,
                        Scalar step,
                        Scalar minimumStep,
                        Scalar & density,
                        Collection<Point> & path) const;

  /** The map defining the fibre */
  Function function_;

  /** The density integrated along the fibre */
  Function density_;

  /** Search box */
  Interval searchInterval_;

  /** Requested level */
  Point level_;
}; /* class LevelSetContinuation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEVELSETCONTINUATION_HXX */
