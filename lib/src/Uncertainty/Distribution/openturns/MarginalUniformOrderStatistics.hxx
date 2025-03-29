//                                               -*- C++ -*-
/**
 *  @brief The MarginalUniformOrderStatistics distribution
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
#ifndef OPENTURNS_MARGINALUNIFORMORDERSTATISTICS_HXX
#define OPENTURNS_MARGINALUNIFORMORDERSTATISTICS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MarginalDistribution.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MarginalUniformOrderStatistics
 *
 * The MarginalUniformOrderStatistics distribution.
 */
class OT_API MarginalUniformOrderStatistics
  : public MarginalDistribution
{
  CLASSNAME

public:

  /** Default constructor */
  MarginalUniformOrderStatistics();

  /** Parameters constructor */
  MarginalUniformOrderStatistics(const UnsignedInteger n,
                                 const Indices & indices);

  /** N accessor */
  UnsignedInteger getN() const;

  /** Comparison operator */
  using MarginalDistribution::operator ==;
  Bool operator ==(const MarginalUniformOrderStatistics & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MarginalUniformOrderStatistics * clone() const override;

  /** Compute the PDF of the distribution */
  using MarginalDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  using MarginalDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Compute the PDF of the distribution */
  using MarginalDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using MarginalDistribution::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Log-normalization factor for the PDF */
  Scalar logNormalizationFactor_ = 0.0;

  /** Actual domain of the distribution */
  Mesh simplex_;

}; /* class MarginalUniformOrderStatistics */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALUNIFORMORDERSTATISTICS_HXX */
