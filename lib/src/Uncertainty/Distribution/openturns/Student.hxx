//                                               -*- C++ -*-
/**
 *  @brief The Student distribution
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
#ifndef OPENTURNS_STUDENT_HXX
#define OPENTURNS_STUDENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/EllipticalDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Student
 *
 * The Student distribution.
 */
class OT_API Student
  : public EllipticalDistribution
{

  CLASSNAME
public:

  /** Default constructor */
  explicit Student(const Scalar nu = 3.0,
                   const UnsignedInteger dimension = 1);

  /** Dimension 1 constructor */
  Student(const Scalar nu,
          const Scalar mu,
          const Scalar sigma);

  /** Constructor for multiD Student distribution */
  Student(const Scalar nu,
          const Point & mu,
          const Point & sigma);

  Student(const Scalar nu,
          const Point & mu,
          const Point & sigma,
          const CorrelationMatrix & R);


  /** Comparison operator */
  Bool operator ==(const Student & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Student * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the CDF of the distribution */
  using EllipticalDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  Sample computeCDF(const Sample & sample) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the PDFGradient of the distribution */
  using EllipticalDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  using EllipticalDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Compute the radial distribution CDF */
  Scalar computeRadialDistributionCDF(const Scalar radius, const Bool tail = false) const override;

  using EllipticalDistribution::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using EllipticalDistribution::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using EllipticalDistribution::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the mean of the distribution */
  Point getMean() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the covariance of the distribution */
  CovarianceMatrix getCovariance() const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using EllipticalDistribution::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /* Interface specific to Student */

  /** Nu accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

  /** Mu accessor */
  void setMu(const Scalar mu);
  Scalar getMu() const;

  /** Compute the density generator of the ellipticalal generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R^(-1)(x-mu))                      */
  Scalar computeDensityGenerator(const Scalar betaSquare) const override;
  Scalar computeLogDensityGenerator(const Scalar betaSquare) const override;

  /** Compute the derivative of the density generator */
  Scalar computeDensityGeneratorDerivative(const Scalar betaSquare) const override;

  /** Compute the seconde derivative of the density generator */
  Scalar computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** The nu of the Student distribution */
  Scalar nu_;

  /** Specific normalization factor for the Student distribution */
  Scalar studentNormalizationFactor_;
}; /* class Student */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDENT_HXX */
