//                                               -*- C++ -*-
/**
 *  @brief The class enables to get the coefficients of an ARMA process using the likelihood function
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
#ifndef OPENTURNS_ARMALIKELIHOODFACTORY_HXX
#define OPENTURNS_ARMALIKELIHOODFACTORY_HXX

#include "openturns/ARMAFactoryImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ARMALikelihoodFactory
 *
 * The class implements the classical likelihood for estimating ARMA coefficients
 */
class OT_API ARMALikelihoodFactory
  : public ARMAFactoryImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  ARMALikelihoodFactory();

  /** Default constructor */
  ARMALikelihoodFactory(const UnsignedInteger p,
                        const UnsignedInteger q,
                        const UnsignedInteger dimension,
                        const Bool invertible = true);

  /** Parameter constructor */
  ARMALikelihoodFactory(const Indices & p,
                        const Indices & q,
                        const UnsignedInteger dimension,
                        const Bool invertible = true);

  /** Virtual constructor */
  ARMALikelihoodFactory * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** set accessor for starting point of the optimization
   *  We also add a global set method
   */
  void setInitialARCoefficients(const ARMACoefficients & phi);
  void setInitialMACoefficients(const ARMACoefficients & theta);
  void setInitialCovarianceMatrix(const CovarianceMatrix & covarianceMatrix);
  void setInitialConditions(const ARMACoefficients & arCoefficients, const ARMACoefficients & maCoefficients, const CovarianceMatrix & covarianceMatrix);

  /** get accessor for starting point of the optimization */
  ARMACoefficients getInitialARCoefficients() const;
  ARMACoefficients getInitialMACoefficients() const;
  CovarianceMatrix getInitialCovarianceMatrix() const;

  /** Build method ==> estimating the coefficients */
  ARMA build(const TimeSeries & timeSeries) const override;
  ARMA build(const ProcessSample & sample) const override;

  /** Verbosity accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private :

  /** Parameter g is the maximum of p and q */
  mutable UnsignedInteger currentG_;

  /** TimeSeries used to pass data */
  mutable TimeSeries w_;

  /** Dimension parameter - only used to pass data */
  UnsignedInteger dimension_;

  /** only used to pass data to be used in computeLogLikelihood */
  mutable CovarianceMatrix covarianceMatrix_;
  mutable SquareMatrix covarianceMatrixCholesky_;
  mutable SquareMatrix covarianceMatrixCholeskyInverse_;

  /** autocovariance matrix ==> matrix of size (dimension, max(1,p) * dimension)
   * only used to pass data to be used in computeLogLikelihood */
  mutable Matrix autoCovariance_;

  /** crosscovariance matrix  ==> matrix of size (dimension, max(1,q) * dimension)
   * only used to pass data to be used in computeLogLikelihood */
  mutable Matrix crossCovariance_;

  /** CoefficientsBlockMatrix ==> matrix of size (dimension , (p + q) * dimension)
   * only used to pass data to be used in computeLogLikelihood
   * Encapsulate both AR coefficients \phi, MA coefficients \theta
   * Sign conventions are different between OpenTurns and Mauricio's papers, so a
   * sign change is performed by accessors.  In order to improve performance during
   * matrix multiplication, transposed matrices are stored. */
  mutable Matrix blockPhiTThetaTMatrix_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable Scalar sigma2_;

  /** Bool variables */
  mutable Bool hasInitializedARCoefficients_;
  mutable Bool hasInitializedMACoefficients_;
  mutable Bool hasInitializedCovarianceMatrix_;

  /** Verbosity control */
  Bool verbose_;

  /** Method that initialize the size of matrices and vectors depending on the used couple (p, q) */
  void initialize();

  /** Compute W0 matrix */
  SquareMatrix computeW0Matrix() const;

  /** Likelihood function ==> Compute the reduced form of the likelihood */
  Scalar computeLogLikelihood(const Point & beta) const;

  /** Run the default initilization of coefficients / covariance for the optimization */
  void defaultInitialize() const;

  /** Compute the autocovariance matrix - This method is public for validation purposes*/
  void computeAutocovarianceMatrix() const;

  /** Compute the cross-covariance matrix - This method is public for validation purposes */
  void computeCrossCovarianceMatrix() const;

  /** Compute the Cholesky factor of V1 Omega V1^{T} */
  SquareMatrix computeV1_Omega_V1T_Cholesky() const;

  /** Compute R xi matrices */
  Matrix computeRXi() const;

  /** Compute eta matrices */
  Matrix computeEta() const;

  /** Compute h vectors */
  Point computeVectorh(const Matrix & rxi, const Matrix & eta, const Matrix & matV1_Omega_V1TCholesky) const;

  /** Compute H^{T} H matrix */
  SymmetricMatrix computeHTH(const Matrix & rxi) const;

  /** Compute I + M^{T} H^{T} H M matrix */
  CovarianceMatrix computeI_MTHTHM(const SymmetricMatrix & matrix_HTH, const Matrix & matV1_Omega_V1TCholesky) const;

  /** Likelihood function accessor */
  Function getLogLikelihoodFunction() const;

  /** Likelihood constraint accessor */
  Function getLogLikelihoodInequalityConstraint() const;

  /** likelihood estimate */
  Point computeLogLikelihoodInequalityConstraint( const Point & beta ) const;

  /** only used to pass data to be used in computeLogLikelihood and computeLogLikelihoodInequalityConstraint  */
  mutable UnsignedInteger inputDimension_;

  /** only used to pass data to be used in computeLogLikelihoodInequalityConstraint */
  mutable UnsignedInteger nbInequalityConstraint_;

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Initialize default Cobyla solver parameter using the ResourceMap */
  void initializeCobylaSolverParameter();

protected:

  /** Optimization solver */
  mutable OptimizationAlgorithm  solver_;

}; /* class ARMALikelihoodFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ARMALIKELIHOODFACTORY_HXX */
