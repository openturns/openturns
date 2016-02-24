//                                               -*- C++ -*-
/**
 *  @brief The class enables to get the coefficients of an ARMA process using its spectral density function
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_WHITTLEFACTORY_HXX
#define OPENTURNS_WHITTLEFACTORY_HXX

#include "openturns/ARMAFactoryImplementation.hxx"
#include "openturns/SpectralModelFactory.hxx"
#include "openturns/WhittleFactoryState.hxx"
#include "openturns/OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WhittleFactory
 *
 * The class implements the Whittle likelihood for estimating ARMA coefficients
 */
class OT_API WhittleFactory
  : public ARMAFactoryImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  WhittleFactory();

  /** Default constructor */
  WhittleFactory(const UnsignedInteger p,
                 const UnsignedInteger q,
                 const Bool invertible = true);

  /** Default constructor */
  WhittleFactory(const Indices & p,
                 const Indices & q,
                 const Bool invertible = true);

  /** Virtual constructor */
  virtual WhittleFactory * clone() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** SpectralModelFactory acccessors - Make easy change of periodogram estimate */
  SpectralModelFactory getSpectralModelFactory() const;
  void setSpectralModelFactory(const SpectralModelFactory & factory);

  /** Build method ==> estimating the coefficients */
  ARMA build(const TimeSeries & timeSeries,
             NumericalPoint & informationCriteria) const;
  ARMA build(const TimeSeries & timeSeries) const;
  ARMA build(const ProcessSample & sample,
             NumericalPoint & informationCriteria) const;
  ARMA build(const ProcessSample & sample) const;

  /** Verbosity accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

  /** Enable or disable the estimation history */
  void enableHistory() const;
  void disableHistory() const;
  Bool isHistoryEnabled() const;
  void clearHistory() const;
  Collection< WhittleFactoryState > getHistory() const;

  /** Starting points accessor */
  void setStartingPoints(const Collection< NumericalPoint > & startingPoints);
  Collection< NumericalPoint > getStartingPoints() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private :

  /** Likelihood function ==> Compute the reduced form of the likelihood */
  NumericalScalar computeLogLikelihood(const NumericalPoint & theta) const;

  /** Log likelihood maximization, shared among the different build() methods */
  ARMA maximizeLogLikelihood(NumericalPoint & InformationCriterion) const;

  /** Compute the spectral density at the relevant normalized frequencies */
  void computeSpectralDensity(const SpectralModel & spectralModel) const;

  /** Estimate the periodogram */
  void buildSpectralDensity(const TimeSeries & timeSeries) const;
  void buildSpectralDensity(const ProcessSample & sample) const;

  /** Initialize the starting points using the ResourceMap */
  void initializeStartingPoints();

  /** Spectral model factory ==> enables user to set anotherparametrized method*/
  SpectralModelFactory spectralFactory_;

  /** Frequency grid to be used in internal */
  mutable NumericalPoint normalizedFrequencies_;

  /** Time grid associated with the given data */
  mutable RegularGrid timeGrid_;

  /** Number of frequecies - Used to pass data to be used in computeLogLikeliHood */
  mutable UnsignedInteger m_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable NumericalPoint spectralDensity_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable NumericalScalar sigma2_;

  /** Verbosity control */
  Bool verbose_;

  /** History flag */
  mutable Bool isHistoryEnabled_;

  /** History */
  mutable PersistentCollection< WhittleFactoryState > history_;

  /** Starting points for the estimation process */
  PersistentCollection< NumericalPoint > startingPoints_;

  /** Likelihood function accessor */
  NumericalMathFunction getLogLikelihoodFunction() const;

  /** likelihood estimate */
  NumericalPoint computeLogLikelihoodInequalityConstraint( const NumericalPoint & theta ) const;

  /** only used to pass data to be used in computeLogLikelihoodInequalityConstraint */
  mutable UnsignedInteger nbInequalityConstraint_;

  /** Likelihood constraint accessor */
  NumericalMathFunction getLogLikelihoodInequalityConstraint() const;

  /** Optimization solver accessor */
  OptimizationSolver getOptimizationSolver() const;
  void setOptimizationSolver(const OptimizationSolver & solver);

  /** Initialize default Cobyla solver parameter using the ResourceMap */
  void initializeCobylaSolverParameter();

protected:

  /** Optimization solver */
  mutable OptimizationSolver  solver_;

}; /* class WhittleFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WHITTLEFACTORY_HXX */
