//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Distribution.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Distribution);

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
  return getImplementation()->operator +(other.getImplementation());
}

Distribution Distribution::operator + (const NumericalScalar value) const
{
  return getImplementation()->operator +(value);
}

/* Substraction operator */
Distribution Distribution::operator - (const Distribution & other) const
{
  return getImplementation()->operator -(other.getImplementation());
}

Distribution Distribution::operator - (const NumericalScalar value) const
{
  return getImplementation()->operator -(value);
}

/* Multiplication operator */
Distribution Distribution::operator * (const Distribution & other) const
{
  return getImplementation()->operator *(other.getImplementation());
}

Distribution Distribution::operator * (const NumericalScalar value) const
{
  return getImplementation()->operator *(value);
}

/* Division operator */
Distribution Distribution::operator / (const Distribution & other) const
{
  return getImplementation()->operator /(other.getImplementation());
}

Distribution Distribution::operator / (const NumericalScalar value) const
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

Distribution Distribution::pow(const NumericalScalar exponent) const
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


/* Weight accessor */
void Distribution::setWeight(const NumericalScalar w)
{
  copyOnWrite();
  getImplementation()->setWeight(w);
}

/* Weight accessor */
NumericalScalar Distribution::getWeight() const
{
  return getImplementation()->getWeight();
}


/* Dimension accessor */
UnsignedInteger Distribution::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar Distribution::getRoughness() const
{
  return getImplementation()->getRoughness();
}

/* Get the mean of the distribution */
NumericalPoint Distribution::getMean() const
{
  return getImplementation()->getMean();
}

/* Get the standard deviation of the distribution */
NumericalPoint Distribution::getStandardDeviation() const
{
  return getImplementation()->getStandardDeviation();
}

/* Get the skewness of the distribution */
NumericalPoint Distribution::getSkewness() const
{
  return getImplementation()->getSkewness();
}

/* Get the kurtosis of the distribution */
NumericalPoint Distribution::getKurtosis() const
{
  return getImplementation()->getKurtosis();
}

/* Get the raw moments of the standardized distribution */
NumericalPoint Distribution::getStandardMoment(const UnsignedInteger n) const
{
  return getImplementation()->getStandardMoment(n);
}

/* Get the raw moments of the distribution */
NumericalPoint Distribution::getMoment(const UnsignedInteger n) const
{
  return getImplementation()->getMoment(n);
}

/* Get the centered moments about the mean of the distribution */
NumericalPoint Distribution::getCenteredMoment(const UnsignedInteger n) const
{
  return getImplementation()->getCenteredMoment(n);
}

/* Get the shifted moments of the distribution */
NumericalPoint Distribution::getShiftedMoment(const UnsignedInteger n,
    const NumericalPoint & shift) const
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

