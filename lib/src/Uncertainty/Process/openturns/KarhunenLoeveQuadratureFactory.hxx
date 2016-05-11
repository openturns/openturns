//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         quadrature approximation of the integral equation.
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
 */
#ifndef OPENTURNS_KARHUNENLOEVEQUADRATUREFACTORY_HXX
#define OPENTURNS_KARHUNENLOEVEQUADRATUREFACTORY_HXX

#include "openturns/Domain.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/Basis.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveQuadratureFactory
 */

class OT_API KarhunenLoeveQuadratureFactory
  : public PersistentObject
{

  CLASSNAME;

 public:
  typedef Collection<NumericalMathFunction>           NumericalMathFunctionCollection;
  typedef PersistentCollection<NumericalMathFunction> NumericalMathFunctionPersistentCollection;

  /** Default constructor without parameters */
  KarhunenLoeveQuadratureFactory();

  /** Default constructor without parameters */
  KarhunenLoeveQuadratureFactory(const Domain & domain,
				 const WeightedExperiment & experiment,
				 const Basis & basis,
				 const UnsignedInteger basisSize,
				 const Bool mustScale,
				 const NumericalScalar threshold);

  /** Virtual copy constructor */
  virtual KarhunenLoeveQuadratureFactory * clone() const;

  /** Solve the Fredholm eigenvalues problem:
   * find (\phi_k, \lambda_k) such that
   * \int_{D} C(s,t)\phi_k(s)ds=\lambda_k\phi_k(t)
   * where C is a given covariance model, using a quadrature approximation
   * of the integral
   */
  Basis build(const CovarianceModel & covarianceModel,
              NumericalPoint & eigenvalues) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

 private:
  /** Underlying domain */
  Domain domain_;

  /** Functions used for the representation */
  NumericalMathFunctionPersistentCollection coll_;

  /** Threshold for eigenvalues selection */
  NumericalScalar threshold_;

  /** Quadrature nodes and weights */
  NumericalSample nodes_;
  NumericalPoint weights_;

  /** Scaled design matrix */
  mutable Matrix theta_;

  /** Inverse of the Cholesky factor of theta */
  mutable TriangularMatrix cholesky_;

} ; /* class KarhunenLoeveQuadratureFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEQUADRATUREFACTORY_HXX */
