//                                               -*- C++ -*-
/**
 *  @brief This class implements a non parametric density estimator for copulas
 *         based on the Bernstein copula approximation of the empirical copula
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_BERNSTEINCOPULAFACTORY_HXX
#define OPENTURNS_BERNSTEINCOPULAFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/EmpiricalBernsteinCopula.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BernsteinCopulaFactory
 *
 * The class describes the probabilistic concept of BernsteinCopulaFactory.
 */
class OT_API BernsteinCopulaFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  BernsteinCopulaFactory();

  /** Virtual constructor */
  virtual BernsteinCopulaFactory * clone() const;

  /** Build a Bernstein copula based on the given sample. The bin number is computed according to the inverse power rule */
  using DistributionFactoryImplementation::build;
  virtual Distribution build() const;

  virtual Distribution build(const Sample & sample) const;

  virtual Distribution build(const Sample & sample,
                             const String & method,
                             const Function & objective = Function()) const;

  /** Build a Bernstein copula based on the given sample and bin number */
  virtual Distribution build(const Sample & sample,
                             const UnsignedInteger binNumber) const;

  /** Build a Bernstein copula based on the given sample and bin number as a EmpiricalBernsteinCopula */
  EmpiricalBernsteinCopula buildAsEmpiricalBernsteinCopula() const;

  EmpiricalBernsteinCopula buildAsEmpiricalBernsteinCopula(const Sample & sample,
      const String & method = ResourceMap::GetAsString("BernsteinCopulaFactory-BinNumberSelectionMethod"),
      const Function & objective = Function()) const;

  EmpiricalBernsteinCopula buildAsEmpiricalBernsteinCopula(const Sample & sample,
      const UnsignedInteger binNumber) const;

  /** Compute the number of bins according to the inverse power rule */
  static UnsignedInteger ComputeAMISEBinNumber(const Sample & sample);

  /** Compute the number of bins according to cross-validation maximum log-likelihood */
  static UnsignedInteger ComputeLogLikelihoodBinNumber(const Sample & sample,
      const UnsignedInteger kFraction = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-kFraction"));

  /** Compute the number of bins according to penalized Csiszar divergence */
  static UnsignedInteger ComputePenalizedCsiszarDivergenceBinNumber(const Sample & sample,
      const Function & f,
      const Scalar alpha = ResourceMap::GetAsScalar("BernsteinCopulaFactory-alpha"));

  /** Build all the learning/validation partitions of the given sample */
  static void BuildCrossValidationSamples(const Sample & sample,
                                          const UnsignedInteger kFraction,
                                          Collection<Sample> & learningCollection,
                                          Collection<Sample> & validationCollection);

private:

  /** Find the best bin number using an exhaustive search between two bounds given throug ResourceMap, or between 1 and size/2 if the bounds are in reverse order */
  static UnsignedInteger FindBestBinNumberSequential(const Function & mObjective,
      const UnsignedInteger mMin,
      const UnsignedInteger mMax);

}; /* class BernsteinCopulaFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BERNSTEINCOPULAFACTORY_HXX */
