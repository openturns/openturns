//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
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
#ifndef OPENTURNS_DISTRIBUTIONIMPLEMENTATION_HXX
#define OPENTURNS_DISTRIBUTIONIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"
#include "NumericalPointWithDescription.hxx"
#include "NumericalSample.hxx"
#include "Indices.hxx"
#include "Interval.hxx"
#include "CorrelationMatrix.hxx"
#include "SquareMatrix.hxx"
#include "Graph.hxx"
#include "Description.hxx"
#include "NumericalMathFunction.hxx"
#include "PersistentCollection.hxx"
#include "UniVariatePolynomial.hxx"
#include "PiecewiseHermiteEvaluationImplementation.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

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
  CLASSNAME;
public:

  typedef Pointer<DistributionImplementation>       Implementation;
  typedef NumericalMathFunction                     IsoProbabilisticTransformation;
  typedef IsoProbabilisticTransformation            InverseIsoProbabilisticTransformation;
  typedef Collection<NumericalPoint>                NumericalPointCollection;
  typedef Collection<NumericalPointWithDescription> NumericalPointWithDescriptionCollection;

  /** Default constructor */
  DistributionImplementation();

  /** Comparison operator */
  Bool operator ==(const DistributionImplementation & other) const;
  Bool operator !=(const DistributionImplementation & other) const;

  /** Addition operator */
  Implementation operator + (const DistributionImplementation & other) const;
  Implementation operator + (const Implementation & other) const;

  Implementation operator + (const NumericalScalar value) const;

  /** Substraction operator */
  Implementation operator - (const DistributionImplementation & other) const;
  Implementation operator - (const Implementation & other) const;

  Implementation operator - (const NumericalScalar value) const;

  /** Multiplication operator */
  Implementation operator * (const DistributionImplementation & other) const;
  Implementation operator * (const Implementation & other) const;

  Implementation operator * (const NumericalScalar value) const;

  /** Division operator */
  Implementation operator / (const DistributionImplementation & other) const;
  Implementation operator / (const Implementation & other) const;

  Implementation operator / (const NumericalScalar value) const;

  /** Methods to transform distributions by usual functions */
  Implementation cos() const;
  Implementation sin() const;
  Implementation tan() const;

  Implementation acos() const;
  Implementation asin() const;
  Implementation atan() const;

  Implementation cosh() const;
  Implementation sinh() const;
  Implementation tanh() const;

  Implementation acosh() const;
  Implementation asinh() const;
  Implementation atanh() const;

  Implementation exp() const;
  Implementation log() const;
  Implementation ln() const;

  Implementation pow(const SignedInteger exponent) const;
  Implementation pow(const NumericalScalar exponent) const;
  Implementation inverse() const;
  Implementation sqr() const;
  Implementation sqrt() const;
  Implementation cbrt() const;
  Implementation abs() const;

  /** String converter */
  virtual String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Weight accessor */
  void setWeight(NumericalScalar w);
  NumericalScalar getWeight() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual DistributionImplementation * clone() const;

  /** Get one realization of the distributionImplementation */
  virtual NumericalPoint getRealization() const;

  /** Get a numerical sample whose elements follow the distributionImplementation */
  virtual NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the distributionImplementation */
  virtual NumericalScalar computeDDF(const NumericalScalar scalar) const;
  virtual NumericalPoint  computeDDF(const NumericalPoint & point) const;
  virtual NumericalSample computeDDF(const NumericalSample & sample) const;
protected:
  virtual NumericalSample computeDDFSequential(const NumericalSample & sample) const;
  virtual NumericalSample computeDDFParallel(const NumericalSample & sample) const;
public:

  /** Get the PDF of the distributionImplementation */
  virtual NumericalScalar computePDF(const NumericalScalar scalar) const;
  virtual NumericalScalar computePDF(const NumericalPoint & point) const;
  virtual NumericalSample computePDF(const NumericalSample & sample) const;
protected:
  virtual NumericalSample computePDFSequential(const NumericalSample & sample) const;
  virtual NumericalSample computePDFParallel(const NumericalSample & sample) const;
