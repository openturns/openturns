//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KARHUNENLOEVERESULT_HXX
#define OPENTURNS_KARHUNENLOEVERESULT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/KarhunenLoeveResultImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveResult
 *
 * Compute the KL decomposition of a given covariance model
 */
class OT_API KarhunenLoeveResult
  : public TypedInterfaceObject<KarhunenLoeveResultImplementation>
{
  CLASSNAME

public:

  typedef TypedInterfaceObject<KarhunenLoeveResultImplementation>::Implementation Implementation;
  typedef KarhunenLoeveResultImplementation::FunctionCollection  FunctionCollection;

  /** Default constructor */
  KarhunenLoeveResult();

  /** Constructor with parameters */
  KarhunenLoeveResult(const CovarianceModel & covariance,
                      const Scalar threshold,
                      const Point & eigenvalues,
                      const FunctionCollection & modes,
                      const ProcessSample & modesAsProcessSample,
                      const Matrix & projection,
                      const Scalar selectionRatio);

  /** Constructor from implementation */
  KarhunenLoeveResult(const KarhunenLoeveResultImplementation & implementation);

  /** Constructor from implementation */
  KarhunenLoeveResult(const Implementation & p_implementation);

  /** Threshold accessor */
  Scalar getThreshold() const;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Eigenvalues accessor */
  Point getEigenvalues() const;

  /** Draw eigen values */
  Graph drawEigenvalues() const;
  Graph drawCumulatedEigenvaluesRemainder() const;

  /** Modes accessors */
  FunctionCollection getModes() const;
  ProcessSample getModesAsProcessSample() const;

  /** Scaled modes accessors */
  FunctionCollection getScaledModes() const;
  ProcessSample getScaledModesAsProcessSample() const;

  /** Projection matrix accessor */
  Matrix getProjectionMatrix() const;

  /** Mesh accessor */
  Mesh getMesh() const;

  /** Selection ratio accessor */
  Scalar getSelectionRatio() const;

  /** Projection method */
  Point project(const Function & function) const;
  Point project(const Sample & values) const;
  Sample project(const FunctionCollection & functionCollection) const;
  Sample project(const ProcessSample & sample) const;

  /** Lift method */
  Function lift(const Point & coefficients) const;
  Field liftAsField(const Point & coefficients) const;
  Sample liftAsSample(const Point & coefficients) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class KarhunenLoeveResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVERESULT_HXX */
