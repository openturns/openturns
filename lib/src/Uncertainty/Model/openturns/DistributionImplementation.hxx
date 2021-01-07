//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DISTRIBUTIONIMPLEMENTATION_HXX
#define OPENTURNS_DISTRIBUTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Interval.hxx"
#include "openturns/LevelSet.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Description.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/PiecewiseHermiteEvaluation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class Distribution;

/**
 * @class DistributionImplementation
 *
 * The class describes the probabilistic concept of distribution.
 * Instances of this class can produce samples following the
 * distribution, can compute PDF or CDF, etc.
 * They are the actual key component of RandomVectors.
 */
class OT_API DistributionImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Pointer<DistributionImplementation>       Implementation;
  typedef Function                                  IsoProbabilisticTransformation;
  typedef IsoProbabilisticTransformation            InverseIsoProbabilisticTransformation;
  typedef Collection<Point>                PointCollection;
  typedef Collection<PointWithDescription> PointWithDescriptionCollection;

  /** Default constructor */
  DistributionImplementation();

  /** Comparison operator */
  Bool operator ==(const DistributionImplementation & other) const;
protected:
  virtual Bool equals(const DistributionImplementation & other) const;
public:
  Bool operator !=(const DistributionImplementation & other) const;

  /** Addition operator */
  Distribution operator + (const DistributionImplementation & other) const;
  Distribution operator + (const Distribution & other) const;

  Distribution operator + (const Scalar value) const;

  /** Subtraction operator */
  Distribution operator - (const DistributionImplementation & other) const;
  Distribution operator - (const Distribution & other) const;

  Distribution operator - (const Scalar value) const;

  /** Multiplication operator */
  Distribution operator * (const DistributionImplementation & other) const;
  Distribution operator * (const Distribution & other) const;

  Distribution operator * (const Scalar value) const;

  /** Division operator */
  Distribution operator / (const DistributionImplementation & other) const;
  Distribution operator / (const Distribution & other) const;

  Distribution operator / (const Scalar value) const;

  /** Methods to transform distributions by usual functions */
  Distribution cos() const;
  Distribution sin() const;
  Distribution tan() const;

  Distribution acos() const;
  Distribution asin() const;
  Distribution atan() const;

  Distribution cosh() const;
  Distribution sinh() const;
  Distribution tanh() const;

  Distribution acosh() const;
  Distribution asinh() const;
  Distribution atanh() const;

  Distribution exp() const;
  Distribution log() const;
  Distribution ln() const;

  Distribution pow(const SignedInteger exponent) const;
  Distribution pow(const Scalar exponent) const;
  Distribution inverse() const;
  Distribution sqr() const;
  Distribution sqrt() const;
  Distribution cbrt() const;
  Distribution abs() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Weight accessor */
  void setWeight(Scalar w);
  Scalar getWeight() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  DistributionImplementation * clone() const override;

  /** Get one realization of the distribution */
  virtual Point getRealization() const;
protected:
  virtual Point getRealizationByInversion() const;

public:
  /** Get a numerical sample whose elements follow the distributionImplementation */
  virtual Sample getSample(const UnsignedInteger size) const;
protected:
  virtual Sample getSampleByInversion(const UnsignedInteger size) const;
  virtual Sample getSampleByQMC(const UnsignedInteger size) const;

public:
  /** Get the DDF of the distribution */
  virtual Scalar computeDDF(const Scalar scalar) const;
  virtual Point  computeDDF(const Point & point) const;
  virtual Sample computeDDF(const Sample & sample) const;
protected:
  virtual Sample computeDDFSequential(const Sample & sample) const;
  virtual Sample computeDDFParallel(const Sample & sample) const;
public:

  /** Get the PDF of the distribution */
  virtual Scalar computePDF(const Scalar scalar) const;
  virtual Scalar computePDF(const Point & point) const;
  virtual Sample computePDF(const Sample & sample) const;
protected:
  virtual Sample computePDFSequential(const Sample & sample) const;
  virtual Sample computePDFParallel(const Sample & sample) const;
public:

  virtual Scalar computeLogPDF(const Scalar scalar) const;
  virtual Scalar computeLogPDF(const Point & point) const;
  virtual Sample computeLogPDF(const Sample & sample) const;
protected:
  virtual Sample computeLogPDFSequential(const Sample & sample) const;
  virtual Sample computeLogPDFParallel(const Sample & sample) const;
public:

  /** Compute the PDF of 1D distributions over a regular grid */
  virtual Sample computePDF(const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            Sample & gridOut) const;

  /** Compute the PDF of nD distributions over a regular grid */
  virtual Sample computePDF(const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            Sample & gridOut) const;

  /** Compute the log-PDF of 1D distributions over a regular grid */
  virtual Sample computeLogPDF(const Scalar xMin,
                               const Scalar xMax,
                               const UnsignedInteger pointNumber,
                               Sample & gridOut) const;

  /** Compute the log-PDF of nD distributions over a regular grid */
  virtual Sample computeLogPDF(const Point & xMin,
                               const Point & xMax,
                               const Indices & pointNumber,
                               Sample & gridOut) const;

  /** Get the CDF of the distribution */
  virtual Scalar computeCDF(const Scalar scalar) const;
  virtual Scalar computeComplementaryCDF(const Scalar scalar) const;
  virtual Scalar computeSurvivalFunction(const Scalar scalar) const;

  virtual Scalar computeCDF(const Point & point) const;
  virtual Scalar computeComplementaryCDF(const Point & point) const;
  virtual Scalar computeSurvivalFunction(const Point & point) const;
  virtual Point computeInverseSurvivalFunction(const Scalar point) const;
