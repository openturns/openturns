//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;
public:

  typedef Function                     IsoProbabilisticTransformation;
  typedef IsoProbabilisticTransformation            InverseIsoProbabilisticTransformation;
  typedef Collection<NumericalPoint>                NumericalPointCollection;
  typedef Collection<NumericalPointWithDescription> NumericalPointWithDescriptionCollection;

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

  Distribution operator + (const NumericalScalar value) const;

  /** Substraction operator */
  Distribution operator - (const Distribution & other) const;

  Distribution operator - (const NumericalScalar value) const;

  /** Multiplication operator */
  Distribution operator * (const Distribution & other) const;

  Distribution operator * (const NumericalScalar value) const;

  /** Division operator */
  Distribution operator / (const Distribution & other) const;

  Distribution operator / (const NumericalScalar value) const;

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
  Distribution pow(const NumericalScalar exponent) const;
  Distribution inverse() const;
  Distribution sqr() const;
  Distribution sqrt() const;
  Distribution cbrt() const;
  Distribution abs() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Weight accessor */
  void setWeight(const NumericalScalar w);
  NumericalScalar getWeight() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /* Here is the interface that all derived class must implement */

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get a numerical sample whose elements follow the distribution */
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the distribution */
  NumericalScalar computeDDF(const NumericalScalar scalar) const;
  NumericalPoint  computeDDF(const NumericalPoint & point) const;
  NumericalSample computeDDF(const NumericalSample & sample) const;

  /** Get the PDF of the distribution */
  NumericalScalar computePDF(const NumericalScalar scalar) const;
  NumericalScalar computePDF(const NumericalPoint & point) const;
  NumericalSample computePDF(const NumericalSample & sample) const;

  NumericalScalar computeLogPDF(const NumericalScalar scalar) const;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;
  NumericalSample computeLogPDF(const NumericalSample & sample) const;

  /** Compute the PDF of 1D distributions over a regular grid */
  NumericalSample computePDF(const NumericalScalar xMin,
                             const NumericalScalar xMax,
                             const UnsignedInteger pointNumber) const;

  /** Compute the log-PDF of 1D distributions over a regular grid */
  NumericalSample computeLogPDF(const NumericalScalar xMin,
				const NumericalScalar xMax,
				const UnsignedInteger pointNumber) const;

  /** Get the CDF of the distribution */
  NumericalScalar computeCDF(const NumericalScalar scalar) const;
  NumericalScalar computeComplementaryCDF(const NumericalScalar scalar) const;
  NumericalScalar computeSurvivalFunction(const NumericalScalar scalar) const;

  NumericalScalar computeCDF(const NumericalPoint & point) const;
  NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;
  NumericalPoint computeInverseSurvivalFunction(const NumericalScalar prob) const;
#ifndef SWIG
  NumericalPoint computeInverseSurvivalFunction(const NumericalScalar prob,
						NumericalScalar & marginalProb) const;
#endif

  NumericalSample computeCDF(const NumericalSample & sample) const;
  NumericalSample computeComplementaryCDF(const NumericalSample & sample) const;
  NumericalSample computeSurvivalFunction(const NumericalSample & sample) const;

  /** Compute the CDF of 1D distributions over a regular grid */
  NumericalSample computeCDF(const NumericalScalar xMin,
                             const NumericalScalar xMax,
                             const UnsignedInteger pointNumber) const;

  NumericalSample computeComplementaryCDF(const NumericalScalar xMin,
                                          const NumericalScalar xMax,
                                          const UnsignedInteger pointNumber) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;
  NumericalComplex computeLogCharacteristicFunction(const NumericalScalar x) const;

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  NumericalScalar computeGeneratingFunction(const NumericalScalar z) const;
  NumericalScalar computeLogGeneratingFunction(const NumericalScalar z) const;

  NumericalComplex computeGeneratingFunction(const NumericalComplex & z) const;
  NumericalComplex computeLogGeneratingFunction(const NumericalComplex & z) const;

  /** Get the PDF gradient of the distribution */
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;
  NumericalSample computePDFGradient(const NumericalSample & sample) const;

  /** Get the log(PDFgradient) of the distribution */
  NumericalPoint computeLogPDFGradient(const NumericalPoint & point) const;
  NumericalSample computeLogPDFGradient(const NumericalSample & sample) const;

  /** Get the CDF gradient of the distribution */
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;
  NumericalSample computeCDFGradient(const NumericalSample & sample) const;

  /** Get the quantile of the distribution */
  NumericalPoint computeQuantile(const NumericalScalar prob,
                                 const Bool tail = false) const;
#ifndef SWIG
  NumericalPoint computeQuantile(const NumericalScalar prob,
                                 const Bool tail,
				 NumericalScalar & marginalProb) const;
#endif
  NumericalSample computeQuantile(const NumericalPoint & prob,
                                  const Bool tail = false) const;
  /** Compute the quantile over a regular grid */
  NumericalSample computeQuantile(const NumericalScalar qMin,
                                  const NumericalScalar qMax,
                                  const UnsignedInteger pointNumber,
                                  const Bool tail = false) const;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeInterval(const NumericalScalar prob) const;
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const;

  /** Get the product bilateral confidence interval containing a given probability of the distribution */
  Interval computeBilateralConfidenceInterval(const NumericalScalar prob) const;
  Interval computeBilateralConfidenceIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const;

  /** Get the product unilateral confidence interval containing a given probability of the distribution */
  Interval computeUnilateralConfidenceInterval(const NumericalScalar prob, const Bool tail = false) const;
  Interval computeUnilateralConfidenceIntervalWithMarginalProbability(const NumericalScalar prob, const Bool tail, NumericalScalar & marginalProb) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSet(const NumericalScalar prob) const;
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const NumericalScalar prob, NumericalScalar & threshold) const;

  /** Get the mathematical and numerical range of the distribution.
      Its mathematical range is the smallest closed interval outside
      of which the PDF is zero, and the numerical range is the interval
      outside of which the PDF is rounded to zero in double precision */
  Interval getRange() const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  NumericalScalar getRoughness() const;

  /** Get the mean of the distribution */
  NumericalPoint getMean() const;

  /** Get the covariance of the distribution */
  CovarianceMatrix getCovariance() const;

  /** Cholesky factor of the covariance matrix accessor */
  TriangularMatrix getCholesky() const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the raw moments of the distribution */
  NumericalPoint getMoment(const UnsignedInteger n) const;

  /** Get the centered moments about the mean of the distribution */
  NumericalPoint getCenteredMoment(const UnsignedInteger n) const;

  /** Get the shifted moments of the distribution */
  NumericalPoint getShiftedMoment(const UnsignedInteger n,
                                  const NumericalPoint & shift) const;

  /** Inverse of the Cholesky factor of the covariance matrix accessor */
  TriangularMatrix getInverseCholesky() const;

  /** Correlation matrix accessor */
  CorrelationMatrix getCorrelation() const;
  CorrelationMatrix getLinearCorrelation() const;
  CorrelationMatrix getPearsonCorrelation() const;

  /** Get the Spearman correlation of the distribution */
  virtual CorrelationMatrix getSpearmanCorrelation() const;

  /** Get the Kendall concordance of the distribution */
  virtual CorrelationMatrix getKendallTau() const;

  /** Get the Shape matrix of the copula */
  CorrelationMatrix getShapeMatrix() const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Get the implementation of the distribution */
  Implementation getImplementation() const;

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
  NumericalSample getSupport(const Interval & interval) const;

  /** Get the support on the whole range */
  NumericalSample getSupport() const;

  /** Get the discrete probability levels */
  NumericalPoint getProbabilities() const;

  /** Get the PDF singularities inside of the range - 1D only */
  NumericalPoint getSingularities() const;

  /** Compute the density generator of the elliptical generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R(x-mu))                      */
  virtual NumericalScalar computeDensityGenerator(const NumericalScalar betaSquare) const;

  /** Compute the derivative of the density generator */
  virtual NumericalScalar computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const;

  /** Compute the seconde derivative of the density generator */
  virtual NumericalScalar computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const;

  /** Compute the radial distribution CDF */
  virtual NumericalScalar computeRadialDistributionCDF (const NumericalScalar radius,
      const Bool tail = false) const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const;

  /** Get the copula of a distribution */
  Distribution getCopula() const;

  /** Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalDDF(const NumericalScalar x,
      const NumericalPoint & y) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalPDF(const NumericalScalar x,
      const NumericalPoint & y) const;
  virtual NumericalPoint computeConditionalPDF(const NumericalPoint & x,
      const NumericalSample & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalCDF(const NumericalScalar x,
      const NumericalPoint & y) const;
  virtual NumericalPoint computeConditionalCDF(const NumericalPoint & x,
      const NumericalSample & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalQuantile(const NumericalScalar q,
      const NumericalPoint & y) const;
  virtual NumericalPoint computeConditionalQuantile(const NumericalPoint & q,
      const NumericalSample & y) const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution */
  Distribution getStandardDistribution() const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  /** Draw the PDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawPDF() const;

  /** Draw the PDF of the distribution when its dimension is 1 */
  virtual Graph drawPDF(const NumericalScalar xMin,
                        const NumericalScalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")) const;
  virtual Graph drawPDF(const UnsignedInteger pointNumber) const;

  /** Draw the PDF of a 1D marginal */
  virtual Graph drawMarginal1DPDF(const UnsignedInteger marginalIndex,
                                  const NumericalScalar xMin,
                                  const NumericalScalar xMax,
                                  const UnsignedInteger pointNumber) const;

  /** Draw the PDF of the distribution when its dimension is 2 */
  virtual Graph drawPDF(const NumericalPoint & xMin,
                        const NumericalPoint & xMax,
                        const Indices & pointNumber) const;
  virtual Graph drawPDF(const NumericalPoint & xMin,
                        const NumericalPoint & xMax) const;
  virtual Graph drawPDF(const Indices & pointNumber) const;

  /** Draw the PDF of a 2D marginal */
  virtual Graph drawMarginal2DPDF(const UnsignedInteger firstMarginal,
                                  const UnsignedInteger secondMarginal,
                                  const NumericalPoint & xMin,
                                  const NumericalPoint & xMax,
                                  const Indices & pointNumber) const;

  /** Draw the log-PDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawLogPDF() const;

  /** Draw the log-PDF of the distribution when its dimension is 1 */
  virtual Graph drawLogPDF(const NumericalScalar xMin,
			   const NumericalScalar xMax,
			   const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")) const;
  virtual Graph drawLogPDF(const UnsignedInteger pointNumber) const;
  
  /** Draw the log-PDF of a 1D marginal */
  virtual Graph drawMarginal1DLogPDF(const UnsignedInteger marginalIndex,
				     const NumericalScalar xMin,
				     const NumericalScalar xMax,
				     const UnsignedInteger pointNumber) const;
  
  /** Draw the log-PDF of the distribution when its dimension is 2 */
  virtual Graph drawLogPDF(const NumericalPoint & xMin,
			   const NumericalPoint & xMax,
			   const Indices & pointNumber) const;
  virtual Graph drawLogPDF(const NumericalPoint & xMin,
			   const NumericalPoint & xMax) const;
  virtual Graph drawLogPDF(const Indices & pointNumber) const;

  /** Draw the PDF of a 2D marginal */
  virtual Graph drawMarginal2DLogPDF(const UnsignedInteger firstMarginal,
				     const UnsignedInteger secondMarginal,
				     const NumericalPoint & xMin,
				     const NumericalPoint & xMax,
				     const Indices & pointNumber) const;

  /** Draw the CDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawCDF() const;

  /** Draw the CDF of the distribution when its dimension is 1 */
  virtual Graph drawCDF(const NumericalScalar xMin,
                        const NumericalScalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")) const;

  virtual Graph drawCDF(const UnsignedInteger pointNumber) const;

  /** Draw the CDF of the distribution when its dimension is 2 */
  virtual Graph drawCDF(const NumericalPoint & xMin,
                        const NumericalPoint & xMax,
                        const Indices & pointNumber) const;
  virtual Graph drawCDF(const NumericalPoint & xMin,
                        const NumericalPoint & xMax) const;
  virtual Graph drawCDF(const Indices & pointNumber) const;

  /** Draw the CDF of a 1D marginal */
  virtual Graph drawMarginal1DCDF(const UnsignedInteger marginalIndex,
                                  const NumericalScalar xMin,
                                  const NumericalScalar xMax,
                                  const UnsignedInteger pointNumber) const;

  /** Draw the CDF of a 2D marginal */
  virtual Graph drawMarginal2DCDF(const UnsignedInteger firstMarginal,
                                  const UnsignedInteger secondMarginal,
                                  const NumericalPoint & xMin,
                                  const NumericalPoint & xMax,
                                  const Indices & pointNumber) const;

  /** Draw the quantile of the distribution when its dimension is 1 or 2 */
  virtual Graph drawQuantile(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")) const;

  virtual Graph drawQuantile(const NumericalScalar qMin,
                             const NumericalScalar qMax,
                             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  void setParametersCollection(const NumericalPointWithDescriptionCollection & parametersCollection);
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Parameters number */
  UnsignedInteger getParameterDimension() const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Accessor to PDF computation precision */
  NumericalScalar getPDFEpsilon() const;

  /** Accessor to CDF computation precision */
  NumericalScalar getCDFEpsilon() const;

  /** Get a positon indicator for a 1D distribution */
  NumericalScalar getPositionIndicator() const;

  /** Get a dispersion indicator for a 1D distribution */
  NumericalScalar getDispersionIndicator() const;

}; /* class Distribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTION_HXX */