CorrelationMatrix Distribution::getLinearCorrelation() const
{
  return getImplementation()->getLinearCorrelation();
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
NumericalPoint Distribution::getRealization() const
{
  return getImplementation()->getRealization();
}

/* Get a numerical sample whose elements follow the distribution */
NumericalSample Distribution::getSample(const UnsignedInteger size) const
{
  return getImplementation()->getSample(size);
}

/* Get the DDF of the distribution */
NumericalPoint Distribution::computeDDF(const NumericalPoint & point) const
{
  return getImplementation()->computeDDF(point);
}

/* Get the PDF of the distribution */
NumericalScalar Distribution::computePDF(const NumericalPoint & point) const
{
  return getImplementation()->computePDF(point);
}

NumericalScalar Distribution::computeLogPDF(const NumericalPoint & point) const
{
  return getImplementation()->computeLogPDF(point);
}

/* Get the CDF of the distribution */
NumericalScalar Distribution::computeCDF(const NumericalPoint & point) const
{
  return getImplementation()->computeCDF(point);
}

NumericalScalar Distribution::computeComplementaryCDF(const NumericalPoint & point) const
{
  return getImplementation()->computeComplementaryCDF(point);
}

NumericalScalar Distribution::computeSurvivalFunction(const NumericalPoint & point) const
{
  return getImplementation()->computeSurvivalFunction(point);
}

NumericalPoint Distribution::computeInverseSurvivalFunction(const NumericalScalar prob) const
{
  return getImplementation()->computeInverseSurvivalFunction(prob);
}

NumericalPoint Distribution::computeInverseSurvivalFunction(const NumericalScalar prob,
							    NumericalScalar & marginalProb) const
{
  return getImplementation()->computeInverseSurvivalFunction(prob, marginalProb);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Distribution::computeCharacteristicFunction(const NumericalScalar x) const
{
  return getImplementation()->computeCharacteristicFunction(x);
}

NumericalComplex Distribution::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return getImplementation()->computeLogCharacteristicFunction(x);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
NumericalScalar Distribution::computeGeneratingFunction(const NumericalScalar z) const
{
  return getImplementation()->computeGeneratingFunction(z);
}

NumericalScalar Distribution::computeLogGeneratingFunction(const NumericalScalar z) const
{
  return getImplementation()->computeLogGeneratingFunction(z);
}

NumericalComplex Distribution::computeGeneratingFunction(const NumericalComplex & z) const
{
  return getImplementation()->computeGeneratingFunction(z);
}

NumericalComplex Distribution::computeLogGeneratingFunction(const NumericalComplex & z) const
{
  return getImplementation()->computeLogGeneratingFunction(z);
}

/* Get the DDF of the distribution (for a sample) */
NumericalSample Distribution::computeDDF(const NumericalSample & sample) const
{
  return getImplementation()->computeDDF(sample);
}

/* Get the PDF of the distribution (for a sample) */
NumericalSample Distribution::computePDF(const NumericalSample & sample) const
{
  return getImplementation()->computePDF(sample);
}

NumericalSample Distribution::computeLogPDF(const NumericalSample & sample) const
{
  return getImplementation()->computeLogPDF(sample);
}

/* Get the CDF of the distribution (for a sample) */
NumericalSample Distribution::computeCDF(const NumericalSample & sample) const
{
  return getImplementation()->computeCDF(sample);
}

NumericalSample Distribution::computeComplementaryCDF(const NumericalSample & sample) const
{
  return getImplementation()->computeComplementaryCDF(sample);
}

NumericalSample Distribution::computeSurvivalFunction(const NumericalSample & sample) const
{
  return getImplementation()->computeSurvivalFunction(sample);
}

/* Get the DDF of the distribution (for a scalar) */
NumericalScalar Distribution::computeDDF(const NumericalScalar scalar) const
{
  return getImplementation()->computeDDF(scalar);
}

/* Get the PDF of the distribution (for a scalar) */
NumericalScalar Distribution::computePDF(const NumericalScalar scalar) const
{
  return getImplementation()->computePDF(scalar);
}

NumericalScalar Distribution::computeLogPDF(const NumericalScalar scalar) const
{
  return getImplementation()->computeLogPDF(scalar);
}

/* Get the CDF of the distribution (for a scalar) */
NumericalScalar Distribution::computeCDF(const NumericalScalar scalar) const
{
  return getImplementation()->computeCDF(scalar);
}

NumericalScalar Distribution::computeComplementaryCDF(const NumericalScalar scalar) const
{
  return getImplementation()->computeComplementaryCDF(scalar);
}

NumericalScalar Distribution::computeSurvivalFunction(const NumericalScalar scalar) const
{
  return getImplementation()->computeSurvivalFunction(scalar);
}

/* Get the probability content of an interval */
NumericalScalar Distribution::computeProbability(const Interval & interval) const
{
  return getImplementation()->computeProbability(interval);
}

/*  Compute the PDF of 1D distributions over a regular grid */
NumericalSample Distribution::computePDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  NumericalSample grid;
  return getImplementation()->computePDF(xMin, xMax, pointNumber, grid);
}

/*  Compute the log-PDF of 1D distributions over a regular grid */
NumericalSample Distribution::computeLogPDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  NumericalSample grid;
  return getImplementation()->computeLogPDF(xMin, xMax, pointNumber, grid);
}

/*  Compute the CDF of 1D distributions over a regular grid */
NumericalSample Distribution::computeCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  NumericalSample grid;
  return getImplementation()->computeCDF(xMin, xMax, pointNumber, grid);
}

NumericalSample Distribution::computeComplementaryCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  NumericalSample grid;
  return getImplementation()->computeComplementaryCDF(xMin, xMax, pointNumber, grid);
}

/* Get the PDF gradient of the distribution */
NumericalPoint Distribution::computePDFGradient(const NumericalPoint & point) const
{
  return getImplementation()->computePDFGradient(point);
}

NumericalSample Distribution::computePDFGradient(const NumericalSample & sample) const
{
  return getImplementation()->computePDFGradient(sample);
}

/* Get the logPDF gradient of the distribution */
NumericalPoint Distribution::computeLogPDFGradient(const NumericalPoint & point) const
{
  return getImplementation()->computeLogPDFGradient(point);
}

