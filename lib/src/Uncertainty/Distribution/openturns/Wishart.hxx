//                                               -*- C++ -*-
/**
 *  @brief The Wishart distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_WISHART_HXX
#define OPENTURNS_WISHART_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Wishart
 *
 * The Wishart distribution.
 */
class OT_API Wishart
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  Wishart();

  /** Parameters constructor */
  Wishart(const CovarianceMatrix & v,
          const Scalar nu);

  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const Wishart & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Wishart * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get one realization of the distribution as a covariance matrix */
  CovarianceMatrix getRealizationAsMatrix() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  Scalar computePDF(const CovarianceMatrix & m) const;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;
  Scalar computeLogPDF(const CovarianceMatrix & m) const;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to Wishart */


  /** V accessor */
  void setV(const CovarianceMatrix & v);
  CovarianceMatrix getV() const;

  /** Nu accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Update the normalization factor on a log scale */
  void update();

  /** The main parameter set of the distribution */
  TriangularMatrix cholesky_;
  Scalar nu_;

  /** The log-normalization factor */
  Scalar logNormalizationFactor_;

}; /* class Wishart */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WISHART_HXX */