public:

  virtual NumericalScalar computeLogPDF(const NumericalScalar scalar) const;
  virtual NumericalScalar computeLogPDF(const NumericalPoint & point) const;
  virtual NumericalSample computeLogPDF(const NumericalSample & sample) const;
protected:
  virtual NumericalSample computeLogPDFSequential(const NumericalSample & sample) const;
  virtual NumericalSample computeLogPDFParallel(const NumericalSample & sample) const;
public:

  /** Compute the PDF of 1D distributions over a regular grid */
  virtual NumericalSample computePDF(const NumericalScalar xMin,
                                     const NumericalScalar xMax,
                                     const UnsignedInteger pointNumber,
                                     NumericalSample & grid) const;

  /** Compute the PDF of nD distributions over a regular grid */
  virtual NumericalSample computePDF(const NumericalPoint & xMin,
                                     const NumericalPoint & xMax,
                                     const Indices & pointNumber,
                                     NumericalSample & grid) const;

  /** Get the CDF of the distributionImplementation */
  virtual NumericalScalar computeCDF(const NumericalScalar scalar) const;
  virtual NumericalScalar computeComplementaryCDF(const NumericalScalar scalar) const;
  virtual NumericalScalar computeSurvivalFunction(const NumericalScalar scalar) const;

  virtual NumericalScalar computeCDF(const NumericalPoint & point) const;
  virtual NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;
  virtual NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

protected:
  virtual NumericalSample computeCDFSequential(const NumericalSample & sample) const;
  virtual NumericalSample computeCDFParallel(const NumericalSample & sample) const;
public:
  virtual NumericalSample computeCDF(const NumericalSample & sample) const;
protected:
  virtual NumericalSample computeSurvivalFunctionSequential(const NumericalSample & sample) const;
  virtual NumericalSample computeSurvivalFunctionParallel(const NumericalSample & sample) const;
public:
  virtual NumericalSample computeSurvivalFunction(const NumericalSample & sample) const;
protected:
  virtual NumericalSample computeComplementaryCDFSequential(const NumericalSample & sample) const;
  virtual NumericalSample computeComplementaryCDFParallel(const NumericalSample & sample) const;
public:
  virtual NumericalSample computeComplementaryCDF(const NumericalSample & sample) const;

  /** Compute the CDF of 1D distributions over a regular grid */
  virtual NumericalSample computeCDF(const NumericalScalar xMin,
                                     const NumericalScalar xMax,
                                     const UnsignedInteger pointNumber,
                                     NumericalSample & grid) const;

  /** Compute the CDF of nD distributions over a regular grid */
  virtual NumericalSample computeCDF(const NumericalPoint & xMin,
                                     const NumericalPoint & xMax,
                                     const Indices & pointNumber,
                                     NumericalSample & grid) const;

  virtual NumericalSample computeComplementaryCDF(const NumericalScalar xMin,
      const NumericalScalar xMax,
      const UnsignedInteger pointNumber,
      NumericalSample & grid) const;

  /** Get the probability content of an interval */
  virtual NumericalScalar computeProbability(const Interval & interval) const;
protected:
  /** Generic implementation for continuous distributions */
  virtual NumericalScalar computeProbabilityContinuous(const Interval & interval) const;
  /** Generic implementation for discrete distributions */
  virtual NumericalScalar computeProbabilityDiscrete(const Interval & interval) const;
  /** Generic implementation for general distributions */
  virtual NumericalScalar computeProbabilityGeneral(const Interval & interval) const;
public:

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  virtual NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;
  virtual NumericalComplex computeCharacteristicFunction(const NumericalPoint & x) const;
  virtual NumericalComplex computeLogCharacteristicFunction(const NumericalScalar x) const;
  virtual NumericalComplex computeLogCharacteristicFunction(const NumericalPoint & x) const;

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  virtual NumericalScalar computeGeneratingFunction(const NumericalScalar z) const;
  virtual NumericalScalar computeLogGeneratingFunction(const NumericalScalar z) const;

  virtual NumericalComplex computeGeneratingFunction(const NumericalComplex & z) const;
  virtual NumericalComplex computeLogGeneratingFunction(const NumericalComplex & z) const;

  /** Get the PDF gradient of the distribution */
  virtual NumericalPoint computePDFGradient(const NumericalPoint & point) const;
  virtual NumericalSample computePDFGradient(const NumericalSample & inSample) const;
