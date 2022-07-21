//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         quadrature approximation.
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
#ifndef OPENTURNS_KARHUNENLOEVEQuadratureALGORITHM_HXX
#define OPENTURNS_KARHUNENLOEVEQuadratureALGORITHM_HXX

#include "openturns/KarhunenLoeveAlgorithmImplementation.hxx"
#include "openturns/Domain.hxx"
#include "openturns/Interval.hxx"
#include "openturns/WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveQuadratureAlgorithm
 */

class OT_API KarhunenLoeveQuadratureAlgorithm
  : public KarhunenLoeveAlgorithmImplementation
{

  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Default constructor without parameters */
  KarhunenLoeveQuadratureAlgorithm();

  /** Parameter constructor */
  KarhunenLoeveQuadratureAlgorithm(const Domain & domain,
                                   const Interval & domainBounds,
                                   const CovarianceModel & covariance,
                                   const WeightedExperiment & experiment,
                                   const FunctionCollection & basis,
                                   const Bool mustScale,
                                   const Scalar threshold = 0.0);

  /** Parameter constructor for the Legendre/GaussProduct case */
  KarhunenLoeveQuadratureAlgorithm(const Domain & domain,
                                   const Interval & domainBounds,
                                   const CovarianceModel & covariance,
                                   const UnsignedInteger marginalDegree,
                                   const Scalar threshold = 0.0);

  /** Virtual copy constructor */
  KarhunenLoeveQuadratureAlgorithm * clone() const override;

  /** Solve the Fredholm eigenvalues problem:
   * find (\phi_k, \lambda_k) such that
   * \int_{D} C(s,t)\phi_k(s)ds=\lambda_k\phi_k(t)
   * where C is a given covariance model, using Quadrature approximation
   */
  void run() override;

  /** Domain accessor */
  Domain getDomain() const;

  /** Experiment accessor */
  WeightedExperiment getExperiment() const;

  /** Basis accessor */
  FunctionCollection getBasis() const;

  /** MustScale accessor */
  Bool getMustScale() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Underlying domain */
  Domain domain_;

  /** Domain lower bound*/
  Point domainLowerBound_;

  /** Domain upper bound*/
  Point domainUpperBound_;

  /** Experiment */
  WeightedExperiment experiment_;

  /** Basis */
  FunctionPersistentCollection basis_;

  /** Scaling flag */
  Bool mustScale_;
} ; /* class KarhunenLoeveQuadratureAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEQuadratureALGORITHM_HXX */
