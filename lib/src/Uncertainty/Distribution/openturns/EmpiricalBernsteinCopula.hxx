//                                               -*- C++ -*-
/**
 *  @brief The class that implements the Bernstein approximation of the empirical copula
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
#ifndef OPENTURNS_EMPIRICALBERNSTEINCOPULA_HXX
#define OPENTURNS_EMPIRICALBERNSTEINCOPULA_HXX

#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EmpiricalBernsteinCopula
 *
 * The class describes the probabilistic concept of copulas
 * made from the empirical compula and a Bernstein approximation
 */
class OT_API EmpiricalBernsteinCopula
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  EmpiricalBernsteinCopula();

  /** Default constructor */
  EmpiricalBernsteinCopula(const Sample & copulaSample,
                           const UnsignedInteger binNumber,
                           const Bool isEmiricalCopulaSample = false);

  /** Comparison operator */
  Bool operator ==(const EmpiricalBernsteinCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Copula sample accessor */
  void setCopulaSample(const Sample & copulaSample,
                       const Bool isEmpiricalCopulaSample = false);
  Sample getCopulaSample() const;

  /** Bin number accessor */
  void setBinNumber(const UnsignedInteger binNumber);
  UnsignedInteger getBinNumber() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual EmpiricalBernsteinCopula * clone() const;

  /** Get one realization of the EmpiricalBernsteinCopula */
  Point getRealization() const;

  /** Get a sample of the distribution */
  Sample getSample(const UnsignedInteger size) const;

  /** Get the PDF of the EmpiricalBernsteinCopula */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the log-PDF of the EmpiricalBernsteinCopula */
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the EmpiricalBernsteinCopula */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using ContinuousDistribution::getMarginal;
  Distribution getMarginal(const UnsignedInteger i) const;
  Distribution getMarginal(const Indices & indices) const;

  /** Get the Spearman correlation of the distribution */
  CorrelationMatrix getSpearmanCorrelation() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Parameter accessor */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


private:

  /** Default constructor */
  EmpiricalBernsteinCopula(const Sample & copulaSample,
                           const UnsignedInteger binNumber,
                           const SampleImplementation & logBetaMarginalFactors,
                           const SampleImplementation & logFactors);

  /** Compute the normalization factors */
  void update();

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The underlying sample */
  Sample copulaSample_;

  /** Bin number */
  UnsignedInteger binNumber_;

  /** Normalization factors */
  Point logBetaFactors_;
  SampleImplementation logBetaMarginalFactors_;
  SampleImplementation logFactors_;
  MatrixImplementation logFactorsMinus1_;

}; /* class EmpiricalBernsteinCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EMPIRICALBERNSTEINCOPULA_HXX */
