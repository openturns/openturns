//                                               -*- C++ -*-
/**
 *  @brief The Histogram distribution
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
#ifndef OPENTURNS_HISTOGRAM_HXX
#define OPENTURNS_HISTOGRAM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/HistogramPair.hxx"
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
  CLASSNAME;

public:

  typedef Collection<HistogramPair>                HistogramPairCollection;
  typedef PersistentCollection<HistogramPair>      HistogramPairPersistentCollection;

  /** Default constructor */
  Histogram();

  /** @deprecated Parameters constructor */
  Histogram(const NumericalScalar first,
            const HistogramPairCollection & collection);

  Histogram(const NumericalScalar first,
            const NumericalPoint & width,
            const NumericalPoint & height);

  /** Comparison operator */
  Bool operator ==(const Histogram & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Histogram * clone() const;

  /** Get one realization of the Histogram distribution */
  NumericalPoint getRealization() const;

  /** Get the DDF of the Histogram distribution */
  using ContinuousDistribution::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the Histogram distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the Histogram distribution */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;

  /** Get the PDFGradient of the Histogram distribution */
  using ContinuousDistribution::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDFGradient of the Histogram distribution */
  using ContinuousDistribution::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Implementation getStandardRepresentative() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /* Interface specific to Histogram */

  /** First point accessor */
  void setFirst(const NumericalScalar first);
  NumericalScalar getFirst() const;

  /** Data accessor */
  void setData(const NumericalPoint & width,
               const NumericalPoint & weight);
  NumericalPoint getWidth() const;
  NumericalPoint getHeight() const;

  /** Collection accessor */
  void setPairCollection(const HistogramPairCollection & collection);
  HistogramPairCollection getPairCollection() const;

  /** Get the PDF singularities inside of the range - 1D only */
  NumericalPoint getSingularities() const;

  /** Draw the PDF of the Histogram using a specific presentation */
  //        using ContinuousDistribution::drawPDF;
  virtual Graph drawPDF() const;

  /** Draw the PDF of the Histogram using a specific presentation */
  virtual Graph drawPDF(const NumericalScalar xMin,
                        const NumericalScalar xMax,
                        const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")) const;

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

  /** Get the quantile of the Histogram distribution */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The first point of the collection */
  NumericalScalar first_;

  /** The collection of widths/heights */
  NumericalPoint width_;
  NumericalPoint height_;

  /** The cumulated bin widths */
  NumericalPoint cumulatedWidth_;

  /** The cumulated bin surface */
  NumericalPoint cumulatedSurface_;

}; /* class Histogram */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HISTOGRAM_HXX */
