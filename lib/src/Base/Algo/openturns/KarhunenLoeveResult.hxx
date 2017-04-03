//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 */
#ifndef OPENTURNS_KARHUNENLOEVERESULT_HXX
#define OPENTURNS_KARHUNENLOEVERESULT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/KarhunenLoeveResultImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveResult
 *
 * Compute the KL decomposition of a given covariance model
 */
class OT_API KarhunenLoeveResult
  : public TypedInterfaceObject<KarhunenLoeveResultImplementation>
{
  CLASSNAME;

public:

  typedef TypedInterfaceObject<KarhunenLoeveResultImplementation>::Implementation Implementation;

  /** Default constructor */
  KarhunenLoeveResult();

  /** Constructor from implementation */
  KarhunenLoeveResult(const KarhunenLoeveResultImplementation & implementation);

  /** Constructor from implementation */
  KarhunenLoeveResult(const Implementation & p_implementation);

  /** Threshold accessor */
  NumericalScalar getThreshold() const;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Eigenvalues accessor */
  NumericalPoint getEigenValues() const;

  /** Modes accessors */
  Basis getModes() const;
  ProcessSample getModesAsProcessSample() const;

  /** Scaled modes accessors */
  Basis getScaledModes() const;
  ProcessSample getScaledModesAsProcessSample() const;

  /** Projection matrix accessor */
  Matrix getProjectionMatrix() const;

  /** Projection method */
  NumericalPoint project(const Function & function) const;
  NumericalPoint project(const Field & field) const;
  Sample project(const Basis & basis) const;
  Sample project(const ProcessSample & sample) const;

  /** Lift method */
  Function lift(const NumericalPoint & coefficients) const;
  Field liftAsField(const NumericalPoint & coefficients) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

}; /* class KarhunenLoeveResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVERESULT_HXX */
