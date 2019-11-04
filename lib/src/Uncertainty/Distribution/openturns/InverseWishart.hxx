//                                               -*- C++ -*-
/**
 *  @brief The InverseWishart distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INVERSEWISHART_HXX
#define OPENTURNS_INVERSEWISHART_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseWishart
 *
 * The InverseWishart distribution.
 */
class OT_API InverseWishart
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  InverseWishart();

  /** Parameters constructor */
  InverseWishart(const CovarianceMatrix & v,
                 const Scalar nu);

  /** Comparison operator */
  Bool operator ==(const InverseWishart & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual InverseWishart * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get one realization of the distribution as a covariance matrix */
  CovarianceMatrix getRealizationAsMatrix() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const;
  Scalar computePDF(const CovarianceMatrix & m) const;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;
  Scalar computeLogPDF(const CovarianceMatrix & m) const;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to InverseWishart */


  /** V accessor */
  void setV(const CovarianceMatrix & v);
  CovarianceMatrix getV() const;

  /** Nu accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:


private:

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Update the normalization factor on a log scale */
  void update();

  /** The main parameter set of the distribution */
  mutable TriangularMatrix cholesky_;
  Scalar nu_;

  /** The inverse of the Cholesky factor of the inverse of the scale matrix V */
  TriangularMatrix inverseCholeskyInverse_;

  /** The log-normalization factor */
  Scalar logNormalizationFactor_;

}; /* class InverseWishart */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEWISHART_HXX */