public:
  /** Get the CDF gradient of the distribution */
  virtual NumericalPoint computeCDFGradient(const NumericalPoint & point) const;
  virtual NumericalSample computeCDFGradient(const NumericalSample & inSample) const;
public:

  /** Get the quantile of the distributionImplementation */
  virtual NumericalPoint computeQuantile(const NumericalScalar prob,
                                         const Bool tail = false) const;

  /** Get the quantile over a provided grid */
protected:
  virtual NumericalSample computeQuantileSequential(const NumericalPoint & prob,
      const Bool tail = false) const;
  virtual NumericalSample computeQuantileParallel(const NumericalPoint & prob,
      const Bool tail = false) const;
public:
  virtual NumericalSample computeQuantile(const NumericalPoint & prob,
                                          const Bool tail = false) const;

  /** Compute the quantile over a regular grid */
  virtual NumericalSample computeQuantile(const NumericalScalar qMin,
                                          const NumericalScalar qMax,
                                          const UnsignedInteger pointNumber,
                                          const Bool tail = false) const;

  virtual NumericalSample computeQuantile(const NumericalScalar qMin,
                                          const NumericalScalar qMax,
                                          const UnsignedInteger pointNumber,
                                          NumericalSample & grid,
                                          const Bool tail = false) const;

  /** Get the minimum volume interval containing a given probability of the distributionImplementation */
  virtual Interval computeMinimumVolumeInterval(const NumericalScalar prob) const;

  /** Get the mathematical and numerical range of the distribution.
      Its mathematical range is the smallest closed interval outside
      of which the PDF is zero, and the numerical range is the interval
      outside of which the PDF is rounded to zero in double precision */
  virtual Interval getRange() const;

protected:

  virtual void setRange(const Interval & range);

