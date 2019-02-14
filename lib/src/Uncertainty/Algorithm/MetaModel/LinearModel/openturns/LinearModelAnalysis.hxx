//                                               -*- C++ -*-
/**
 *  @brief The linear model analysis
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LINEARMODELANALYSIS_HXX
#define OPENTURNS_LINEARMODELANALYSIS_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Description.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/Graph.hxx"
#include "openturns/LinearModelResult.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelAnalysis
 *
 * The linear model analysis
 */

class OT_API LinearModelAnalysis :
  public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  LinearModelAnalysis();

  /** Parameter constructor */
  explicit LinearModelAnalysis(const LinearModelResult & linearModelResult);

  /** Virtual constructor */
  LinearModelAnalysis * clone() const;

  /** String converter */
  String __repr__() const;

  /** Method that returns the ANOVA table (ANalyse Of VAriance) */
  String __str__(const String & offset = "") const;

  /** Linear model accessor */
  LinearModelResult getLinearModelResult() const;

  /** Accessors to data from ANOVA table */
  Point getCoefficientsStandardErrors() const;
  Point getCoefficientsTScores() const;
  Point getCoefficientsPValues() const;
  Interval getCoefficientsConfidenceInterval(const Scalar level = 0.95) const;

  /** R-squared */
  Scalar getRSquared() const;

  /** Adjusted R-squared */
  Scalar getAdjustedRSquared() const;

  /** Fisher test */
  Scalar getFisherScore() const;
  Scalar getFisherPValue() const;

  /** Kolmogorov-Smirnov normality test */
  TestResult getNormalityTestResultKolmogorovSmirnov() const;

  /** Anderson-Darling normality test */
  TestResult getNormalityTestResultAndersonDarling() const;

  /** Chi-Squared normality test */
  TestResult getNormalityTestResultChiSquared() const;

  /** Cramer Von mises normality test */
  TestResult getNormalityTestCramerVonMises() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


  /* [0] Draw model versus fitted values */
  Graph drawModelVsFitted() const;
  
  /** [1] Draw a plot of residuals versus fitted values  */
  Graph drawResidualsVsFitted() const;

  /** [2] a Scale-Location plot of sqrt(| residuals |) versus fitted values */
  Graph drawScaleLocation() const;

  /** [3] a Normal quantiles-quantiles plot of standardized residuals */
  Graph drawQQplot() const;

  /** [4] a plot of Cook's distances versus row labels */
  Graph drawCookDistance() const;

  /** [5] a plot of residuals versus leverages that adds bands corresponding to Cook's distances of 0.5 and 1. */
  Graph drawResidualsVsLeverages() const;

  /** [6] a plot of Cook's distances versus leverage/(1-leverage) */
  Graph drawCookVsLeverages() const;

private:

  /** linear model result */
  LinearModelResult linearModelResult_;

}; /* class LinearModelAnalysis */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELANALYSIS_HXX */