#ifndef SWIG
  virtual Point computeInverseSurvivalFunction(const Scalar prob,
      Scalar & marginalProb) const;
#endif
protected:
  virtual Sample computeCDFSequential(const Sample & sample) const;
  virtual Sample computeCDFParallel(const Sample & sample) const;
public:
  virtual Sample computeCDF(const Sample & sample) const;
protected:
  virtual Sample computeSurvivalFunctionSequential(const Sample & sample) const;
  virtual Sample computeSurvivalFunctionParallel(const Sample & sample) const;
public:
  virtual Sample computeSurvivalFunction(const Sample & sample) const;
protected:
  virtual Sample computeComplementaryCDFSequential(const Sample & sample) const;
  virtual Sample computeComplementaryCDFParallel(const Sample & sample) const;
public:
  virtual Sample computeComplementaryCDF(const Sample & sample) const;

  /** Compute the CDF of 1D distributions over a regular grid */
  virtual Sample computeCDF(const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            Sample & gridOut) const;

  /** Compute the CDF of nD distributions over a regular grid */
  virtual Sample computeCDF(const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            Sample & gridOut) const;

  virtual Sample computeComplementaryCDF(const Scalar xMin,
                                         const Scalar xMax,
                                         const UnsignedInteger pointNumber,
                                         Sample & gridOut) const;

  /** Get the probability content of an interval */
  virtual Scalar computeProbability(const Interval & interval) const;
protected:
  /** Generic implementation for continuous distributions */
  virtual Scalar computeProbabilityContinuous(const Interval & interval) const;
  /** Generic implementation for discrete distributions */
  virtual Scalar computeProbabilityDiscrete(const Interval & interval) const;
  /** Generic implementation for general distributions */
  virtual Scalar computeProbabilityGeneral(const Interval & interval) const;
public:

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  virtual Complex computeCharacteristicFunction(const Scalar x) const;
  virtual Complex computeCharacteristicFunction(const Point & x) const;
  virtual Complex computeLogCharacteristicFunction(const Scalar x) const;
  virtual Complex computeLogCharacteristicFunction(const Point & x) const;

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  virtual Scalar computeGeneratingFunction(const Scalar z) const;
  virtual Scalar computeLogGeneratingFunction(const Scalar z) const;

  virtual Complex computeGeneratingFunction(const Complex & z) const;
  virtual Complex computeLogGeneratingFunction(const Complex & z) const;

  /** Compute the entropy of the distribution */
  virtual Scalar computeEntropy() const;

  /** Get the PDF gradient of the distribution */
  virtual Point computePDFGradient(const Point & point) const;
  virtual Sample computePDFGradient(const Sample & inSample) const;

  /** Get the logPDF gradient of the distribution */
  virtual Point computeLogPDFGradient(const Point & point) const;
  virtual Sample computeLogPDFGradient(const Sample & inSample) const;

protected:
  virtual Sample computeLogPDFGradientSequential(const Sample & sample) const;
  virtual Sample computeLogPDFGradientParallel(const Sample & sample) const;

public:
  /** Get the CDF gradient of the distribution */
  virtual Point computeCDFGradient(const Point & point) const;
  virtual Sample computeCDFGradient(const Sample & inSample) const;
public:

  /** Get the quantile of the distribution */
  virtual Point computeQuantile(const Scalar prob,
                                const Bool tail = false) const;
#ifndef SWIG
  virtual Point computeQuantile(const Scalar prob,
                                const Bool tail,
                                Scalar & marginalProb) const;
#endif
  /** Quantile computation for dimension=1 */
  virtual Scalar computeScalarQuantile(const Scalar prob,
                                       const Bool tail = false) const;

  /** Get the quantile over a provided grid */
protected:
  virtual Sample computeQuantileSequential(const Point & prob,
      const Bool tail = false) const;
  virtual Sample computeQuantileParallel(const Point & prob,
                                         const Bool tail = false) const;
public:
  virtual Sample computeQuantile(const Point & prob,
                                 const Bool tail = false) const;

  /** Compute the quantile over a regular grid */
  virtual Sample computeQuantile(const Scalar qMin,
                                 const Scalar qMax,
                                 const UnsignedInteger pointNumber,
                                 const Bool tail = false) const;

#ifndef SWIG
  virtual Sample computeQuantile(const Scalar qMin,
                                 const Scalar qMax,
                                 const UnsignedInteger pointNumber,
                                 Sample & grid,
                                 const Bool tail = false) const;
#endif

  /** Get the product minimum volume interval containing a given probability of the distribution */
  virtual Interval computeMinimumVolumeInterval(const Scalar prob) const;
  virtual Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

protected:
  Interval computeUnivariateMinimumVolumeIntervalByOptimization(const Scalar prob,
      Scalar & marginalProbOut) const;
  Interval computeUnivariateMinimumVolumeIntervalByRootFinding(const Scalar prob,
      Scalar & marginalProbOut) const;

public:

  /** Get the product bilateral confidence interval containing a given probability of the distribution */
  virtual Interval computeBilateralConfidenceInterval(const Scalar prob) const;
  virtual Interval computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the product unilateral confidence interval containing a given probability of the distribution */
  virtual Interval computeUnilateralConfidenceInterval(const Scalar prob, const Bool tail = false) const;
  virtual Interval computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, const Bool tail, Scalar & marginalProbOut) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  virtual LevelSet computeMinimumVolumeLevelSet(const Scalar prob) const;
  virtual LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const;

protected:
  virtual LevelSet computeUnivariateMinimumVolumeLevelSetByQMC(const Scalar prob,
      Scalar & thresholdOut) const;

