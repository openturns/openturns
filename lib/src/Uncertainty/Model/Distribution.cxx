//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Distribution.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Distribution)

/* Default constructor */
Distribution::Distribution()
  : TypedInterfaceObject<DistributionImplementation>(new Uniform())
{
  // Nothing to do
}

/* Default constructor */
Distribution::Distribution(const DistributionImplementation & implementation)
  : TypedInterfaceObject<DistributionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
Distribution::Distribution(const Implementation & p_implementation)
  : TypedInterfaceObject<DistributionImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Constructor from implementation pointer */
Distribution::Distribution(DistributionImplementation * p_implementation)
  : TypedInterfaceObject<DistributionImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Comparison operator */
Bool Distribution::operator ==(const Distribution & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* Comparison operator */
Bool Distribution::operator !=(const Distribution & other) const
{
  return !operator==(other);
}

/* Addition operator */
Distribution Distribution::operator + (const Distribution & other) const
{
  return getImplementation()->operator +(other);
}

Distribution Distribution::operator + (const Scalar value) const
{
  return getImplementation()->operator +(value);
}

/* Subtraction operator */
Distribution Distribution::operator - (const Distribution & other) const
{
  return getImplementation()->operator -(other);
}

Distribution Distribution::operator - (const Scalar value) const
{
  return getImplementation()->operator -(value);
}

/* Multiplication operator */
Distribution Distribution::operator * (const Distribution & other) const
{
  return getImplementation()->operator *(other);
}

Distribution Distribution::operator * (const Scalar value) const
{
  return getImplementation()->operator *(value);
}

/* Division operator */
Distribution Distribution::operator / (const Distribution & other) const
{
  return getImplementation()->operator /(other);
}

Distribution Distribution::operator / (const Scalar value) const
{
  return getImplementation()->operator /(value);
}

/* Transformation of distributions by usual functions */
Distribution Distribution::cos() const
{
  return getImplementation()->cos();
}

Distribution Distribution::sin() const
{
  return getImplementation()->sin();
}

Distribution Distribution::tan() const
{
  return getImplementation()->tan();
}

Distribution Distribution::acos() const
{
  return getImplementation()->acos();
}

Distribution Distribution::asin() const
{
  return getImplementation()->asin();
}

Distribution Distribution::atan() const
{
  return getImplementation()->atan();
}

Distribution Distribution::cosh() const
{
  return getImplementation()->cosh();
}

Distribution Distribution::sinh() const
{
  return getImplementation()->sinh();
}

Distribution Distribution::tanh() const
{
  return getImplementation()->tanh();
}

Distribution Distribution::acosh() const
{
  return getImplementation()->acosh();
}

Distribution Distribution::asinh() const
{
  return getImplementation()->asinh();
}

Distribution Distribution::atanh() const
{
  return getImplementation()->atanh();
}

Distribution Distribution::exp() const
{
  return getImplementation()->exp();
}

Distribution Distribution::log() const
{
  return getImplementation()->log();
}

Distribution Distribution::ln() const
{
  return getImplementation()->ln();
}

Distribution Distribution::pow(const Scalar exponent) const
{
  return getImplementation()->pow(exponent);
}

Distribution Distribution::pow(const SignedInteger exponent) const
{
  return getImplementation()->pow(exponent);
}

Distribution Distribution::sqr() const
{
  return getImplementation()->sqr();
}

Distribution Distribution::inverse() const
{
  return getImplementation()->inverse();
}

Distribution Distribution::sqrt() const
{
  return getImplementation()->sqrt();
}

Distribution Distribution::cbrt() const
{
  return getImplementation()->cbrt();
}

Distribution Distribution::abs() const
{
  return getImplementation()->abs();
}

/* String converter */
String Distribution::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String Distribution::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* String html converter */
String Distribution::_repr_html_() const
{
  return getImplementation()->_repr_html_();
}

/* String markdown converter */
String Distribution::__repr_markdown__() const
{
  return getImplementation()->__repr_markdown__();
}

/* Weight accessor */
void Distribution::setWeight(const Scalar w)
{
  copyOnWrite();
  getImplementation()->setWeight(w);
}

/* Weight accessor */
Scalar Distribution::getWeight() const
{
  return getImplementation()->getWeight();
}


/* Dimension accessor */
UnsignedInteger Distribution::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Distribution::getRoughness() const
{
  return getImplementation()->getRoughness();
}

/* Get the mean of the distribution */
Point Distribution::getMean() const
{
  return getImplementation()->getMean();
}

/* Get the standard deviation of the distribution */
Point Distribution::getStandardDeviation() const
{
  return getImplementation()->getStandardDeviation();
}

/* Get the skewness of the distribution */
Point Distribution::getSkewness() const
{
  return getImplementation()->getSkewness();
}

/* Get the kurtosis of the distribution */
Point Distribution::getKurtosis() const
{
  return getImplementation()->getKurtosis();
}

/* Get the raw moments of the distribution */
Point Distribution::getMoment(const UnsignedInteger n) const
{
  return getImplementation()->getMoment(n);
}

/* Get the centered moments about the mean of the distribution */
Point Distribution::getCentralMoment(const UnsignedInteger n) const
{
  return getImplementation()->getCentralMoment(n);
}

/* Get the shifted moments of the distribution */
Point Distribution::getShiftedMoment(const UnsignedInteger n,
                                     const Point & shift) const
{
  return getImplementation()->getShiftedMoment(n, shift);
}

/* Get the covariance of the distribution */
CovarianceMatrix Distribution::getCovariance() const
{
  return getImplementation()->getCovariance();
}

/* Cholesky factor of the covariance matrix accessor */
TriangularMatrix Distribution::getCholesky() const
{
  return getImplementation()->getCholesky();
}

/* Inverse of the Cholesky factor of the covariance matrix accessor */
TriangularMatrix Distribution::getInverseCholesky() const
{
  return getImplementation()->getInverseCholesky();
}

/* Get the linear correlation of the distribution */
CorrelationMatrix Distribution::getCorrelation() const
{
  return getImplementation()->getCorrelation();
}

CorrelationMatrix Distribution::getPearsonCorrelation() const
{
  return getImplementation()->getPearsonCorrelation();
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix Distribution::getSpearmanCorrelation() const
{
  return getImplementation()->getSpearmanCorrelation();
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix Distribution::getKendallTau() const
{
  return getImplementation()->getKendallTau();
}

/* Get the Shape matrix of the copula */
CorrelationMatrix Distribution::getShapeMatrix() const
{
  return getImplementation()->getShapeMatrix();
}

/* Get one realization of the distribution */
Point Distribution::getRealization() const
{
  return getImplementation()->getRealization();
}

/* Get a numerical sample whose elements follow the distribution */
Sample Distribution::getSample(const UnsignedInteger size) const
{
  return getImplementation()->getSample(size);
}

Sample Distribution::getSampleByInversion(const UnsignedInteger size) const
{
  return getImplementation()->getSampleByInversion(size);
}

Sample Distribution::getSampleByQMC(const UnsignedInteger size) const
{
  return getImplementation()->getSampleByQMC(size);
}

/* Get the DDF of the distribution */
Point Distribution::computeDDF(const Point & point) const
{
  return getImplementation()->computeDDF(point);
}

/* Get the PDF of the distribution */
Scalar Distribution::computePDF(const Point & point) const
{
  return getImplementation()->computePDF(point);
}

Scalar Distribution::computeLogPDF(const Point & point) const
{
  return getImplementation()->computeLogPDF(point);
}

/* Get the CDF of the distribution */
Scalar Distribution::computeCDF(const Point & point) const
{
  return getImplementation()->computeCDF(point);
}

Scalar Distribution::computeComplementaryCDF(const Point & point) const
{
  return getImplementation()->computeComplementaryCDF(point);
}

Scalar Distribution::computeSurvivalFunction(const Point & point) const
{
  return getImplementation()->computeSurvivalFunction(point);
}

Point Distribution::computeInverseSurvivalFunction(const Scalar prob) const
{
  return getImplementation()->computeInverseSurvivalFunction(prob);
}

Point Distribution::computeInverseSurvivalFunction(const Scalar prob,
    Scalar & marginalProb) const
{
  return getImplementation()->computeInverseSurvivalFunction(prob, marginalProb);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Distribution::computeCharacteristicFunction(const Scalar x) const
{
  return getImplementation()->computeCharacteristicFunction(x);
}

Complex Distribution::computeLogCharacteristicFunction(const Scalar x) const
{
  return getImplementation()->computeLogCharacteristicFunction(x);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Scalar Distribution::computeGeneratingFunction(const Scalar z) const
{
  return getImplementation()->computeGeneratingFunction(z);
}

Scalar Distribution::computeLogGeneratingFunction(const Scalar z) const
{
  return getImplementation()->computeLogGeneratingFunction(z);
}

Complex Distribution::computeGeneratingFunction(const Complex & z) const
{
  return getImplementation()->computeGeneratingFunction(z);
}

Complex Distribution::computeLogGeneratingFunction(const Complex & z) const
{
  return getImplementation()->computeLogGeneratingFunction(z);
}

/* Compute the entropy of the distribution */
Scalar Distribution::computeEntropy() const
{
  return getImplementation()->computeEntropy();
}

/* Get the DDF of the distribution (for a sample) */
Sample Distribution::computeDDF(const Sample & sample) const
{
  return getImplementation()->computeDDF(sample);
}

/* Get the PDF of the distribution (for a sample) */
Sample Distribution::computePDF(const Sample & sample) const
{
  return getImplementation()->computePDF(sample);
}

Sample Distribution::computeLogPDF(const Sample & sample) const
{
  return getImplementation()->computeLogPDF(sample);
}

/* Get the CDF of the distribution (for a sample) */
Sample Distribution::computeCDF(const Sample & sample) const
{
  return getImplementation()->computeCDF(sample);
}

Sample Distribution::computeComplementaryCDF(const Sample & sample) const
{
  return getImplementation()->computeComplementaryCDF(sample);
}

Sample Distribution::computeSurvivalFunction(const Sample & sample) const
{
  return getImplementation()->computeSurvivalFunction(sample);
}

/* Get the DDF of the distribution (for a scalar) */
Scalar Distribution::computeDDF(const Scalar scalar) const
{
  return getImplementation()->computeDDF(scalar);
}

/* Get the PDF of the distribution (for a scalar) */
Scalar Distribution::computePDF(const Scalar scalar) const
{
  return getImplementation()->computePDF(scalar);
}

Scalar Distribution::computeLogPDF(const Scalar scalar) const
{
  return getImplementation()->computeLogPDF(scalar);
}

/* Get the CDF of the distribution (for a scalar) */
Scalar Distribution::computeCDF(const Scalar scalar) const
{
  return getImplementation()->computeCDF(scalar);
}

Scalar Distribution::computeComplementaryCDF(const Scalar scalar) const
{
  return getImplementation()->computeComplementaryCDF(scalar);
}

Scalar Distribution::computeSurvivalFunction(const Scalar scalar) const
{
  return getImplementation()->computeSurvivalFunction(scalar);
}

/* Get the probability content of an interval */
Scalar Distribution::computeProbability(const Interval & interval) const
{
  return getImplementation()->computeProbability(interval);
}

/*  Compute the PDF of 1D distributions over a regular grid */
Sample Distribution::computePDF(const Scalar xMin,
                                const Scalar xMax,
                                const UnsignedInteger pointNumber) const
{
  Sample grid;
  return getImplementation()->computePDF(xMin, xMax, pointNumber, grid);
}

/*  Compute the log-PDF of 1D distributions over a regular grid */
Sample Distribution::computeLogPDF(const Scalar xMin,
                                   const Scalar xMax,
                                   const UnsignedInteger pointNumber) const
{
  Sample grid;
  return getImplementation()->computeLogPDF(xMin, xMax, pointNumber, grid);
}

/*  Compute the CDF of 1D distributions over a regular grid */
Sample Distribution::computeCDF(const Scalar xMin,
                                const Scalar xMax,
                                const UnsignedInteger pointNumber) const
{
  Sample grid;
  return getImplementation()->computeCDF(xMin, xMax, pointNumber, grid);
}

Sample Distribution::computeComplementaryCDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber) const
{
  Sample grid;
  return getImplementation()->computeComplementaryCDF(xMin, xMax, pointNumber, grid);
}

/* Get the PDF gradient of the distribution */
Point Distribution::computePDFGradient(const Point & point) const
{
  return getImplementation()->computePDFGradient(point);
}

Sample Distribution::computePDFGradient(const Sample & sample) const
{
  return getImplementation()->computePDFGradient(sample);
}

/* Get the logPDF gradient of the distribution */
Point Distribution::computeLogPDFGradient(const Point & point) const
{
  return getImplementation()->computeLogPDFGradient(point);
}

Sample Distribution::computeLogPDFGradient(const Sample & sample) const
{
  return getImplementation()->computeLogPDFGradient(sample);
}


/* Get the CDF gradient of the distribution */
Point Distribution::computeCDFGradient(const Point & point) const
{
  return getImplementation()->computeCDFGradient(point);
}

Sample Distribution::computeCDFGradient(const Sample & sample) const
{
  return getImplementation()->computeCDFGradient(sample);
}

/* Get the mathematical and numerical range of the distribution.
   Its mathematical range is the smallest closed interval outside
   of which the PDF is zero, and the numerical range is the interval
   outside of which the PDF is rounded to zero in double precision */
Interval Distribution::getRange() const
{
  return getImplementation()->getRange();
}

/* Get the quantile of the distribution */
Point Distribution::computeQuantile(const Scalar prob,
                                    const Bool tail) const
{
  return getImplementation()->computeQuantile(prob, tail);
}

Point Distribution::computeQuantile(const Scalar prob,
                                    const Bool tail,
                                    Scalar & marginalProb) const
{
  return getImplementation()->computeQuantile(prob, tail, marginalProb);
}

Sample Distribution::computeQuantile(const Point & prob,
                                     const Bool tail) const
{
  return getImplementation()->computeQuantile(prob, tail);
}

Scalar Distribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  return getImplementation()->computeScalarQuantile(prob, tail);
}

/* Get the product minimum volume interval containing at least a given probability of the distribution.
   The minimum volume interval [a, b] is such that:
   a\in[lowerBound, F^{-1}(1-p)]
   b = F^{-1}(p+F(a))
   f(a) = f(b) = f(F^{-1}(p+F(a)))
   so we look for the root of f(F^{-1}(p+F(a))) - f(a)
*/
Interval Distribution::computeMinimumVolumeInterval(const Scalar prob) const
{
  return getImplementation()->computeMinimumVolumeInterval(prob);
}

Interval Distribution::computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return getImplementation()->computeMinimumVolumeIntervalWithMarginalProbability(prob, marginalProb);
}

/* Get the product bilateral confidence interval containing at least a given probability of the distribution.
   The bilateral confidence interval [a, b] is such that:
   for all i\in{1,...,d}, P(X_i\in[a_i, b_i])=\beta
   where \beta is such that P(X\in\prod_{i=1}^d[a_i, b_i])=p
*/
Interval Distribution::computeBilateralConfidenceInterval(const Scalar prob) const
{
  return getImplementation()->computeBilateralConfidenceInterval(prob);
}

Interval Distribution::computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return getImplementation()->computeBilateralConfidenceIntervalWithMarginalProbability(prob, marginalProb);
}

/* Get the product unilateral confidence interval containing at least a given probability of the distribution.
   The bilateral confidence interval [a, b] is such that:
   if upper == false
   for all i\in{1,...,d}, a_i=-\intfy and P(X_i<=b_i)=\beta
   if upper == true
   for all i\in{1,...,d}, P(a_i<=X_i)=\beta and b_i=\intfy

   where in both cases \beta is such that P(X\in\prod_{i=1}^d[a_i, b_i])=p
*/
Interval Distribution::computeUnilateralConfidenceInterval(const Scalar prob,
    const Bool tail) const
{
  return getImplementation()->computeUnilateralConfidenceInterval(prob, tail);
}

Interval Distribution::computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, const Bool tail, Scalar & marginalProb) const
{
  return getImplementation()->computeUnilateralConfidenceIntervalWithMarginalProbability(prob, tail, marginalProb);
}

