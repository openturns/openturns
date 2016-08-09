//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
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
#ifndef OPENTURNS_KARHUNENLOEVEP1FACTORY_HXX
#define OPENTURNS_KARHUNENLOEVEP1FACTORY_HXX

#include "openturns/Mesh.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Basis.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveP1Factory
 */

class OT_API KarhunenLoeveP1Factory
  : public PersistentObject
{

  CLASSNAME;

public:

  /** Default constructor without parameters */
  KarhunenLoeveP1Factory();

  /** Default constructor without parameters */
  KarhunenLoeveP1Factory(const Mesh & mesh,
                         const NumericalScalar threshold);

  /** Virtual copy constructor */
  virtual KarhunenLoeveP1Factory * clone() const;

  /** Solve the Fredholm eigenvalues problem:
   * find (\phi_k, \lambda_k) such that
   * \int_{D} C(s,t)\phi_k(s)ds=\lambda_k\phi_k(t)
   * where C is a given covariance model, using P1 approximation
   */
  Basis build(const CovarianceModel & covarianceModel,
              NumericalPoint & eigenvalues) const;

  ProcessSample buildAsProcessSample(const CovarianceModel & covarianceModel,
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
  /** Underlying mesh */
  Mesh mesh_;

  /** Gram matrix of the mesh */
  CovarianceMatrix gram_;

  /** Threshold for eigenvalues selection */
  NumericalScalar threshold_;

} ; /* class KarhunenLoeveP1Factory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEP1FACTORY_HXX */