public:

  /** Get the mathematical and numerical range of the distribution.
      Its mathematical range is the smallest closed interval outside
      of which the PDF is zero, and the numerical range is the interval
      outside of which the PDF is rounded to zero in double precision */
  virtual Interval getRange() const;

protected:

  virtual void setRange(const Interval & range);

public:
  /** Get the roughness, i.e. the L2-norm of the PDF */
  virtual Scalar getRoughness() const;

  /** Get the mean of the distribution */
  virtual Point getMean() const;

  /** Get the standard deviation of the distribution */
  virtual Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  virtual Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  virtual Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  virtual Point getStandardMoment(const UnsignedInteger n) const;

  /** Get the raw moments of the distribution */
  virtual Point getMoment(const UnsignedInteger n) const;

  /** Get the centered moments of the distribution */
  virtual Point getCenteredMoment(const UnsignedInteger n) const;

  /** Get the shifted moments of the distribution */
  virtual Point getShiftedMoment(const UnsignedInteger n,
                                 const Point & shift) const;

  /** Get the covariance of the distribution */
  virtual CovarianceMatrix getCovariance() const;

  /** Correlation matrix accessor */
  CorrelationMatrix getCorrelation() const;

  /** Get the linear correlation of the distribution */
  virtual CorrelationMatrix getLinearCorrelation() const;
  virtual CorrelationMatrix getPearsonCorrelation() const;

  /** Get the Spearman correlation of the distribution */
  virtual CorrelationMatrix getSpearmanCorrelation() const;

  /** Get the Kendall concordance of the distribution */
  virtual CorrelationMatrix getKendallTau() const;

  /** Get the shape matrix of the distribution, ie the correlation matrix
      of its copula if it is elliptical */
  virtual CorrelationMatrix getShapeMatrix() const;

  /** Cholesky factor of the covariance matrix accessor */
  TriangularMatrix getCholesky() const;

  /** Inverse of the Cholesky factor of the covariance matrix accessor */
  TriangularMatrix getInverseCholesky() const;

  /** Check if the distribution is a copula */
  virtual Bool isCopula() const;

  /** Check if the distribution is elliptical */
  virtual Bool isElliptical() const;

  /** Check if the distribution is continuous */
  virtual Bool isContinuous() const;

  /** Check if the distribution is discrete */
  virtual Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  virtual Bool isIntegral() const;

  /** Tell if the distribution has elliptical copula */
  virtual Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  virtual Bool hasIndependentCopula() const;

  /** Get the support of a distribution that intersect a given interval */
  virtual Sample getSupport(const Interval & interval) const;

  /** Get the support on the whole range */
  virtual Sample getSupport() const;

  /** Get the discrete probability levels */
  virtual Point getProbabilities() const;

  /** Get the PDF singularities inside of the range - 1D only */
  virtual Point getSingularities() const;

  /** Compute the density generator of the elliptical generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R(x-mu))                      */
  virtual Scalar computeDensityGenerator(const Scalar betaSquare) const;

  /** Compute the derivative of the density generator */
  virtual Scalar computeDensityGeneratorDerivative(const Scalar betaSquare) const;

  /** Compute the seconde derivative of the density generator */
  virtual Scalar computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const;

  /** Compute the radial distribution CDF */
  virtual Scalar computeRadialDistributionCDF(const Scalar radius,
      const Bool tail = false) const;

  /** Get the i-th marginal distribution */
  virtual Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  virtual Distribution getMarginal(const Indices & indices) const;

  /** Get the copula of a distribution */
  virtual Distribution getCopula() const;

  /** Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual Scalar computeConditionalDDF(const Scalar x,
                                       const Point & y) const;

  virtual Point computeSequentialConditionalDDF(const Point & x) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual Scalar computeConditionalPDF(const Scalar x,
                                       const Point & y) const;

  virtual Point computeSequentialConditionalPDF(const Point & x) const;

  virtual Point computeConditionalPDF(const Point & x,
                                      const Sample & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual Scalar computeConditionalCDF(const Scalar x,
                                       const Point & y) const;

  virtual Point computeSequentialConditionalCDF(const Point & x) const;

  virtual Point computeConditionalCDF(const Point & x,
                                      const Sample & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  virtual Scalar computeConditionalQuantile(const Scalar q,
      const Point & y) const;

  virtual Point computeSequentialConditionalQuantile(const Point & q) const;

  virtual Point computeConditionalQuantile(const Point & q,
      const Sample & y) const;

  /** Get the isoprobabilist transformation */
  virtual IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  virtual InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution */
  virtual Distribution getStandardDistribution() const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  virtual Distribution getStandardRepresentative() const;

  /** integrationNodesNumber accessors */
  UnsignedInteger getIntegrationNodesNumber() const;
  void setIntegrationNodesNumber(const UnsignedInteger integrationNodesNumber) const;

protected:
  /** Gauss nodes and weights accessor */
  Point getGaussNodesAndWeights(Point & weights) const;

