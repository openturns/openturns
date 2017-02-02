//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and by-products
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
#ifndef OPENTURNS_KARHUNENLOEVERESULTIMPLEMENTATION_HXX
#define OPENTURNS_KARHUNENLOEVERESULTIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Basis.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Field.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveResultImplementation
 *
 * Compute the KL decomposition of a given covariance model
 */
class OT_API KarhunenLoeveResultImplementation
  : public PersistentObject
{
  CLASSNAME;

public:


  /** Default constructor */
  KarhunenLoeveResultImplementation();

  /** Default constructor */
  explicit KarhunenLoeveResultImplementation(const CovarianceModel & covariance,
					     const NumericalScalar threshold,
					     const NumericalPoint & eigenvalues,
					     const Basis & modes,
					     const ProcessSample & modesAsProcessSample,
					     const Matrix & projection);

  /** Virtual constructor */
  virtual KarhunenLoeveResultImplementation * clone() const;

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
  NumericalPoint project(const NumericalMathFunction & function) const;
  NumericalPoint project(const Field & field) const;
  NumericalSample project(const Basis & basis) const;
  NumericalSample project(const ProcessSample & sample) const;

  /** Lift method */
  NumericalMathFunction lift(const NumericalPoint & coefficients) const;
  Field liftAsField(const NumericalPoint & coefficients) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /** Covariance model */
  CovarianceModel covariance_;

  /** Threshold */
  NumericalScalar threshold_;

  /** Eigenvalues */
  NumericalPoint eigenvalues_;

  /** Modes */
  Basis modes_;

  /** Modes as process sample */
  ProcessSample modesAsProcessSample_;

  /** Projection matrix */
  Matrix projection_;

}; /* class KarhunenLoeveResultImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVERESULTIMPLEMENTATION_HXX */
