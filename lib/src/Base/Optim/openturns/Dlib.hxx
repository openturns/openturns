//                                               -*- C++ -*-
/**
 *  @brief Dlib solvers
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
#ifndef OPENTURNS_DLIB_HXX
#define OPENTURNS_DLIB_HXX

#include "openturns/OTconfig.hxx"
#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Dlib
 * Dlib solver
 */
class OT_API Dlib :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  Dlib();

  /** Constructors with parameters */
  Dlib(const String & algoName);

  Dlib(const OptimizationProblem & problem,
       const String & algoName);

  /** Virtual constructor */
  Dlib * clone() const override;

  /** Dlib static methods */
  static Bool IsAvailable();  // Check whether Dlib support is available
  static Description GetAlgorithmNames(); // List available dlib search strategies

  /** ACCESSORS */
  // Name of the algorithm
  String getAlgorithmName();
  void setAlgorithmName(const String algoName);

  // wolfeRho parameter, used in line search process
  Scalar getWolfeRho() const;
  void setWolfeRho(const Scalar wolfeRho);

  // wolfeSigma parameter, used in line search process
  Scalar getWolfeSigma() const;
  void setWolfeSigma(const Scalar wolfeSigma);

  // Maximum number of line search iterations
  UnsignedInteger getMaxLineSearchIterations() const;
  void setMaxLineSearchIterations(const UnsignedInteger maxLineSearchIterations);

  // Maximum amount of memory used during L-BFGS optimization
  UnsignedInteger getMaxSize() const;
  void setMaxSize(const UnsignedInteger maxSize);

  // Initial radius of the trust region used in "TrustRegion" and least squares algorithms
  Scalar getInitialTrustRegionRadius() const;
  void setInitialTrustRegionRadius(const Scalar radius);

  /** Performs the actual computation. Must be overloaded by the actual optimization algorithm */
  void run() override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const override;

private:
  // Name of the optimization algorithm to use
  String algoName_;

  // Parameters of the line search performed in some algorithms
  Scalar wolfeRho_;
  Scalar wolfeSigma_;
  UnsignedInteger maxLineSearchIterations_;

  // Maximum size used by L-BFGS algorithm
  UnsignedInteger maxSize_;

  // Initial trust region radius to use in "TrustRegion", "LSQ" and "LSQLM" algorithms
  Scalar initialTrustRegionRadius_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DLIB_HXX */