NumericalSample Distribution::computeLogPDFGradient(const NumericalSample & sample) const
{
  return getImplementation()->computeLogPDFGradient(sample);
}


/* Get the CDF gradient of the distribution */
NumericalPoint Distribution::computeCDFGradient(const NumericalPoint & point) const
{
  return getImplementation()->computeCDFGradient(point);
}

NumericalSample Distribution::computeCDFGradient(const NumericalSample & sample) const
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
NumericalPoint Distribution::computeQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return getImplementation()->computeQuantile(prob, tail);
}

NumericalPoint Distribution::computeQuantile(const NumericalScalar prob,
					     const Bool tail,
					     NumericalScalar & marginalProb) const
{
  return getImplementation()->computeQuantile(prob, tail, marginalProb);
}

NumericalSample Distribution::computeQuantile(const NumericalPoint & prob,
    const Bool tail) const
{
  return getImplementation()->computeQuantile(prob, tail);
}

/* Get the product minimum volume interval containing at least a given probability of the distribution.
   The minimum volume interval [a, b] is such that:
   a\in[lowerBound, F^{-1}(1-p)]
   b = F^{-1}(p+F(a))
   f(a) = f(b) = f(F^{-1}(p+F(a)))
   so we look for the root of f(F^{-1}(p+F(a))) - f(a)
*/
Interval Distribution::computeMinimumVolumeInterval(const NumericalScalar prob) const
{
  return getImplementation()->computeMinimumVolumeInterval(prob);
}

Interval Distribution::computeMinimumVolumeIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const
{
  return getImplementation()->computeMinimumVolumeIntervalWithMarginalProbability(prob, marginalProb);
}

/* Get the product bilateral confidence interval containing at least a given probability of the distribution.
   The bilateral confidence interval [a, b] is such that:
   for all i\in{1,...,d}, P(X_i\in[a_i, b_i])=\beta
   where \beta is such that P(X\in\prod_{i=1}^d[a_i, b_i])=p
*/
Interval Distribution::computeBilateralConfidenceInterval(const NumericalScalar prob) const
{
  return getImplementation()->computeBilateralConfidenceInterval(prob);
}

Interval Distribution::computeBilateralConfidenceIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const
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
Interval Distribution::computeUnilateralConfidenceInterval(const NumericalScalar prob,
							   const Bool tail) const
{
  return getImplementation()->computeUnilateralConfidenceInterval(prob, tail);
}

Interval Distribution::computeUnilateralConfidenceIntervalWithMarginalProbability(const NumericalScalar prob, const Bool tail, NumericalScalar & marginalProb) const
{
  return getImplementation()->computeUnilateralConfidenceIntervalWithMarginalProbability(prob, tail, marginalProb);
}

/* Get the minimum volume level set containing at least a given probability of the distribution.
   The minimum volume level A(p) set is such that A(p)={x\in R^n | y(x) <= y_p}
   where y(x)=-\log X and y_p is the p-quantile of Y=pdf(X)
*/
LevelSet Distribution::computeMinimumVolumeLevelSet(const NumericalScalar prob) const
{
  return getImplementation()->computeMinimumVolumeLevelSet(prob);
}

LevelSet Distribution::computeMinimumVolumeLevelSetWithThreshold(const NumericalScalar prob, NumericalScalar & threshold) const
{
  return getImplementation()->computeMinimumVolumeLevelSetWithThreshold(prob, threshold);
}

/* Compute the quantile over a regular grid */
NumericalSample Distribution::computeQuantile(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber,
    const Bool tail) const
{
  return getImplementation()->computeQuantile(qMin, qMax, pointNumber, tail);
}

