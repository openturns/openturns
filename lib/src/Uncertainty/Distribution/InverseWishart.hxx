//                                               -*- C++ -*-
/**
 *  @brief The InverseWishart distribution
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
#ifndef OPENTURNS_INVERSEWISHART_HXX
#define OPENTURNS_INVERSEWISHART_HXX

#include "OTprivate.hxx"
#include "ContinuousDistribution.hxx"
#include "CovarianceMatrix.hxx"
#include "TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseWishart
 *
 * The InverseWishart distribution.
 */
class OT_API InverseWishart
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  InverseWishart();

  /** Parameters constructor */
  InverseWishart(const CovarianceMatrix & v,
                 const NumericalScalar nu);

  /** Comparison operator */
  Bool operator ==(const InverseWishart & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual InverseWishart * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get one realization of the distribution as a covariance matrix */
  CovarianceMatrix getRealizationAsMatrix() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;
  NumericalScalar computePDF(const CovarianceMatrix & m) const;
  using ContinuousDistribution::computeLogPDF;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;
  NumericalScalar computeLogPDF(const CovarianceMatrix & m) const;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);


  /* Interface specific to InverseWishart */


  /** V accessor */
  void setV(const CovarianceMatrix & v);
  CovarianceMatrix getV() const;

  /** Nu accessor */
  void setNu(const NumericalScalar nu);
  NumericalScalar getNu() const;

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
  NumericalScalar nu_;

  /** The log-normalization factor */
  NumericalScalar logNormalizationFactor_;

}; /* class InverseWishart */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEWISHART_HXX */