/* Get the minimum volume level set containing at least a given probability of the distribution.
   The minimum volume level A(p) set is such that A(p)={x\in R^n | y(x) <= y_p}
   where y(x)=-\log X and y_p is the p-quantile of Y=pdf(X)
*/
LevelSet Distribution::computeMinimumVolumeLevelSet(const Scalar prob) const
{
  return getImplementation()->computeMinimumVolumeLevelSet(prob);
}

LevelSet Distribution::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & threshold) const
{
  return getImplementation()->computeMinimumVolumeLevelSetWithThreshold(prob, threshold);
}

/* Compute the quantile over a regular grid */
Sample Distribution::computeQuantile(const Scalar qMin,
                                     const Scalar qMax,
                                     const UnsignedInteger pointNumber,
                                     const Bool tail) const
{
  return getImplementation()->computeQuantile(qMin, qMax, pointNumber, tail);
}

/* Check if the distribution is elliptical */
Bool Distribution::isElliptical() const
{
  return getImplementation()->isElliptical();
}

/* Check if the distribution is a copula */
Bool Distribution::isCopula() const
{
  return getImplementation()->isCopula();
}

/* Check if the distribution is continuous */
Bool Distribution::isContinuous() const
{
  return getImplementation()->isContinuous();
}

/* Check if the distribution is discrete */
Bool Distribution::isDiscrete() const
{
  return getImplementation()->isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool Distribution::isIntegral() const
{
  return getImplementation()->isIntegral();
}

/* Tell if the distribution has elliptical copula */
Bool Distribution::hasEllipticalCopula() const
{
  return getImplementation()->hasEllipticalCopula();
}

/* Tell if the distribution has independent copula */
Bool Distribution::hasIndependentCopula() const
{
  return getImplementation()->hasIndependentCopula();
}

/* Get the support of a distribution that intersect a given interval */
Sample Distribution::getSupport(const Interval & interval) const
{
  return getImplementation()->getSupport(interval);
}

/* Get the support on the whole range */
Sample Distribution::getSupport() const
{
  return getImplementation()->getSupport();
}

/* Get the discrete probability levels */
Point Distribution::getProbabilities() const
{
  return getImplementation()->getProbabilities();
}

/* Get the PDF singularities inside of the range - 1D only */
Point Distribution::getSingularities() const
{
  return getImplementation()->getSingularities();
}

/* Compute the radial distribution CDF */
Scalar Distribution::computeRadialDistributionCDF (const Scalar radius,
    const Bool tail) const
{
  return getImplementation()->computeRadialDistributionCDF(radius, tail);
}

/* Get the i-th marginal distribution */
Distribution Distribution::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Distribution::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Get the copula of a distribution */
Distribution Distribution::getCopula() const
{
  return getImplementation()->getCopula();
}

/* Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Distribution::computeConditionalDDF(const Scalar x, const Point & y) const
{
  return getImplementation()->computeConditionalDDF(x, y);
}

Point Distribution::computeSequentialConditionalDDF(const Point & x) const
{
  return getImplementation()->computeSequentialConditionalDDF(x);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Distribution::computeConditionalPDF(const Scalar x, const Point & y) const
{
  return getImplementation()->computeConditionalPDF(x, y);
}

Point Distribution::computeSequentialConditionalPDF(const Point & x) const
{
  return getImplementation()->computeSequentialConditionalPDF(x);
}

Point Distribution::computeConditionalPDF(const Point & x, const Sample & y) const
{
  return getImplementation()->computeConditionalPDF(x, y);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Distribution::computeConditionalCDF(const Scalar x, const Point & y) const
{
  return getImplementation()->computeConditionalCDF(x, y);
}

Point Distribution::computeSequentialConditionalCDF(const Point & x) const
{
  return getImplementation()->computeSequentialConditionalCDF(x);
}

Point Distribution::computeConditionalCDF(const Point & x, const Sample & y) const
{
  return getImplementation()->computeConditionalCDF(x, y);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar Distribution::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  return getImplementation()->computeConditionalQuantile(q, y);
}

Point Distribution::computeSequentialConditionalQuantile(const Point & q) const
{
  return getImplementation()->computeSequentialConditionalQuantile(q);
}

Point Distribution::computeConditionalQuantile(const Point & q, const Sample & y) const
{
  return getImplementation()->computeConditionalQuantile(q, y);
}

/* Get the isoprobabilist transformation */
Distribution::IsoProbabilisticTransformation Distribution::getIsoProbabilisticTransformation() const
{
  return getImplementation()->getIsoProbabilisticTransformation();
}

/* Get the inverse isoprobabilist transformation */
Distribution::InverseIsoProbabilisticTransformation Distribution::getInverseIsoProbabilisticTransformation() const
{
  return getImplementation()->getInverseIsoProbabilisticTransformation();
}

/* Get the standard distribution */
Distribution Distribution::getStandardDistribution() const
{
  return getImplementation()->getStandardDistribution();
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Distribution::getStandardRepresentative() const
{
  return getImplementation()->getStandardRepresentative();
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph Distribution::drawPDF(const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            const Bool logScale) const
{
  return getImplementation()->drawPDF(xMin, xMax, pointNumber, logScale);
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph Distribution::drawPDF(const UnsignedInteger pointNumber,
                            const Bool logScale) const
{
  return getImplementation()->drawPDF(pointNumber, logScale);
}

/* Draw the PDF of a 1D marginal */
Graph Distribution::drawMarginal1DPDF(const UnsignedInteger marginalIndex,
                                      const Scalar xMin,
                                      const Scalar xMax,
                                      const UnsignedInteger pointNumber,
                                      const Bool logScale) const
{
  return getImplementation()->drawMarginal1DPDF(marginalIndex, xMin, xMax, pointNumber, logScale);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph Distribution::drawPDF(const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            const Bool logScaleX,
                            const Bool logScaleY) const
{
  return getImplementation()->drawPDF(xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph Distribution::drawPDF(const Point & xMin,
                            const Point & xMax,
                            const Bool logScaleX,
                            const Bool logScaleY) const
{
  return getImplementation()->drawPDF(xMin, xMax, logScaleX, logScaleY);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph Distribution::drawPDF(const Indices & pointNumber,
                            const Bool logScaleX,
                            const Bool logScaleY) const
{
  return getImplementation()->drawPDF(pointNumber, logScaleX, logScaleY);
}

/* Draw the PDF of a 2D marginal */
Graph Distribution::drawMarginal2DPDF(const UnsignedInteger firstMarginal,
                                      const UnsignedInteger secondMarginal,
                                      const Point & xMin,
                                      const Point & xMax,
                                      const Indices & pointNumber,
                                      const Bool logScaleX,
                                      const Bool logScaleY) const
{
  return getImplementation()->drawMarginal2DPDF(firstMarginal, secondMarginal, xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph Distribution::drawLogPDF(const Scalar xMin,
                               const Scalar xMax,
                               const UnsignedInteger pointNumber,
                               const Bool logScale) const
{
  return getImplementation()->drawLogPDF(xMin, xMax, pointNumber, logScale);
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph Distribution::drawLogPDF(const UnsignedInteger pointNumber,
                               const Bool logScale) const
{
  return getImplementation()->drawLogPDF(pointNumber, logScale);
}

/* Draw the log-PDF of a 1D marginal */
Graph Distribution::drawMarginal1DLogPDF(const UnsignedInteger marginalIndex,
    const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  return getImplementation()->drawMarginal1DLogPDF(marginalIndex, xMin, xMax, pointNumber, logScale);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph Distribution::drawLogPDF(const Point & xMin,
                               const Point & xMax,
                               const Indices & pointNumber,
                               const Bool logScaleX,
                               const Bool logScaleY) const
{
  return getImplementation()->drawLogPDF(xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph Distribution::drawLogPDF(const Point & xMin,
                               const Point & xMax,
                               const Bool logScaleX,
                               const Bool logScaleY) const
{
  return getImplementation()->drawLogPDF(xMin, xMax, logScaleX, logScaleY);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph Distribution::drawLogPDF(const Indices & pointNumber,
                               const Bool logScaleX,
                               const Bool logScaleY) const
{
  return getImplementation()->drawLogPDF(pointNumber, logScaleX, logScaleY);
}

/* Draw the log-PDF of a 2D marginal */
Graph Distribution::drawMarginal2DLogPDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return getImplementation()->drawMarginal2DLogPDF(firstMarginal, secondMarginal, xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph Distribution::drawCDF(const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            const Bool logScale) const
{
  return getImplementation()->drawCDF(xMin, xMax, pointNumber, logScale);
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph Distribution::drawCDF(const UnsignedInteger pointNumber,
                            const Bool logScale) const
{
  return getImplementation()->drawCDF(pointNumber, logScale);
}

/* Draw the CDF of a 1D marginal */
Graph Distribution::drawMarginal1DCDF(const UnsignedInteger marginalIndex,
                                      const Scalar xMin,
                                      const Scalar xMax,
                                      const UnsignedInteger pointNumber,
                                      const Bool logScale) const
{
  return getImplementation()->drawMarginal1DCDF(marginalIndex, xMin, xMax, pointNumber, logScale);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph Distribution::drawCDF(const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            const Bool logScaleX,
                            const Bool logScaleY) const
{
  return getImplementation()->drawCDF(xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph Distribution::drawCDF(const Point & xMin,
                            const Point & xMax,
                            const Bool logScaleX,
                            const Bool logScaleY) const
{
  return getImplementation()->drawCDF(xMin, xMax, logScaleX, logScaleY);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph Distribution::drawCDF(const Indices & pointNumber,
                            const Bool logScaleX,
                            const Bool logScaleY) const
{
  return getImplementation()->drawCDF(pointNumber, logScaleX, logScaleY);
}

/* Draw the CDF of a 2D marginal */
Graph Distribution::drawMarginal2DCDF(const UnsignedInteger firstMarginal,
                                      const UnsignedInteger secondMarginal,
                                      const Point & xMin,
                                      const Point & xMax,
                                      const Indices & pointNumber,
                                      const Bool logScaleX,
                                      const Bool logScaleY) const
{
  return getImplementation()->drawMarginal2DCDF(firstMarginal, secondMarginal, xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the SurvivalFunction of the distribution when its dimension is 1 */
Graph Distribution::drawSurvivalFunction(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  return getImplementation()->drawSurvivalFunction(xMin, xMax, pointNumber, logScale);
}

/* Draw the SurvivalFunction of the distribution when its dimension is 1 */
Graph Distribution::drawSurvivalFunction(const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  return getImplementation()->drawSurvivalFunction(pointNumber, logScale);
}

/* Draw the SurvivalFunction of a 1D marginal */
Graph Distribution::drawMarginal1DSurvivalFunction(const UnsignedInteger marginalIndex,
    const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  return getImplementation()->drawMarginal1DSurvivalFunction(marginalIndex, xMin, xMax, pointNumber, logScale);
}

/* Draw the SurvivalFunction of the distribution when its dimension is 2 */
Graph Distribution::drawSurvivalFunction(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return getImplementation()->drawSurvivalFunction(xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the SurvivalFunction of the distribution when its dimension is 2 */
Graph Distribution::drawSurvivalFunction(const Point & xMin,
    const Point & xMax,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return getImplementation()->drawSurvivalFunction(xMin, xMax, logScaleX, logScaleY);
}

/* Draw the SurvivalFunction of the distribution when its dimension is 2 */
Graph Distribution::drawSurvivalFunction(const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return getImplementation()->drawSurvivalFunction(pointNumber, logScaleX, logScaleY);
}

/* Draw the SurvivalFunction of a 2D marginal */
Graph Distribution::drawMarginal2DSurvivalFunction(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return getImplementation()->drawMarginal2DSurvivalFunction(firstMarginal, secondMarginal, xMin, xMax, pointNumber, logScaleX, logScaleY);
}

/* Draw the quantile of the distribution when its dimension is 1 or 2 */
Graph Distribution::drawQuantile(const UnsignedInteger pointNumber,
                                 const Bool logScale) const
{
  return getImplementation()->drawQuantile(pointNumber, logScale);
}

Graph Distribution::drawQuantile(const Scalar qMin,
                                 const Scalar qMax,
                                 const UnsignedInteger pointNumber,
                                 const Bool logScale) const
{
  return getImplementation()->drawQuantile(qMin, qMax, pointNumber, logScale);
}

/* Dependence functions */
CorrelationMatrix Distribution::computeUpperTailDependenceMatrix() const
{
  return getImplementation()->computeUpperTailDependenceMatrix();
}

CorrelationMatrix Distribution::computeUpperExtremalDependenceMatrix() const
{
  return getImplementation()->computeUpperExtremalDependenceMatrix();
}

CorrelationMatrix Distribution::computeLowerTailDependenceMatrix() const
{
  return getImplementation()->computeLowerTailDependenceMatrix();
}

CorrelationMatrix Distribution::computeLowerExtremalDependenceMatrix() const
{
  return getImplementation()->computeLowerExtremalDependenceMatrix();
}

/* Draw dependence functions */
Graph Distribution::drawUpperTailDependenceFunction() const
{
  return getImplementation()->drawUpperTailDependenceFunction();
}

Graph Distribution::drawUpperExtremalDependenceFunction() const
{
  return getImplementation()->drawUpperExtremalDependenceFunction();
}

Graph Distribution::drawLowerTailDependenceFunction() const
{
  return getImplementation()->drawLowerTailDependenceFunction();
}

Graph Distribution::drawLowerExtremalDependenceFunction() const
{
  return getImplementation()->drawLowerExtremalDependenceFunction();
}

/* Parameters value and description accessor */
Distribution::PointWithDescriptionCollection Distribution::getParametersCollection() const
{
  return getImplementation()->getParametersCollection();
}

void Distribution::setParametersCollection(const PointWithDescriptionCollection & parametersCollection)
{
  copyOnWrite();
  getImplementation()->setParametersCollection(parametersCollection);
}

void Distribution::setParametersCollection(const PointCollection & parametersCollection)
{
  copyOnWrite();
  getImplementation()->setParametersCollection(parametersCollection);
}

/* Parameters value accessor */
void Distribution::setParameter(const Point & parameters)
{
  copyOnWrite();
  getImplementation()->setParameter(parameters);
}

Point Distribution::getParameter() const
{
  return getImplementation()->getParameter();
}

/* Parameters description accessor */
Description Distribution::getParameterDescription() const
{
  return getImplementation()->getParameterDescription();
}

/* Parameters number */
UnsignedInteger Distribution::getParameterDimension() const
{
  return getImplementation()->getParameterDimension();
}

/* Description accessor */
void Distribution::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}

Description Distribution::getDescription() const
{
  return getImplementation()->getDescription();
}

/* Accessor to PDF computation precision */
Scalar Distribution::getPDFEpsilon() const
{
  return getImplementation()->getPDFEpsilon();
}

/* Accessor to CDF computation precision */
Scalar Distribution::getCDFEpsilon() const
{
  return getImplementation()->getCDFEpsilon();
}

/* Get a position indicator for a 1D distribution */
Scalar Distribution::getPositionIndicator() const
{
  return getImplementation()->getPositionIndicator();
}

/* Get a dispersion indicator for a 1D distribution */
Scalar Distribution::getDispersionIndicator() const
{
  return getImplementation()->getDispersionIndicator();
}

END_NAMESPACE_OPENTURNS