public:
  /** Draw the PDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawPDF(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                        const Bool logScale = false) const;

  /** Draw the PDF of the distribution when its dimension is 1 */
  virtual Graph drawPDF(const Scalar xMin,
                        const Scalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                        const Bool logScale = false) const;

  /** Draw the PDF of a 1D marginal */
  virtual Graph drawMarginal1DPDF(const UnsignedInteger marginalIndex,
                                  const Scalar xMin,
                                  const Scalar xMax,
                                  const UnsignedInteger pointNumber,
                                  const Bool logScale = false) const;

  /** Draw the PDF of the distribution when its dimension is 2 */
  virtual Graph drawPDF(const Point & xMin,
                        const Point & xMax,
                        const Indices & pointNumber,
                        const Bool logScaleX = false,
                        const Bool logScaleY = false) const;
  virtual Graph drawPDF(const Point & xMin,
                        const Point & xMax,
                        const Bool logScaleX = false,
                        const Bool logScaleY = false) const;
  virtual Graph drawPDF(const Indices & pointNumber,
                        const Bool logScaleX = false,
                        const Bool logScaleY = false) const;

  /** Draw the PDF of a 2D marginal */
  virtual Graph drawMarginal2DPDF(const UnsignedInteger firstMarginal,
                                  const UnsignedInteger secondMarginal,
                                  const Point & xMin,
                                  const Point & xMax,
                                  const Indices & pointNumber,
                                  const Bool logScaleX = false,
                                  const Bool logScaleY = false) const;

  /** Draw the log-PDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawLogPDF(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                           const Bool logScale = false) const;

  /** Draw the log-PDF of the distribution when its dimension is 1 */
  virtual Graph drawLogPDF(const Scalar xMin,
                           const Scalar xMax,
                           const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                           const Bool logScale = false) const;

  /** Draw the log-PDF of a 1D marginal */
  virtual Graph drawMarginal1DLogPDF(const UnsignedInteger marginalIndex,
                                     const Scalar xMin,
                                     const Scalar xMax,
                                     const UnsignedInteger pointNumber,
                                     const Bool logScale = false) const;

  /** Draw the log-PDF of the distribution when its dimension is 2 */
  virtual Graph drawLogPDF(const Point & xMin,
                           const Point & xMax,
                           const Indices & pointNumber,
                           const Bool logScaleX = false,
                           const Bool logScaleY = false) const;
  virtual Graph drawLogPDF(const Point & xMin,
                           const Point & xMax,
                           const Bool logScaleX = false,
                           const Bool logScaleY = false) const;
  virtual Graph drawLogPDF(const Indices & pointNumber,
                           const Bool logScaleX = false,
                           const Bool logScaleY = false) const;

  /** Draw the PDF of a 2D marginal */
  virtual Graph drawMarginal2DLogPDF(const UnsignedInteger firstMarginal,
                                     const UnsignedInteger secondMarginal,
                                     const Point & xMin,
                                     const Point & xMax,
                                     const Indices & pointNumber,
                                     const Bool logScaleX = false,
                                     const Bool logScaleY = false) const;

  /** Draw the CDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawCDF(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                        const Bool logScale = false) const;

  /** Draw the CDF of the distribution when its dimension is 1 */
  virtual Graph drawCDF(const Scalar xMin,
                        const Scalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                        const Bool logScale = false) const;

  /** Draw the CDF of the distribution when its dimension is 2 */
  virtual Graph drawCDF(const Point & xMin,
                        const Point & xMax,
                        const Indices & pointNumber,
                        const Bool logScaleX = false,
                        const Bool logScaleY = false) const;
  virtual Graph drawCDF(const Point & xMin,
                        const Point & xMax,
                        const Bool logScaleX = false,
                        const Bool logScaleY = false) const;
  virtual Graph drawCDF(const Indices & pointNumber,
                        const Bool logScaleX = false,
                        const Bool logScaleY = false) const;

  /** Draw the CDF of a 1D marginal */
  virtual Graph drawMarginal1DCDF(const UnsignedInteger marginalIndex,
                                  const Scalar xMin,
                                  const Scalar xMax,
                                  const UnsignedInteger pointNumber,
                                  const Bool logScale = false) const;

  /** Draw the CDF of a 2D marginal */
  virtual Graph drawMarginal2DCDF(const UnsignedInteger firstMarginal,
                                  const UnsignedInteger secondMarginal,
                                  const Point & xMin,
                                  const Point & xMax,
                                  const Indices & pointNumber,
                                  const Bool logScaleX = false,
                                  const Bool logScaleY = false) const;

  /** Draw the SurvivalFunction of the distribution when its dimension is 1 or 2 */
  virtual Graph drawSurvivalFunction(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                                     const Bool logScale = false) const;

  /** Draw the SurvivalFunction of the distribution when its dimension is 1 */
  virtual Graph drawSurvivalFunction(const Scalar xMin,
                                     const Scalar xMax,
                                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                                     const Bool logScale = false) const;

  /** Draw the SurvivalFunction of the distribution when its dimension is 2 */
  virtual Graph drawSurvivalFunction(const Point & xMin,
                                     const Point & xMax,
                                     const Indices & pointNumber,
                                     const Bool logScaleX = false,
                                     const Bool logScaleY = false) const;
  virtual Graph drawSurvivalFunction(const Point & xMin,
                                     const Point & xMax,
                                     const Bool logScaleX = false,
                                     const Bool logScaleY = false) const;
  virtual Graph drawSurvivalFunction(const Indices & pointNumber,
                                     const Bool logScaleX = false,
                                     const Bool logScaleY = false) const;

  /** Draw the SurvivalFunction of a 1D marginal */
  virtual Graph drawMarginal1DSurvivalFunction(const UnsignedInteger marginalIndex,
      const Scalar xMin,
      const Scalar xMax,
      const UnsignedInteger pointNumber,
      const Bool logScale = false) const;

  /** Draw the SurvivalFunction of a 2D marginal */
  virtual Graph drawMarginal2DSurvivalFunction(const UnsignedInteger firstMarginal,
      const UnsignedInteger secondMarginal,
      const Point & xMin,
      const Point & xMax,
      const Indices & pointNumber,
      const Bool logScaleX = false,
      const Bool logScaleY = false) const;

  /** Draw the quantile of the distribution when its dimension is 1 or 2 */
  virtual Graph drawQuantile(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                             const Bool logScale = false) const;

  virtual Graph drawQuantile(const Scalar qMin,
                             const Scalar qMax,
                             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                             const Bool logScale = false) const;