public:
  /** Get the roughness, i.e. the L2-norm of the PDF */
  virtual NumericalScalar getRoughness() const;

  /** Get the mean of the distribution */
  virtual NumericalPoint getMean() const;

  /** Get the standard deviation of the distribution */
  virtual NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  virtual NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  virtual NumericalPoint getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  virtual NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the raw moments of the distribution */
  virtual NumericalPoint getMoment(const UnsignedInteger n) const;

  /** Get the centered moments of the distribution */
  virtual NumericalPoint getCenteredMoment(const UnsignedInteger n) const;

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
  virtual NumericalSample getSupport(const Interval & interval) const;

  /** Get the support on the whole range */
  virtual NumericalSample getSupport() const;

  /** Compute the density generator of the elliptical generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R(x-mu))                      */
  virtual NumericalScalar computeDensityGenerator(const NumericalScalar betaSquare) const;

  /** Compute the derivative of the density generator */
  virtual NumericalScalar computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const;

  /** Compute the seconde derivative of the density generator */
  virtual NumericalScalar computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const;

  /** Compute the radial distribution CDF */
  virtual NumericalScalar computeRadialDistributionCDF(const NumericalScalar radius,
      const Bool tail = false) const;

  /** Get the i-th marginal distribution */
  virtual Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  virtual Implementation getMarginal(const Indices & indices) const;

  /** Get the copula of a distribution */
  virtual Implementation getCopula() const;

  /** Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalDDF(const NumericalScalar x,
      const NumericalPoint & y) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalPDF(const NumericalScalar x,
      const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalCDF(const NumericalScalar x,
      const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  virtual NumericalScalar computeConditionalQuantile(const NumericalScalar q,
      const NumericalPoint & y) const;

  /** Get the isoprobabilist transformation */
  virtual IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  virtual InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution */
  virtual Implementation getStandardDistribution() const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  virtual Implementation getStandardRepresentative() const;

  /** integrationNodesNumber accessors */
  UnsignedInteger getIntegrationNodesNumber() const;
  void setIntegrationNodesNumber(const UnsignedInteger integrationNodesNumber) const;

  /** Gauss nodes and weights accessor */
  NumericalPoint getGaussNodesAndWeights(NumericalPoint & weights) const;

  /** Draw the PDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawPDF() const;

  /** Draw the PDF of the distribution when its dimension is 1 */
  virtual Graph drawPDF(const NumericalScalar xMin,
                        const NumericalScalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;

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

  /** Draw the CDF of the distribution when its dimension is 1 or 2 */
  virtual Graph drawCDF() const;

  /** Draw the CDF of the distribution when its dimension is 1 */
  virtual Graph drawCDF(const NumericalScalar xMin,
                        const NumericalScalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;

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
  virtual Graph drawQuantile(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;

  virtual Graph drawQuantile(const NumericalScalar qMin,
                             const NumericalScalar qMax,
                             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;
protected:
  virtual Graph drawQuantile1D(const NumericalScalar qMin,
                               const NumericalScalar qMax,
                               const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;

  virtual Graph drawQuantile2D(const NumericalScalar qMin,
                               const NumericalScalar qMax,
                               const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;
public:

  /** Parameters value and description accessor */
  virtual NumericalPointWithDescriptionCollection getParametersCollection() const;
  virtual void setParametersCollection(const NumericalPointWithDescriptionCollection & parametersCollection);
  virtual void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Total number of parameters */
  virtual UnsignedInteger getParameterDimension() const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Accessor to PDF computation precision */
  NumericalScalar getPDFEpsilon() const;

  /** Accessor to CDF computation precision */
  NumericalScalar getCDFEpsilon() const;

  /** Get a positon indicator for a 1D distribution */
  NumericalScalar getPositionIndicator() const;

  /** Get a dispersion indicator for a 1D distribution */
  NumericalScalar getDispersionIndicator() const;

  /** Is it safe to compute PDF/CDF etc in parallel? */
  Bool isParallel() const;
  void setParallel(const Bool flag);

protected:

  /** Draw the PDF of a discrete distribution */
  virtual Graph drawDiscretePDF(const NumericalScalar xMin,
                                const NumericalScalar xMax,
                                const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("DistributionImplementation-DefaultPointNumber")) const;

  /** Draw the CDF of a discrete distribution */
  Graph drawDiscreteCDF(const NumericalScalar xMin,
                        const NumericalScalar xMax,
                        const UnsignedInteger pointNumber) const;

  /** Compute the PDF and CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
      Used to speed-up the computeConditionalQuantile() method */
  NumericalScalar computeConditionalPDFAndCDF(const NumericalScalar x,
      const NumericalPoint & y,
      NumericalScalar & cdf,
      const Implementation & conditioningDistribution,
      const Implementation & conditionedDistribution,
      const NumericalScalar xMin) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) with reuse of expansive data */
  NumericalScalar computeConditionalCDFForQuantile(const NumericalScalar x,
      const NumericalPoint & y,
      const Implementation & conditioningDistribution,
      const Implementation & conditionedDistribution,
      const NumericalScalar xMin) const;

  /** Compute the characteristic function of 1D distributions in a regular pattern with cache */
  virtual NumericalComplex computeCharacteristicFunction(const UnsignedInteger index,
      const NumericalScalar step) const;
  virtual NumericalComplex computeLogCharacteristicFunction(const UnsignedInteger index,
      const NumericalScalar step) const;
  virtual NumericalComplex computeCharacteristicFunction(const Indices & indices,
      const NumericalPoint & step) const;
  virtual NumericalComplex computeLogCharacteristicFunction(const Indices & indices,
      const NumericalPoint & step) const;

  /** Compute the mean of the distribution */
  virtual void computeMean() const;

  /** Compute the covariance of the distribution */
  virtual void computeCovariance() const;
  virtual void computeCovarianceContinuous() const;
  virtual void computeCovarianceDiscrete() const;
  virtual void computeCovarianceGeneral() const;

  /** Compute the shifted moments of the distribution */
  virtual NumericalPoint computeShiftedMoment(const UnsignedInteger n,
      const NumericalPoint & shift) const;
  virtual NumericalPoint computeShiftedMomentContinuous(const UnsignedInteger n,
      const NumericalPoint & shift) const;
  virtual NumericalPoint computeShiftedMomentDiscrete(const UnsignedInteger n,
      const NumericalPoint & shift) const;
  virtual NumericalPoint computeShiftedMomentGeneral(const UnsignedInteger n,
      const NumericalPoint & shift) const;


  /** Compute the nodes and weights of the 1D gauss integration rule over [-1, 1] */
  virtual void computeGaussNodesAndWeights() const;

  /** Dimension accessor */
  void setDimension(UnsignedInteger dim);

  /** Quantile computation for dimension=1 */
  virtual NumericalScalar computeScalarQuantile(const NumericalScalar prob,
      const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  virtual void computeRange();
  virtual NumericalPoint computeLowerBound() const;
  virtual NumericalPoint computeUpperBound() const;

  /** Compute the standard distribution associated with the current distribution */
  virtual void computeStandardDistribution() const;

  /** Interpolate the PDF and CDF for smooth continuous distributions */
  virtual Collection<PiecewiseHermiteEvaluationImplementation> interpolatePDFCDF(const UnsignedInteger n);

  mutable NumericalPoint mean_;
  mutable CovarianceMatrix covariance_;
  mutable NumericalPoint gaussNodes_;
  mutable NumericalPoint gaussWeights_;

  /** The integration nodes number for covariance computation */
  mutable UnsignedInteger integrationNodesNumber_;

  /** Flags to avoid redundant computations */
  mutable Bool isAlreadyComputedMean_;
  mutable Bool isAlreadyComputedCovariance_;
  mutable Bool isAlreadyComputedGaussNodesAndWeights_;

  /** Indicators for PDF and CDF absolute precision. By default, equals to DefaultPDFEpsilon, DefaultCDFEpsilon and DefaultQuantileEpsilon */
  mutable NumericalScalar pdfEpsilon_;
  mutable NumericalScalar cdfEpsilon_;
  mutable NumericalScalar quantileEpsilon_;
  mutable UnsignedInteger quantileIterations_;

  /** Standard distribution */
  mutable Bool isAlreadyComputedStandardDistribution_;
  mutable Implementation p_standardDistribution_;

  /** Optimization for the generating function evaluation */
  mutable Bool isAlreadyCreatedGeneratingFunction_;
  mutable UniVariatePolynomial generatingFunction_;

#ifndef SWIG

  // Structure used to wrap the computePDF() method for interpolation purpose
  struct PDFWrapper
  {
    PDFWrapper(const DistributionImplementation * p_distribution):
      p_distribution_(p_distribution) {};

    NumericalPoint computePDF(const NumericalPoint & point) const
    {
      return NumericalPoint(1, p_distribution_->computePDF(point));
    };

    const DistributionImplementation * p_distribution_;
  }; // struct PDFWrapper

  // Structure used to wrap the computePDF() method for the computation of the conditional CDF
  struct ConditionalPDFWrapper
  {
    ConditionalPDFWrapper(const NumericalPoint & y,
                          const Implementation p_distribution)
      : y_(y)
      , p_distribution_(p_distribution)
    {};

    NumericalPoint computeConditionalPDF(const NumericalPoint & point) const
    {
      NumericalPoint z(y_);
      z.add(point[0]);
      return NumericalPoint(1, p_distribution_->computePDF(z));
    };

    const NumericalPoint y_;
    const Implementation p_distribution_;
  }; // struct ConditionalPDFWrapper

  // Structure used to wrap the computeCDF() method for interpolation purpose
  struct CDFWrapper
  {
    CDFWrapper(const DistributionImplementation * p_distribution):
      p_distribution_(p_distribution) {};

    NumericalPoint computeCDF(const NumericalPoint & point) const
    {
      return NumericalPoint(1, p_distribution_->computeCDF(point));
    };

    const DistributionImplementation * p_distribution_;
  }; // struct CDFWrapper

  // Structure used to implement the computeQuantile() method efficiently
  struct QuantileWrapper
  {
    QuantileWrapper(const Collection< Implementation > marginals,
                    const DistributionImplementation * p_distribution)
      : marginals_(marginals)
      , p_distribution_(p_distribution)
      , dimension_(p_distribution->getDimension())
    {
      // Nothing to do
    }

    NumericalPoint computeDiagonal(const NumericalPoint & u) const
    {
      const NumericalScalar cdf(p_distribution_->computeCDF(diagonalToSpace(u[0])));
      LOGDEBUG(OSS() << "in DistributionImplementation::QuantileWrapper::computeDiagonal, u=" << u << ", cdf=" << cdf);
      return NumericalPoint(1, cdf);
    }

    NumericalPoint diagonalToSpace(const NumericalScalar tau) const
    {
      NumericalPoint x(dimension_);
      for (UnsignedInteger i = 0; i < dimension_; ++i) x[i] = marginals_[i]->computeQuantile(tau)[0];
      LOGDEBUG(OSS() << "in DistributionImplementation::QuantileWrapper::diagonalToSpace, tau=" << tau << ", x=" << x);
      return x;
    }

    const Collection< Implementation > marginals_;
    const DistributionImplementation * p_distribution_;
    const UnsignedInteger dimension_;
  }; // struct QuantileWrapper

#endif

  /** The dimension of the distributionImplementation */
  UnsignedInteger dimension_;

  /** The weight used ONLY by Mixture */
  NumericalScalar weight_;

  /** Range of the distribution */
  Interval range_;

  /** Description of each component */
  Description description_;

  /** Use parallelism */
  Bool isParallel_;

  /** Data for characteristic function computation */
  mutable Bool isInitializedCF_;

  mutable NumericalPoint pdfGrid_;

}; /* class DistributionImplementation */

#ifndef SWIG

/** Product operator */
OT_API DistributionImplementation::Implementation operator * (const NumericalScalar,
    const DistributionImplementation & distribution);

OT_API DistributionImplementation::Implementation operator * (const NumericalScalar,
    const DistributionImplementation::Implementation & p_distribution);

/** Division operator */
OT_API DistributionImplementation::Implementation operator / (const NumericalScalar,
    const DistributionImplementation & distribution);

OT_API DistributionImplementation::Implementation operator / (const NumericalScalar,
    const DistributionImplementation::Implementation & p_distribution);

/** Addition operator */
OT_API DistributionImplementation::Implementation operator + (const NumericalScalar,
    const DistributionImplementation & distribution);

OT_API DistributionImplementation::Implementation operator + (const NumericalScalar,
    const DistributionImplementation::Implementation & p_distribution);

/** Substraction operator */
OT_API DistributionImplementation::Implementation operator - (const NumericalScalar,
    const DistributionImplementation & distribution);

OT_API DistributionImplementation::Implementation operator - (const NumericalScalar,
    const DistributionImplementation::Implementation & p_distribution);

OT_API DistributionImplementation::Implementation operator - (const DistributionImplementation & distribution);

OT_API DistributionImplementation::Implementation operator - (const DistributionImplementation::Implementation & p_distribution);


#endif

OT_API DistributionImplementation::Implementation maximum(const DistributionImplementation::Implementation & p_left,
    const DistributionImplementation::Implementation & p_right);

OT_API DistributionImplementation::Implementation maximum(const DistributionImplementation & left,
    const DistributionImplementation::Implementation & p_right);

OT_API DistributionImplementation::Implementation maximum(const DistributionImplementation::Implementation & p_left,
    const DistributionImplementation & right);

OT_API DistributionImplementation::Implementation maximum(const DistributionImplementation & left,
    const DistributionImplementation & right);


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONIMPLEMENTATION_HXX */
