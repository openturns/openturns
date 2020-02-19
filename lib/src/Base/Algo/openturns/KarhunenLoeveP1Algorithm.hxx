//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
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
#ifndef OPENTURNS_KARHUNENLOEVEP1ALGORITHM_HXX
#define OPENTURNS_KARHUNENLOEVEP1ALGORITHM_HXX

#include "openturns/KarhunenLoeveAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveP1Algorithm
 */

class OT_API KarhunenLoeveP1Algorithm
  : public KarhunenLoeveAlgorithmImplementation
{

  CLASSNAME

public:
  
  /** Default constructor without parameters */
  KarhunenLoeveP1Algorithm();

  /** Constructor with parameters */
  KarhunenLoeveP1Algorithm( const Mesh & mesh,
                            const CovarianceModel & covariance,
                            const Scalar threshold = 0.0
                          );

  /** Virtual copy constructor */
  virtual KarhunenLoeveP1Algorithm * clone() const;

  /** Solve the Fredholm eigenvalues problem:
   * find (\phi_k, \lambda_k) such that
   * \int_{D} C(s,t)\phi_k(s)ds=\lambda_k\phi_k(t)
   * where C is a given covariance model, using P1 approximation
   */
  void run();
  
  /** Mesh accessor */
  Mesh getMesh() const;
  
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

} ; /* class KarhunenLoeveP1Algorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEP1ALGORITHM_HXX */