protected:
  virtual Graph drawQuantile1D(const Scalar qMin,
                               const Scalar qMax,
                               const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                               const Bool logScale = false) const;

  virtual Graph drawQuantile2D(const Scalar qMin,
                               const Scalar qMax,
                               const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                               const Bool logScaleX = false,
                               const Bool logScaleY = false) const;
public:

  /** Parameters value and description accessor */
  virtual PointWithDescriptionCollection getParametersCollection() const;
  virtual void setParametersCollection(const PointWithDescriptionCollection & parametersCollection);
  virtual void setParametersCollection(const PointCollection & parametersCollection);

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Total number of parameters */
  virtual UnsignedInteger getParameterDimension() const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Accessor to PDF computation precision */
  Scalar getPDFEpsilon() const;

  /** Accessor to CDF computation precision */
  Scalar getCDFEpsilon() const;

  /** Get a positon indicator for a 1D distribution */
  Scalar getPositionIndicator() const;

  /** Get a dispersion indicator for a 1D distribution */
  Scalar getDispersionIndicator() const;

  /** Is it safe to compute PDF/CDF etc in parallel? */
  Bool isParallel() const;
  void setParallel(const Bool flag);

protected:

  /** Draw the PDF of a discrete distribution */
  virtual Graph drawDiscretePDF(const Scalar xMin,
                                const Scalar xMax,
                                const Bool logScale = false) const;

  /** Draw the log-PDF of a discrete distribution */
  virtual Graph drawDiscreteLogPDF(const Scalar xMin,
                                   const Scalar xMax,
                                   const Bool logScale = false) const;

  /** Draw the CDF of a discrete distribution */
  Graph drawDiscreteCDF(const Scalar xMin,
                        const Scalar xMax,
                        const Bool logScale = false) const;

  /** Draw the SurvivalFunction of a discrete distribution */
  Graph drawDiscreteSurvivalFunction(const Scalar xMin,
                                     const Scalar xMax,
                                     const Bool logScale = false) const;

  /** Compute the characteristic function of 1D distributions in a regular pattern with cache */
  virtual Complex computeCharacteristicFunction(const UnsignedInteger index,
      const Scalar step) const;
  virtual Complex computeLogCharacteristicFunction(const UnsignedInteger index,
      const Scalar step) const;
  virtual Complex computeCharacteristicFunction(const Indices & indices,
      const Point & step) const;
  virtual Complex computeLogCharacteristicFunction(const Indices & indices,
      const Point & step) const;

  /** Compute the mean of the distribution */
  virtual void computeMean() const;

  /** Compute the covariance of the distribution */
  virtual void computeCovariance() const;
  virtual void computeCovarianceContinuous() const;
  virtual void computeCovarianceDiscrete() const;
  virtual void computeCovarianceGeneral() const;

  /** Compute the shifted moments of the distribution */
  virtual Point computeShiftedMomentContinuous(const UnsignedInteger n,
      const Point & shift) const;
  virtual Point computeShiftedMomentDiscrete(const UnsignedInteger n,
      const Point & shift) const;
  virtual Point computeShiftedMomentGeneral(const UnsignedInteger n,
      const Point & shift) const;


  /** Compute the nodes and weights of the 1D gauss integration rule over [-1, 1] */
  virtual void computeGaussNodesAndWeights() const;

  /** Dimension accessor */
  void setDimension(UnsignedInteger dim);

  /** Compute the numerical range of the distribution given the parameters values */
  virtual void computeRange();
  virtual Point computeLowerBound() const;
  virtual Point computeUpperBound() const;

  /** Compute the standard distribution associated with the current distribution */
  virtual void computeStandardDistribution() const;

  /** Interpolate the PDF and CDF for smooth continuous distributions */
  virtual Collection<PiecewiseHermiteEvaluation> interpolatePDFCDF(const UnsignedInteger n);

  mutable Point mean_;
  mutable CovarianceMatrix covariance_;
  mutable Point gaussNodes_;
  mutable Point gaussWeights_;

  /** The integration nodes number for covariance computation */
  mutable UnsignedInteger integrationNodesNumber_;

  /** Flags to avoid redundant computations */
  mutable Bool isAlreadyComputedMean_;
  mutable Bool isAlreadyComputedCovariance_;
  mutable Bool isAlreadyComputedGaussNodesAndWeights_;

  /** Indicators for PDF and CDF absolute precision. By default, equals to DefaultPDFEpsilon, DefaultCDFEpsilon and DefaultQuantileEpsilon */
  Scalar pdfEpsilon_;
  Scalar cdfEpsilon_;
  Scalar quantileEpsilon_;
  UnsignedInteger quantileIterations_;

  /** Standard distribution */
  mutable Bool isAlreadyComputedStandardDistribution_;
  // We cannot have a Distribution member in DistributionImplementation, there would
  // be circular dependency.  Hence this Pointer<DistributionImplementation>
  mutable Implementation p_standardDistribution_;

  /** Optimization for the generating function evaluation */
  mutable Bool isAlreadyCreatedGeneratingFunction_;
  mutable UniVariatePolynomial generatingFunction_;

#ifndef SWIG

  // Class used to wrap the computePDF() method for interpolation purpose
  class PDFWrapper: public EvaluationImplementation
  {
  public:
    PDFWrapper(const DistributionImplementation * p_distribution)
      : EvaluationImplementation()
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    PDFWrapper * clone() const override
    {
      return new PDFWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      return Point(1, p_distribution_->computePDF(point));
    }

    Sample operator() (const Sample & sample) const override
    {
      return p_distribution_->computePDF(sample);
    };

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "pdf");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "PDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "PDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation * p_distribution_;
  };  // class PDFWrapper

  // Class used to wrap the computeLogPDF() method for interpolation purpose
  class LogPDFWrapper: public EvaluationImplementation
  {
  public:
    LogPDFWrapper(const DistributionImplementation * p_distribution)
      : EvaluationImplementation()
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    LogPDFWrapper * clone() const override
    {
      return new LogPDFWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      return Point(1, p_distribution_->computeLogPDF(point));
    }

    Sample operator() (const Sample & sample) const override
    {
      return p_distribution_->computeLogPDF(sample);
    };

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "logpdf");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "LogPDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "LogPDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation * p_distribution_;
  };  // class LogPDFWrapper

  // Structure used to wrap the computeCDF() method for interpolation purpose
  class CDFWrapper: public EvaluationImplementation
  {
  public:
    CDFWrapper(const DistributionImplementation * p_distribution)
      : EvaluationImplementation()
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    CDFWrapper * clone() const override
    {
      return new CDFWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      return computeCDF(point);
    }

    Sample operator() (const Sample & sample) const override
    {
      return computeCDF(sample);
    }

    Point computeCDF(const Point & point) const
    {
      return Point(1, p_distribution_->computeCDF(point));
    };

    Sample computeCDF(const Sample & sample) const
    {
      return p_distribution_->computeCDF(sample);
    };

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "cdf");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "CDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "CDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation * p_distribution_;
  }; // class CDFWrapper

  // Structure used to implement the computeQuantile() method efficiently
  class QuantileWrapper: public EvaluationImplementation
  {
  public:
    QuantileWrapper(const Collection< Pointer<DistributionImplementation> > marginals,
                    const DistributionImplementation * p_distribution)
      : EvaluationImplementation()
      , marginals_(marginals)
      , p_distribution_(p_distribution)
      , dimension_(p_distribution->getDimension())
    {
      // Nothing to do
    }

    QuantileWrapper * clone() const override
    {
      return new QuantileWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      return p_distribution_->computeQuantile(point[0]);
    }

    Sample operator() (const Sample & sample) const override
    {
      return p_distribution_->computeQuantile(sample.asPoint());
    }

    Point computeDiagonal(const Point & u) const
    {
      const Scalar cdf = p_distribution_->computeCDF(diagonalToSpace(u[0]));
      LOGDEBUG(OSS(false) << "in DistributionImplementation::QuantileWrapper::computeDiagonal, u=" << u << ", cdf=" << cdf);
      return Point(1, cdf);
    }

    Point diagonalToSpace(const Scalar tau) const
    {
      Point x(dimension_);
      for (UnsignedInteger i = 0; i < dimension_; ++i) x[i] = marginals_[i]->computeQuantile(tau)[0];
      LOGDEBUG(OSS(false) << "in DistributionImplementation::QuantileWrapper::diagonalToSpace, tau=" << tau << ", x=" << x);
      return x;
    }

    UnsignedInteger getInputDimension() const override
    {
      return 1;
    }

    UnsignedInteger getOutputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    Description getInputDescription() const override
    {
      return Description(1, "quantile");
    }

    Description getOutputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "QuantileWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "QuantileWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const Collection< Pointer<DistributionImplementation> > marginals_;
    const DistributionImplementation * p_distribution_;
    const UnsignedInteger dimension_;
  }; // struct QuantileWrapper

  // Structure used to implement the computeInverseSurvivalFunction() method efficiently
  class SurvivalFunctionWrapper: public EvaluationImplementation
  {
  public:
    SurvivalFunctionWrapper(const Collection< Pointer<DistributionImplementation> > marginals,
                            const DistributionImplementation * p_distribution)
      : EvaluationImplementation()
      , marginals_(marginals)
      , p_distribution_(p_distribution)
      , dimension_(p_distribution->getDimension())
    {
      // Nothing to do
    }

    SurvivalFunctionWrapper * clone() const override
    {
      return new SurvivalFunctionWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      return Point(1, p_distribution_->computeSurvivalFunction(point));
    }

    Sample operator() (const Sample & sample) const override
    {
      return p_distribution_->computeSurvivalFunction(sample);
    }

    Point computeDiagonal(const Point & u) const
    {
      const Scalar survival = p_distribution_->computeSurvivalFunction(diagonalToSpace(u[0]));
      LOGDEBUG(OSS(false) << "in DistributionImplementation::InverseSurvivalFunctionWrapper::computeDiagonal, u=" << u << ", survival=" << survival);
      return Point(1, survival);
    }

    Point diagonalToSpace(const Scalar tau) const
    {
      Point x(dimension_);
      for (UnsignedInteger i = 0; i < dimension_; ++i) x[i] = marginals_[i]->computeQuantile(tau, true)[0];
      LOGDEBUG(OSS(false) << "in DistributionImplementation::InverseSurvivalFunctionWrapper::diagonalToSpace, tau=" << tau << ", x=" << x);
      return x;
    }

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "survival function");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "SurvivalFunctionWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "SurvivalFunctionWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const Collection< Pointer<DistributionImplementation> > marginals_;
    const DistributionImplementation * p_distribution_;
    const UnsignedInteger dimension_;
  }; // struct SurvivalFunctionWrapper

  class MinimumVolumeLevelSetEvaluation: public EvaluationImplementation
  {
  public:
    // Here we use a smart pointer instead of a const C++ pointer because the life-cycle of the
    // object goes outside of the calling method
    MinimumVolumeLevelSetEvaluation(const DistributionImplementation::Implementation & p_distribution)
      : EvaluationImplementation()
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    MinimumVolumeLevelSetEvaluation * clone() const override
    {
      return new MinimumVolumeLevelSetEvaluation(*this);
    }

    // The minimum volume level A(p) set is such that A(p)={x\in R^n | y(x) <= y_p}
    // where y(x)=-\log X and y_p is the p-quantile of Y=pdf(X)
    Point operator() (const Point & point) const override
    {
      const Scalar value = -p_distribution_->computeLogPDF(point);
      return Point(1, value);
    }

    Sample operator() (const Sample & sample) const override
    {
      return p_distribution_->computeLogPDF(sample) * (-1.0);
    }

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "-logPDF");
    }

    Description getDescription() const override
    {
      Description description(getInputDescription());
      description.add(getOutputDescription());
      return description;
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "MinimumVolumeLevelSetEvaluation(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "MinimumVolumeLevelSetEvaluation(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation::Implementation p_distribution_;
  }; // class MinimumVolumeLevelSetEvaluation

  class MinimumVolumeLevelSetGradient: public GradientImplementation
  {
  public:
    // Here we use a smart pointer instead of a const C++ pointer because the life-cycle of the
    // object goes outside of the calling method
    MinimumVolumeLevelSetGradient(const DistributionImplementation::Implementation & p_distribution)
      : GradientImplementation()
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    MinimumVolumeLevelSetGradient * clone() const override
    {
      return new MinimumVolumeLevelSetGradient(*this);
    }

    Matrix gradient(const Point & point) const override
    {
      const Scalar pdf = p_distribution_->computePDF(point);
      if (pdf == 0) return Matrix(getInputDimension(), getOutputDimension());
      const Point value = p_distribution_->computeDDF(point) * (-1.0 / pdf);
      return MatrixImplementation(getInputDimension(), getOutputDimension(), value);
    }

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const
    {
      return Description(1, "-logPDF");
    }

    Description getDescription() const
    {
      Description description(getInputDescription());
      description.add(getOutputDescription());
      return description;
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "MinimumVolumeLevelSetGradient(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "MinimumVolumeLevelSetGradient(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation::Implementation p_distribution_;
  }; // class MinimumVolumeLevelSetGradient

  class CovarianceWrapper: public EvaluationImplementation
  {
  public:
    CovarianceWrapper(const DistributionImplementation::Implementation & p_distribution,
                      const Scalar muI,
                      const Scalar muJ)
      : EvaluationImplementation()
      , p_distribution_(p_distribution)
      , muI_(muI)
      , muJ_(muJ)
    {
      // Nothing to do
    }

    CovarianceWrapper * clone() const override
    {
      return new CovarianceWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      return Point(1, (point[0] - muI_) * (point[1] - muJ_) * p_distribution_->computePDF(point));
    }

    Sample operator() (const Sample & sample) const override
    {
      const UnsignedInteger size = sample.getSize();
      Sample result(p_distribution_->computePDF(sample));
      for (UnsignedInteger i = 0; i < size; ++i) result(i, 0) *= (sample(i, 0) - muI_) * (sample(i, 1) - muJ_);
      return result;
    }

    UnsignedInteger getInputDimension() const override
    {
      return 2;
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return Description::BuildDefault(2, "x");
    }

    Description getOutputDescription() const override
    {
      return Description(1, "c");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "CovarianceWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "CovarianceWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation::Implementation p_distribution_;
    const Scalar muI_;
    const Scalar muJ_;
  };  // class CovarianceWrapper

  // Class used to wrap the computeConditionalPDF() method for the computation of the conditional CDF
  class ShiftedMomentWrapper: public EvaluationImplementation
  {
  public:
    ShiftedMomentWrapper(const UnsignedInteger n,
                         const Scalar shift,
                         const DistributionImplementation::Implementation & p_distribution)
      : EvaluationImplementation()
      , n_(1.0 * n)
      , shift_(shift)
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    };

    ShiftedMomentWrapper * clone() const override
    {
      return new ShiftedMomentWrapper(*this);
    }

    Point operator() (const Point & point) const override
    {
      const Scalar power = std::pow(point[0] - shift_, n_);
      const Scalar pdf = p_distribution_->computePDF(point);
      return Point(1, power * pdf);
    };

    Sample operator() (const Sample & sample) const override
    {
      const UnsignedInteger size = sample.getSize();
      Sample result(size, 1);
      const Sample pdf(p_distribution_->computePDF(sample));
      for (UnsignedInteger i = 0; i < size; ++i)
        result(i, 0) = std::pow(sample(i, 0) - shift_, n_) * pdf(i, 0);
      return result;
    };

    UnsignedInteger getInputDimension() const override
    {
      return 1;
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "ShiftedMomentWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "ShiftedMomentWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const Scalar n_;
    const Scalar shift_;
    const DistributionImplementation::Implementation & p_distribution_;
  }; // class ShiftedMomentWrapper

  // Class used to wrap the computeConditionalPDF() method for the computation of the conditional CDF
  class ConditionalPDFWrapper: public UniVariateFunctionImplementation
  {
  public:
    ConditionalPDFWrapper(const DistributionImplementation::Implementation p_distribution)
      : UniVariateFunctionImplementation()
      , y_(1, 0.0)
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    };

    Scalar operator() (const Scalar x) const override
    {
      Collection<Scalar> z(y_);
      z.add(x);
      return p_distribution_->computePDF(z);
    };

    void setParameter(const Point & parameters)
    {
      y_ = parameters;
    }

    Point getParameter() const
    {
      return y_;
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "ConditionalPDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "ConditionalPDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    Collection<Scalar> y_;
    const DistributionImplementation::Implementation p_distribution_;
  }; // class ConditionalPDFWrapper

  // Class used to wrap the computeConditionalCDF() method for the computation of the conditional quantile
  class ConditionalCDFWrapper: public UniVariateFunctionImplementation
  {
  public:
    ConditionalCDFWrapper(const DistributionImplementation * p_distribution)
      : UniVariateFunctionImplementation()
      , y_(1, 0.0)
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    };

    ConditionalCDFWrapper * clone() const override
    {
      return new ConditionalCDFWrapper(*this);
    }

    Scalar operator() (const Scalar x) const override
    {
      return p_distribution_->computeConditionalCDF(x, y_);
    };

    void setParameter(const Point & parameters)
    {
      y_ = parameters;
    }

    Point getParameter() const
    {
      return y_;
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "ConditionalCDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "ConditionalCDFWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    Point y_;
    const DistributionImplementation * p_distribution_;
  }; // class ConditionalCDFWrapper

  // Class used to compute entropy
  class EntropyKernel: public EvaluationImplementation
  {
  public:
    EntropyKernel(const DistributionImplementation * p_distribution)
      : EvaluationImplementation()
      , p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    EntropyKernel * clone() const override
    {
      return new EntropyKernel(*this);
    }

    Point operator() (const Point & point) const override
    {
      const Scalar logPDF = p_distribution_->computeLogPDF(point);
      return Point(1, -std::exp(logPDF) * logPDF);
    }

    Sample operator() (const Sample & sample) const override
    {
      const UnsignedInteger size = sample.getSize();
      const Point logPDF(p_distribution_->computeLogPDF(sample).asPoint());
      Sample result(size, 1);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const Scalar logPDFI = logPDF[i];
        result(i, 0) = -std::exp(logPDFI) * logPDFI;
      }
      return result;
    };

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "entropyKernel");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "EntropyKernel(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String & ) const override
    {
      OSS oss;
      oss << "EntropyKernel(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation * p_distribution_;
  };  // class EntropyKernel

  // Class used to wrap the computePDF()**2 method for integration purpose
  class PDFSquaredWrapper : public EvaluationImplementation
  {
  public:
    PDFSquaredWrapper(const DistributionImplementation *p_distribution)
      : EvaluationImplementation(), p_distribution_(p_distribution)
    {
      // Nothing to do
    }

    PDFSquaredWrapper *clone() const override
    {
      return new PDFSquaredWrapper(*this);
    }

    Point operator()(const Point &point) const override
    {
      const Scalar pdf = p_distribution_->computePDF(point);
      return Point(1, pdf * pdf);
    }

    Sample operator()(const Sample &sample) const override
    {
      // Keep the parallelism of computePDF
      Sample pdfSquared(p_distribution_->computePDF(sample));
      for (UnsignedInteger i = 0; i < pdfSquared.getSize(); ++i)
        pdfSquared(i, 0) = pdfSquared(i, 0) * pdfSquared(i, 0);
      return pdfSquared;
    }

    UnsignedInteger getInputDimension() const override
    {
      return p_distribution_->getDimension();
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return p_distribution_->getDescription();
    }

    Description getOutputDescription() const override
    {
      return Description(1, "pdfSquared");
    }

    String __repr__() const override
    {
      OSS oss;
      oss << "PDFSquaredWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

    String __str__(const String &) const override
    {
      OSS oss;
      oss << "PDFSquaredWrapper(" << p_distribution_->__str__() << ")";
      return oss;
    }

  private:
    const DistributionImplementation *p_distribution_;
  }; // class PDFSquaredWrapper

#endif

  /** The dimension of the distribution */
  UnsignedInteger dimension_;

  /** The weight used ONLY by Mixture */
  Scalar weight_;

  /** Range of the distribution */
  Interval range_;

  /** Description of each component */
  Description description_;

  /** Use parallelism */
  Bool isParallel_;

  /** Flag to tell if the distribution is indeed a copula */
  Bool isCopula_;

}; /* class DistributionImplementation */

#ifndef SWIG

/** Product operator */
OT_API Distribution operator * (const Scalar,
                                const DistributionImplementation & distribution);

OT_API Distribution operator * (const Scalar,
                                const DistributionImplementation::Implementation & p_distribution);

/** Division operator */
OT_API Distribution operator / (const Scalar,
                                const DistributionImplementation & distribution);

OT_API Distribution operator / (const Scalar,
                                const DistributionImplementation::Implementation & p_distribution);

/** Addition operator */
OT_API Distribution operator + (const Scalar,
                                const DistributionImplementation & distribution);

OT_API Distribution operator + (const Scalar,
                                const DistributionImplementation::Implementation & p_distribution);

/** Subtraction operator */
OT_API Distribution operator - (const Scalar,
                                const DistributionImplementation & distribution);

OT_API Distribution operator - (const Scalar,
                                const DistributionImplementation::Implementation & p_distribution);

OT_API Distribution operator - (const DistributionImplementation & distribution);

OT_API Distribution operator - (const DistributionImplementation::Implementation & p_distribution);


#endif

OT_API Distribution maximum(const DistributionImplementation::Implementation & p_left,
                            const DistributionImplementation::Implementation & p_right);

OT_API Distribution maximum(const DistributionImplementation & left,
                            const DistributionImplementation::Implementation & p_right);

OT_API Distribution maximum(const DistributionImplementation::Implementation & p_left,
                            const DistributionImplementation & right);

OT_API Distribution maximum(const DistributionImplementation & left,
                            const DistributionImplementation & right);


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONIMPLEMENTATION_HXX */
