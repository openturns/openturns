//                                               -*- C++ -*-
/**
 *  @brief The class enables to get the coefficients of an ARMA process using its spectral density function
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
#ifndef OPENTURNS_WHITTLEFACTORY_HXX
#define OPENTURNS_WHITTLEFACTORY_HXX

#include "openturns/ARMAFactoryImplementation.hxx"
#include "openturns/WelchFactory.hxx"
#include "openturns/WhittleFactoryState.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WhittleFactory
 *
 * The class implements the Whittle likelihood for estimating ARMA coefficients
 */
class OT_API WhittleFactory
  : public ARMAFactoryImplementation
{
  CLASSNAME

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
  WhittleFactory * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** SpectralModelFactory accessors - Make easy change of periodogram estimate */
  WelchFactory getSpectralModelFactory() const;
  void setSpectralModelFactory(const WelchFactory & factory);

  /** Build method ==> estimating the coefficients */
  ARMA build(const TimeSeries & timeSeries) const override;
  ARMA buildWithCriteria(const TimeSeries & timeSeries,
                         Point & informationCriteriaOut) const;
  ARMA build(const ProcessSample & sample) const override;
  ARMA buildWithCriteria(const ProcessSample & sample,
                         Point & informationCriteriaOut) const;

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
  void setStartingPoints(const Collection< Point > & startingPoints);
  Collection< Point > getStartingPoints() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private :

  /** Likelihood function ==> Compute the reduced form of the likelihood */
  Scalar computeLogLikelihood(const Point & theta) const;

  /** Log likelihood maximization, shared among the different build() methods */
  ARMA maximizeLogLikelihood(Point & InformationCriterion) const;

  /** Compute the spectral density at the relevant normalized frequencies */
  void computeSpectralDensity(const UserDefinedSpectralModel & spectralModel) const;

  /** Estimate the periodogram */
  void buildSpectralDensity(const TimeSeries & timeSeries) const;
  void buildSpectralDensity(const ProcessSample & sample) const;

  /** Initialize the starting points using the ResourceMap */
  void initializeStartingPoints();

  /** Welch factory used to estimate the spectral model */
  WelchFactory spectralFactory_;

  /** Frequency grid to be used in internal */
  mutable Point normalizedFrequencies_;

  /** Time grid associated with the given data */
  mutable RegularGrid timeGrid_;

  /** Number of frequecies - Used to pass data to be used in computeLogLikeliHood */
  mutable UnsignedInteger m_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable Point spectralDensity_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable Scalar sigma2_;

  /** Verbosity control */
  Bool verbose_;

  /** History flag */
  mutable Bool isHistoryEnabled_;

  /** History */
  mutable PersistentCollection< WhittleFactoryState > history_;

  /** Starting points for the estimation process */
  PersistentCollection< Point > startingPoints_;

  /** Likelihood function accessor */
  Function getLogLikelihoodFunction() const;

  /** likelihood estimate */
  Point computeLogLikelihoodInequalityConstraint( const Point & theta ) const;

  /** only used to pass data to be used in computeLogLikelihoodInequalityConstraint */
  mutable UnsignedInteger nbInequalityConstraint_;

  /** Likelihood constraint accessor */
  Function getLogLikelihoodInequalityConstraint() const;

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

  /** Initialize default Cobyla solver parameter using the ResourceMap */
  void initializeCobylaSolverParameter();

protected:

  /** Optimization solver */
  mutable OptimizationAlgorithm  solver_;

}; /* class WhittleFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WHITTLEFACTORY_HXX */
