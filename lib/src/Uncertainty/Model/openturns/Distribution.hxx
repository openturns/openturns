//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
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
#ifndef OPENTURNS_DISTRIBUTION_HXX
#define OPENTURNS_DISTRIBUTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Distribution
 *
 * The class describes the probabilistic concept of distribution.
 * Instances of this class can produce samples following the
 * distribution, can compute PDF or CDF, etc.
 * They are the actual key component of RandomVectors.
 */
class OT_API Distribution
  : public TypedInterfaceObject<DistributionImplementation>
{
  CLASSNAME
public:

  typedef Function                     IsoProbabilisticTransformation;
  typedef IsoProbabilisticTransformation            InverseIsoProbabilisticTransformation;
  typedef Collection<Point>                PointCollection;
  typedef Collection<PointWithDescription> PointWithDescriptionCollection;

  /** Default constructor */
  Distribution();

  /** Default constructor */
  Distribution(const DistributionImplementation & implementation);


  /** Constructor from implementation */
  Distribution(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  Distribution(DistributionImplementation * p_implementation);
#endif

  /** Comparison operator */
  Bool operator ==(const Distribution & other) const;

  /** Comparison operator */
  Bool operator !=(const Distribution & other) const;

  /** Addition operator */
  Distribution operator + (const Distribution & other) const;

  Distribution operator + (const Scalar value) const;

  /** Subtraction operator */
  Distribution operator - (const Distribution & other) const;

  Distribution operator - (const Scalar value) const;

  /** Multiplication operator */
  Distribution operator * (const Distribution & other) const;

  Distribution operator * (const Scalar value) const;

  /** Division operator */
  Distribution operator / (const Distribution & other) const;

  Distribution operator / (const Scalar value) const;

  /** Static methods to transform distributions by usual functions */
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
  void setWeight(const Scalar w);
  Scalar getWeight() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /* Here is the interface that all derived class must implement */

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get a numerical sample whose elements follow the distribution */
  Sample getSample(const UnsignedInteger size) const;
  Sample getSampleByInversion(const UnsignedInteger size) const;
  Sample getSampleByQMC(const UnsignedInteger size) const;

  /** Get the DDF of the distribution */
  Scalar computeDDF(const Scalar scalar) const;
  Point  computeDDF(const Point & point) const;
  Sample computeDDF(const Sample & sample) const;

  /** Get the PDF of the distribution */
  Scalar computePDF(const Scalar scalar) const;
  Scalar computePDF(const Point & point) const;
  Sample computePDF(const Sample & sample) const;

  Scalar computeLogPDF(const Scalar scalar) const;
  Scalar computeLogPDF(const Point & point) const;
  Sample computeLogPDF(const Sample & sample) const;

  /** Compute the PDF of 1D distributions over a regular grid */
  Sample computePDF(const Scalar xMin,
                    const Scalar xMax,
                    const UnsignedInteger pointNumber) const;

  /** Compute the log-PDF of 1D distributions over a regular grid */
  Sample computeLogPDF(const Scalar xMin,
                       const Scalar xMax,
                       const UnsignedInteger pointNumber) const;

  /** Get the CDF of the distribution */
  Scalar computeCDF(const Scalar scalar) const;
  Scalar computeComplementaryCDF(const Scalar scalar) const;
  Scalar computeSurvivalFunction(const Scalar scalar) const;

  Scalar computeCDF(const Point & point) const;
  Scalar computeComplementaryCDF(const Point & point) const;
  Scalar computeSurvivalFunction(const Point & point) const;
  Point computeInverseSurvivalFunction(const Scalar prob) const;
#ifndef SWIG
  Point computeInverseSurvivalFunction(const Scalar prob,
                                       Scalar & marginalProb) const;
#endif

  Sample computeCDF(const Sample & sample) const;
  Sample computeComplementaryCDF(const Sample & sample) const;
  Sample computeSurvivalFunction(const Sample & sample) const;

  /** Compute the CDF of 1D distributions over a regular grid */
  Sample computeCDF(const Scalar xMin,
                    const Scalar xMax,
                    const UnsignedInteger pointNumber) const;

  Sample computeComplementaryCDF(const Scalar xMin,
                                 const Scalar xMax,
                                 const UnsignedInteger pointNumber) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Scalar x) const;

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  Scalar computeGeneratingFunction(const Scalar z) const;
  Scalar computeLogGeneratingFunction(const Scalar z) const;

  Complex computeGeneratingFunction(const Complex & z) const;
  Complex computeLogGeneratingFunction(const Complex & z) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the PDF gradient of the distribution */
  Point computePDFGradient(const Point & point) const;
  Sample computePDFGradient(const Sample & sample) const;

  /** Get the log(PDFgradient) of the distribution */
  Point computeLogPDFGradient(const Point & point) const;
  Sample computeLogPDFGradient(const Sample & sample) const;

  /** Get the CDF gradient of the distribution */
  Point computeCDFGradient(const Point & point) const;
  Sample computeCDFGradient(const Sample & sample) const;

  /** Get the quantile of the distribution */
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const;
#ifndef SWIG
  Point computeQuantile(const Scalar prob,
                        const Bool tail,
                        Scalar & marginalProb) const;
#endif
  Sample computeQuantile(const Point & prob,
                         const Bool tail = false) const;
  /** Compute the quantile over a regular grid */
  Sample computeQuantile(const Scalar qMin,
                         const Scalar qMax,
                         const UnsignedInteger pointNumber,
                         const Bool tail = false) const;
  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeInterval(const Scalar prob) const;
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the product bilateral confidence interval containing a given probability of the distribution */
  Interval computeBilateralConfidenceInterval(const Scalar prob) const;
  Interval computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the product unilateral confidence interval containing a given probability of the distribution */
  Interval computeUnilateralConfidenceInterval(const Scalar prob, const Bool tail = false) const;
  Interval computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, const Bool tail, Scalar & marginalProbOut) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSet(const Scalar prob) const;
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const;

  /** Get the mathematical and numerical range of the distribution.
      Its mathematical range is the smallest closed interval outside
      of which the PDF is zero, and the numerical range is the interval
      outside of which the PDF is rounded to zero in double precision */
  Interval getRange() const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  Scalar getRoughness() const;

  /** Get the mean of the distribution */
  Point getMean() const;

  /** Get the covariance of the distribution */
  CovarianceMatrix getCovariance() const;

  /** Cholesky factor of the covariance matrix accessor */
  TriangularMatrix getCholesky() const;

  /** Get the raw moments of the distribution */
  Point getMoment(const UnsignedInteger n) const;

  /** Get the central moments about the mean of the distribution */
  Point getCentralMoment(const UnsignedInteger n) const;

  /** @deprecated */
  Point getCenteredMoment(const UnsignedInteger n) const;

  /** Get the shifted moments of the distribution */
  Point getShiftedMoment(const UnsignedInteger n,
                         const Point & shift) const;

  /** Inverse of the Cholesky factor of the covariance matrix accessor */
  TriangularMatrix getInverseCholesky() const;

  /** Correlation matrix accessor */
  CorrelationMatrix getCorrelation() const;
  CorrelationMatrix getPearsonCorrelation() const;

  /** Get the Spearman correlation of the distribution */
  virtual CorrelationMatrix getSpearmanCorrelation() const;

  /** Get the Kendall concordance of the distribution */
  virtual CorrelationMatrix getKendallTau() const;

  /** Get the Shape matrix of the copula */
  CorrelationMatrix getShapeMatrix() const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Check if the distribution is a copula */
  Bool isCopula() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is constinuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Get the support of a distribution that intersect a given interval */
  Sample getSupport(const Interval & interval) const;

  /** Get the support on the whole range */
  Sample getSupport() const;

  /** Get the discrete probability levels */
  Point getProbabilities() const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const;

  /** Compute the density generator of the elliptical generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R(x-mu))                      */
  virtual Scalar computeDensityGenerator(const Scalar betaSquare) const;

  /** Compute the derivative of the density generator */
  virtual Scalar computeDensityGeneratorDerivative(const Scalar betaSquare) const;

  /** Compute the seconde derivative of the density generator */
  virtual Scalar computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const;

  /** Compute the radial distribution CDF */
  virtual Scalar computeRadialDistributionCDF (const Scalar radius,
      const Bool tail = false) const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const;

  /** Get the copula of a distribution */
  Distribution getCopula() const;

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
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution */
  Distribution getStandardDistribution() const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

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

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const;
  void setParametersCollection(const PointWithDescriptionCollection & parametersCollection);
  void setParametersCollection(const PointCollection & parametersCollection);

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Parameters number */
  UnsignedInteger getParameterDimension() const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Accessor to PDF computation precision */
  Scalar getPDFEpsilon() const;

  /** Accessor to CDF computation precision */
  Scalar getCDFEpsilon() const;

  /** Get a positon indicator for a 1D distribution */
  Scalar getPositionIndicator() const;

  /** Get a dispersion indicator for a 1D distribution */
  Scalar getDispersionIndicator() const;

}; /* class Distribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTION_HXX */
