//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and by-products
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KARHUNENLOEVERESULTIMPLEMENTATION_HXX
#define OPENTURNS_KARHUNENLOEVERESULTIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Point.hxx"
#include "openturns/Function.hxx"
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
  CLASSNAME

public:

  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Default constructor */
  KarhunenLoeveResultImplementation();

  /** Default constructor */
  KarhunenLoeveResultImplementation(const CovarianceModel & covariance,
                                    const Scalar threshold,
                                    const Point & eigenvalues,
                                    const FunctionCollection & modes,
                                    const ProcessSample & modesAsProcessSample,
                                    const Matrix & projection);

  /** Virtual constructor */
  KarhunenLoeveResultImplementation * clone() const override;

  /** Threshold accessor */
  Scalar getThreshold() const;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Eigenvalues accessor */
  Point getEigenValues() const;

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

  /** Projection method */
  Point project(const Function & function) const;
  Point project(const Sample & values) const;
  Sample project(const FunctionCollection & basis) const;
  Sample project(const ProcessSample & sample) const;

  /** Lift method */
  Function lift(const Point & coefficients) const;
  Sample liftAsSample(const Point & coefficients) const;
  Field liftAsField(const Point & coefficients) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Covariance model */
  CovarianceModel covariance_;

  /** Threshold */
  Scalar threshold_;

  /** Eigenvalues */
  Point eigenvalues_;

  /** Modes */
  FunctionPersistentCollection modes_;

  /** Modes as process sample */
  ProcessSample modesAsProcessSample_;

  /** Projection matrix */
  Matrix projection_;

}; /* class KarhunenLoeveResultImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVERESULTIMPLEMENTATION_HXX */
