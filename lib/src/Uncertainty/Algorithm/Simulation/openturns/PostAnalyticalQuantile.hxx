//                                               -*- C++ -*-
/**
 *  @brief FORM/SORM quantile estimator
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_POSTANALYTICALQUANTILE_HXX
#define OPENTURNS_POSTANALYTICALQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PostAnalyticalQuantile
 *
 * Quantile obtained from the FORM or SORM analytical probability.
 * The analytical probability is a deterministic function of the
 * threshold, so the quantile is the root of P_FORM(t)=1-alpha.  An
 * optional importance-sampling refinement replaces the analytical
 * probability with a weighted empirical probability evaluated on a
 * fixed sample drawn around the design point; independent replicates
 * then provide an asymptotic interval.
 *
 * References:
 * R. Rackwitz. Reliability analysis, a review and some perspectives.
 * Structural Safety, 23(4):365-395, 2001.  This article reviews the
 * FORM and SORM approximations and the reliability index used here.
 * M. Hohenbichler and R. Rackwitz. Improvement of second-order
 * reliability estimates by importance sampling. Journal of
 * Engineering Mechanics, 114(12):2195-2199, 1988.  This article
 * introduces the importance-sampling correction around the design
 * point used by the refinement.
 */
class OT_API PostAnalyticalQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Analytical method */
  enum AnalyticalMethod
  {
    FORM_APPROXIMATION,
    SORM_APPROXIMATION
  };

  /** SORM curvature formula */
  enum SORMFormula
  {
    BREITUNG,
    HOHENBICHLER,
    TVEDT
  };

  /** Default constructor */
  PostAnalyticalQuantile();

  /** Constructor with parameters */
  PostAnalyticalQuantile(const RandomVector & output,
                         const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                         const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Constructor with parameters and nearest point algorithm */
  PostAnalyticalQuantile(const OptimizationAlgorithm & nearestPointAlgorithm,
                         const RandomVector & output,
                         const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                         const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  PostAnalyticalQuantile * clone() const override;

  /** Nearest point algorithm accessor */
  void setNearestPointAlgorithm(const OptimizationAlgorithm & nearestPointAlgorithm);
  OptimizationAlgorithm getNearestPointAlgorithm() const;

  /** Analytical method accessor */
  void setAnalyticalMethod(const AnalyticalMethod method);
  AnalyticalMethod getAnalyticalMethod() const;
  String getAnalyticalMethodDescription() const;

  /** SORM formula accessor */
  void setSORMFormula(const SORMFormula formula);
  SORMFormula getSORMFormula() const;
  String getSORMFormulaDescription() const;

  /** Importance-sampling refinement accessor */
  void setRefinementEnabled(const Bool refinementEnabled);
  Bool getRefinementEnabled() const;

  /** Refinement replicate count accessor */
  void setReplicateCount(const UnsignedInteger replicateCount);
  UnsignedInteger getReplicateCount() const;

  /** Root-finding tolerance accessor */
  void setRootTolerance(const Scalar tolerance);
  Scalar getRootTolerance() const;

  /** Maximum root-finding iteration accessor */
  void setMaximumRootIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumRootIteration() const;

  /** Pilot sample size accessor */
  void setPilotSampleSize(const UnsignedInteger pilotSampleSize);
  UnsignedInteger getPilotSampleSize() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the analytical root-finding experiment */
  QuantileSimulationResult runFixedSample() override;

  /** Check the inputs without requiring a sampling size */
  void checkInputs() const override;

private:
  /** Result of one analytical run at a threshold */
  struct AnalyticalEvaluation
  {
    Scalar probability = 0.0;
    Scalar formProbability = 0.0;
    Point designPoint;
    Scalar reliabilityIndex = 0.0;
    Bool hasCurvatureCorrection = false;
    Scalar curvatureCorrection = 1.0;
  };

  /** Run FORM or SORM at the given threshold */
  AnalyticalEvaluation runAnalytical(const RandomVector & compositeOutput,
                                     const Scalar threshold) const;

  /** Weighted empirical probability of a fixed importance sample */
  Scalar computeImportanceProbability(const Point & values,
                                      const Point & weights,
                                      const Scalar threshold) const;

  OptimizationAlgorithm nearestPointAlgorithm_;
  AnalyticalMethod analyticalMethod_ = FORM_APPROXIMATION;
  SORMFormula sormFormula_ = HOHENBICHLER;
  Bool refinementEnabled_ = false;
  UnsignedInteger replicateCount_ = ResourceMap::GetAsUnsignedInteger("PostAnalyticalQuantile-DefaultReplicateCount");
  Scalar rootTolerance_ = ResourceMap::GetAsScalar("QuantileRootSolver-DefaultTolerance");
  UnsignedInteger maximumRootIteration_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultMaximumIteration");
  UnsignedInteger pilotSampleSize_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-PilotSampleSize");

  /** State cached for the duration of a run */
  Function standardFunction_;
  Distribution standardDistribution_;

}; /* class PostAnalyticalQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POSTANALYTICALQUANTILE_HXX */
