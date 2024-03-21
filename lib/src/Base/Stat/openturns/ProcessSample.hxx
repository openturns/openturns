//                                               -*- C++ -*-
/**
 *  @brief ProcessSample class
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
#ifndef OPENTURNS_PROCESSSAMPLE_HXX
#define OPENTURNS_PROCESSSAMPLE_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ProcessSampleImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProcessSample
 *
 * An interface for time series
 */
class OT_API ProcessSample
  : public TypedInterfaceObject<ProcessSampleImplementation>
{
  CLASSNAME

public:

  /** Some typedefs to ease reading */
  typedef Collection<Sample>           SampleCollection;
  typedef PersistentCollection<Sample> SamplePersistentCollection;

  /** Default constructor */
  ProcessSample();

  /** Constructors */
  ProcessSample(const UnsignedInteger size,
                const Field & field);

  ProcessSample(const Mesh & mesh,
                const UnsignedInteger size,
                const UnsignedInteger dimension);

  ProcessSample(const Mesh & mesh,
                const SampleCollection & collection);

  /** Copy constructors */
  ProcessSample(const ProcessSampleImplementation & implementation);

  /** Constructor from implementation */
  ProcessSample(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  ProcessSample(ProcessSampleImplementation * p_implementation);
#endif

  /** Partial copy constructor */
  void add(const Field & field);

  void add(const Sample & sample);

  /** Erase elements */
  void erase(const UnsignedInteger index);
  void erase(const UnsignedInteger first, const UnsignedInteger last);

  /** Clear data */
  void clear();

  /** Field accessor */
  void setField (const Field & field, const UnsignedInteger i);
  Field getField (const UnsignedInteger i) const;
#ifndef SWIG

  /** Operators accessors */
  Sample & operator[] (const UnsignedInteger i);
  const Sample & operator[] (const UnsignedInteger i) const;

#endif

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Time grid accessors */
  RegularGrid getTimeGrid() const;

  /** Mesh accessors */
  Mesh getMesh() const;

  /** Size accessor */
  UnsignedInteger getSize() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Mean accessor */
  Field computeMean() const;

  /** Temporal mean accessor */
  Sample computeTemporalMean() const;

  /** Spatial mean accessor */
  Sample computeSpatialMean() const;

  /** Standard deviation accessor */
  Field computeStandardDeviation() const;

  /** Gives the range of the sample (by component) */
  Field computeRange() const;

  /** Gives the median of the sample (by component) */
  Field computeMedian() const;

  /** Gives the variance of the sample (by component) */
  Field computeVariance() const;

  /** Gives the skewness of the sample (by component) */
  Field computeSkewness() const;

  /** Gives the kurtosis of the sample (by component) */
  Field computeKurtosis() const;

  /** Gives the central moment of order k of the sample (by component) */
  Field computeCentralMoment(const UnsignedInteger k) const;

  /** Gives the raw moment of order k of the sample (by component) */
  Field computeRawMoment(const UnsignedInteger k) const;

  /** Get the empirical CDF of the sample */
  Field computeEmpiricalCDF(const Point & point,
                            const Bool tail = false) const;

  /** Maximum accessor */
  Field getMax() const;

  /** Minimum accessor */
  Field getMin() const;

  /** Method computeQuantilePerComponent() gives the quantile per component of the sample */
  Field computeQuantilePerComponent(const Scalar prob) const;
  ProcessSample computeQuantilePerComponent(const Point & prob) const;

  /** Get the i-th marginal sample */
  ProcessSample getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  ProcessSample getMarginal(const Indices & indices) const;

  /** Draw a marginal */
  Graph drawMarginal(const UnsignedInteger index = 0,
                     const Bool interpolate = true) const;

  /** Draw all marginals */
  GridLayout draw(const Bool interpolate = true) const;

  /** Draw correlation between 2 marginals */
  Graph drawMarginalCorrelation(const UnsignedInteger i,
                                const UnsignedInteger j) const;

  /** Draw correlation between all marginals */
  GridLayout drawCorrelation() const;

  /** Translate realizations in-place */
  ProcessSample & operator += (const Sample & translation);
  ProcessSample & operator -= (const Sample & translation);
  ProcessSample & operator += (const Point & translation);
  ProcessSample & operator -= (const Point & translation);
  ProcessSample & operator += (const ProcessSample & translation);
  ProcessSample & operator -= (const ProcessSample & translation);
  
  /** Translate realizations */
  ProcessSample operator + (const Sample & translation) const;
  ProcessSample operator - (const Sample & translation) const;
  ProcessSample operator + (const Point & translation) const;
  ProcessSample operator - (const Point & translation) const;
  ProcessSample operator + (const ProcessSample & translation) const;
  ProcessSample operator - (const ProcessSample & translation) const;

  /** Extract the sample of values at the given vertex index */
  Sample getSampleAtVertex(const UnsignedInteger index) const;

private:

}; /* class ProcessSample */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROCESSSAMPLE_HXX */