/* Get the implementation of the distribution */
Distribution::Implementation Distribution::getImplementation() const
{
  return p_implementation_;
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
NumericalSample Distribution::getSupport(const Interval & interval) const
{
  return getImplementation()->getSupport(interval);
}

/* Get the support on the whole range */
NumericalSample Distribution::getSupport() const
{
  return getImplementation()->getSupport();
}

/* Get the discrete probability levels */
NumericalPoint Distribution::getProbabilities() const
{
  return getImplementation()->getProbabilities();
}

/* Get the PDF singularities inside of the range - 1D only */
NumericalPoint Distribution::getSingularities() const
{
  return getImplementation()->getSingularities();
}

/* Compute the density generator of the elliptical generator, i.e.
  *  the function phi such that the density of the distribution can
  *  be written as p(x) = phi(t(x-mu)R(x-mu))                      */
NumericalScalar Distribution::computeDensityGenerator(const NumericalScalar betaSquare) const
{
  return getImplementation()->computeDensityGenerator(betaSquare);
}

/* Compute the derivative of the density generator */
NumericalScalar Distribution::computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const
{
  return getImplementation()->computeDensityGeneratorDerivative(betaSquare);
}

/* Compute the seconde derivative of the density generator */
NumericalScalar Distribution::computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const
{
  return getImplementation()->computeDensityGeneratorSecondDerivative(betaSquare);
}

/* Compute the radial distribution CDF */
NumericalScalar Distribution::computeRadialDistributionCDF (const NumericalScalar radius,
    const Bool tail) const
{
  return getImplementation()->computeRadialDistributionCDF(radius, tail);
}

/* Get the i-th marginal distribution */
Distribution Distribution::getMarginal(const UnsignedInteger i) const
{
  return *(getImplementation()->getMarginal(i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Distribution::getMarginal(const Indices & indices) const
{
  return *(getImplementation()->getMarginal(indices));
}

/* Get the copula of a distribution */
Distribution Distribution::getCopula() const
{
  return getImplementation()->getCopula();
}

/* Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar Distribution::computeConditionalDDF(const NumericalScalar x, const NumericalPoint & y) const
{
  return getImplementation()->computeConditionalDDF(x, y);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar Distribution::computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const
{
  return getImplementation()->computeConditionalPDF(x, y);
}

NumericalPoint Distribution::computeConditionalPDF(const NumericalPoint & x, const NumericalSample & y) const
{
  return getImplementation()->computeConditionalPDF(x, y);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar Distribution::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const
{
  return getImplementation()->computeConditionalCDF(x, y);
}

NumericalPoint Distribution::computeConditionalCDF(const NumericalPoint & x, const NumericalSample & y) const
{
  return getImplementation()->computeConditionalCDF(x, y);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar Distribution::computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const
{
  return getImplementation()->computeConditionalQuantile(q, y);
}

NumericalPoint Distribution::computeConditionalQuantile(const NumericalPoint & q, const NumericalSample & y) const
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
Graph Distribution::drawPDF(const NumericalScalar xMin,
                            const NumericalScalar xMax,
                            const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawPDF(xMin, xMax, pointNumber);
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph Distribution::drawPDF(const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawPDF(pointNumber);
}

/* Draw the PDF of a 1D marginal */
Graph Distribution::drawMarginal1DPDF(const UnsignedInteger marginalIndex,
                                      const NumericalScalar xMin,
                                      const NumericalScalar xMax,
                                      const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawMarginal1DPDF(marginalIndex, xMin, xMax, pointNumber);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph Distribution::drawPDF(const NumericalPoint & xMin,
                            const NumericalPoint & xMax,
                            const Indices & pointNumber) const
{
  return getImplementation()->drawPDF(xMin, xMax, pointNumber);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph Distribution::drawPDF(const NumericalPoint & xMin,
                            const NumericalPoint & xMax) const
{
  return getImplementation()->drawPDF(xMin, xMax);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph Distribution::drawPDF(const Indices & pointNumber) const
{
  return getImplementation()->drawPDF(pointNumber);
}

/* Draw the PDF of a 2D marginal */
Graph Distribution::drawMarginal2DPDF(const UnsignedInteger firstMarginal,
                                      const UnsignedInteger secondMarginal,
                                      const NumericalPoint & xMin,
                                      const NumericalPoint & xMax,
                                      const Indices & pointNumber) const
{
  return getImplementation()->drawMarginal2DPDF(firstMarginal, secondMarginal, xMin, xMax, pointNumber);
}

/* Draw the PDF of the distribution when its dimension is 1 or 2 */
Graph Distribution::drawPDF() const
{
  return getImplementation()->drawPDF();
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph Distribution::drawLogPDF(const NumericalScalar xMin,
			       const NumericalScalar xMax,
			       const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawLogPDF(xMin, xMax, pointNumber);
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph Distribution::drawLogPDF(const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawLogPDF(pointNumber);
}

/* Draw the log-PDF of a 1D marginal */
Graph Distribution::drawMarginal1DLogPDF(const UnsignedInteger marginalIndex,
					 const NumericalScalar xMin,
					 const NumericalScalar xMax,
					 const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawMarginal1DLogPDF(marginalIndex, xMin, xMax, pointNumber);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph Distribution::drawLogPDF(const NumericalPoint & xMin,
			       const NumericalPoint & xMax,
			       const Indices & pointNumber) const
{
  return getImplementation()->drawLogPDF(xMin, xMax, pointNumber);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph Distribution::drawLogPDF(const NumericalPoint & xMin,
			       const NumericalPoint & xMax) const
{
  return getImplementation()->drawLogPDF(xMin, xMax);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph Distribution::drawLogPDF(const Indices & pointNumber) const
{
  return getImplementation()->drawLogPDF(pointNumber);
}

/* Draw the log-PDF of a 2D marginal */
Graph Distribution::drawMarginal2DLogPDF(const UnsignedInteger firstMarginal,
					 const UnsignedInteger secondMarginal,
					 const NumericalPoint & xMin,
					 const NumericalPoint & xMax,
					 const Indices & pointNumber) const
{
  return getImplementation()->drawMarginal2DLogPDF(firstMarginal, secondMarginal, xMin, xMax, pointNumber);
}

/* Draw the log-PDF of the distribution when its dimension is 1 or 2 */
Graph Distribution::drawLogPDF() const
{
  return getImplementation()->drawLogPDF();
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph Distribution::drawCDF(const NumericalScalar xMin,
                            const NumericalScalar xMax,
                            const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawCDF(xMin, xMax, pointNumber);
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph Distribution::drawCDF(const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawCDF(pointNumber);
}

/* Draw the CDF of a 1D marginal */
Graph Distribution::drawMarginal1DCDF(const UnsignedInteger marginalIndex,
                                      const NumericalScalar xMin,
                                      const NumericalScalar xMax,
                                      const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawMarginal1DCDF(marginalIndex, xMin, xMax, pointNumber);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph Distribution::drawCDF(const NumericalPoint & xMin,
                            const NumericalPoint & xMax,
                            const Indices & pointNumber) const
{
  return getImplementation()->drawCDF(xMin, xMax, pointNumber);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph Distribution::drawCDF(const NumericalPoint & xMin,
                            const NumericalPoint & xMax) const
{
  return getImplementation()->drawCDF(xMin, xMax);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph Distribution::drawCDF(const Indices & pointNumber) const
{
  return getImplementation()->drawCDF(pointNumber);
}

/* Draw the CDF of a 2D marginal */
Graph Distribution::drawMarginal2DCDF(const UnsignedInteger firstMarginal,
                                      const UnsignedInteger secondMarginal,
                                      const NumericalPoint & xMin,
                                      const NumericalPoint & xMax,
                                      const Indices & pointNumber) const
{
  return getImplementation()->drawMarginal2DCDF(firstMarginal, secondMarginal, xMin, xMax, pointNumber);
}

/* Draw the CDF of the distribution when its dimension is 1 or 2 */
Graph Distribution::drawCDF() const
{
  return getImplementation()->drawCDF();
}

/* Draw the quantile of the distribution when its dimension is 1 or 2 */
Graph Distribution::drawQuantile(const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawQuantile(pointNumber);
}

Graph Distribution::drawQuantile(const NumericalScalar qMin,
                                 const NumericalScalar qMax,
                                 const UnsignedInteger pointNumber) const
{
  return getImplementation()->drawQuantile(qMin, qMax, pointNumber);
}

/* Parameters value and description accessor */
Distribution::NumericalPointWithDescriptionCollection Distribution::getParametersCollection() const
{
  return getImplementation()->getParametersCollection();
}

void Distribution::setParametersCollection(const NumericalPointWithDescriptionCollection & parametersCollection)
{
  copyOnWrite();
  getImplementation()->setParametersCollection(parametersCollection);
}

void Distribution::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  copyOnWrite();
  getImplementation()->setParametersCollection(parametersCollection);
}

/* Parameters value accessor */
void Distribution::setParameter(const NumericalPoint & parameters)
{
  copyOnWrite();
  getImplementation()->setParameter(parameters);
}

NumericalPoint Distribution::getParameter() const
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
NumericalScalar Distribution::getPDFEpsilon() const
{
  return getImplementation()->getPDFEpsilon();
}

/* Accessor to CDF computation precision */
NumericalScalar Distribution::getCDFEpsilon() const
{
  return getImplementation()->getCDFEpsilon();
}

/* Get a positon indicator for a 1D distribution */
NumericalScalar Distribution::getPositionIndicator() const
{
  return getImplementation()->getPositionIndicator();
}

/* Get a dispersion indicator for a 1D distribution */
NumericalScalar Distribution::getDispersionIndicator() const
{
  return getImplementation()->getDispersionIndicator();
}

END_NAMESPACE_OPENTURNS
