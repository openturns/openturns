//                                               -*- C++ -*-
/**
 *  @brief The Histogram distribution
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
#ifndef OPENTURNS_HISTOGRAM_HXX
#define OPENTURNS_HISTOGRAM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Histogram
 *
 * The Histogram distribution.
 */
class OT_API Histogram
  : public ContinuousDistribution
{
  CLASSNAME

public:

  /** Default constructor */
  Histogram();

  /** Parameters constructor */
  Histogram(const Scalar first,
            const Point & width,
            const Point & height);

  /** Parameters constructor */
  Histogram(const Point & ticks,
            const Point & frequencies);

  /** Comparison operator */
  Bool operator ==(const Histogram & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Histogram * clone() const override;

  /** Get one realization of the Histogram distribution */
  Point getRealization() const override;

  /** Get the DDF of the Histogram distribution */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the Histogram distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the Histogram distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the PDFGradient of the Histogram distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the Histogram distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Parameters value and description accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;
  Description getParameterDescription() const override;

  /* Interface specific to Histogram */

  /** First point accessor */
  void setFirst(const Scalar first);
  Scalar getFirst() const;

  /** Data accessor */
  void setData(const Point & width,
               const Point & weight);
  Point getWidth() const;
  Point getHeight() const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const override;

  /** Draw the PDF of the Histogram using a specific presentation */
  //        using ContinuousDistribution::drawPDF;
  Graph drawPDF(const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                const Bool logScale = false) const override;

  /** Draw the PDF of the Histogram using a specific presentation */
  Graph drawPDF(const Scalar xMin,
                const Scalar xMax,
                const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"),
                const Bool logScale = false) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Get the quantile of the Histogram distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** The first point of the collection */
  Scalar first_;

  /** The collection of widths/heights */
  Point width_;
  Point height_;

  /** The cumulated bin widths */
  Point cumulatedWidth_;

  /** The cumulated bin surface */
  Point cumulatedSurface_;

}; /* class Histogram */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HISTOGRAM_HXX */
