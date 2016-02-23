//                                               -*- C++ -*-
/**
 *  @brief The class NumericalSample implements blank free samples
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
#ifndef OPENTURNS_NUMERICALSAMPLE_HXX
#define OPENTURNS_NUMERICALSAMPLE_HXX

#include <iostream>              // for std::ostream
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Description.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/NumericalSampleImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NumericalSample
 */

class OT_API NumericalSample
  : public TypedInterfaceObject<NumericalSampleImplementation>
{
  CLASSNAME;

public:

  /* Some typedefs for easy reading */

public:

  /** Factory of NumericalSample from CSV file */
  static NumericalSample ImportFromCSVFile(const FileName & fileName,
      const String & csvSeparator = ResourceMap::Get( "csv-file-separator" ));

  /** Factory of NumericalSample from Text file */
  static NumericalSample ImportFromTextFile(const FileName & fileName,
      const String & separator = " ");

  /** Export NumericalSample into CSV file */
  void exportToCSVFile(const FileName & fileName,
                       const String & csvSeparator = ResourceMap::Get( "csv-file-separator" )) const;

  /** Export a sample as a matrix, one row by realization, in a format suitable to exchange with R. */
  String streamToRFormat() const;


public:

  /**
   * Default constructor
   * Build a NumericalSample of 1 dimension and with size equal to 0
   */
  NumericalSample();

  /** Constructor with size and dimension */
  NumericalSample(const UnsignedInteger size,
                  const UnsignedInteger dim);

  /** Constructor from a NumericalPoint (all elements are equal to the NumericalPoint) */
  NumericalSample(const UnsignedInteger size,
                  const NumericalPoint & point);

#ifndef SWIG
  /** Constructor from a collection of NumericalPoint */
  NumericalSample(const Collection<NumericalPoint> & coll);

  /** Constructor from a collection of Indices */
  NumericalSample(const Collection<Indices> & coll);
#endif

  /** Constructor from implementation */
  NumericalSample(const NumericalSampleImplementation & implementation);

private:

  /** Constructor from implementation */
  NumericalSample(const Implementation & implementation);

public:

  /** Comparison operator */
  Bool operator ==(const NumericalSample & other) const;

  /** Comparison operator */
  Bool operator !=(const NumericalSample & other) const;

#ifndef SWIG
  NSI_point operator [] (const UnsignedInteger index);
  NSI_const_point operator [] (const UnsignedInteger index) const;
  NSI_point at (const UnsignedInteger index);
  NSI_const_point at (const UnsignedInteger index) const;
  NumericalScalar & operator () (const UnsignedInteger i,
                                 const UnsignedInteger j);
  const NumericalScalar & operator () (const UnsignedInteger i,
                                       const UnsignedInteger j) const;
  NumericalScalar & at (const UnsignedInteger i,
                        const UnsignedInteger j);
  const NumericalScalar & at (const UnsignedInteger i,
                              const UnsignedInteger j) const;

  void erase(NumericalSampleImplementation::iterator first, NumericalSampleImplementation::iterator last);
#endif
  // These functions are only intended to be used by SWIG, DO NOT use them for your own purpose !
  // INTENTIONALY NOT DOCUMENTED
  const NumericalScalar * __baseaddress__ () const;
  UnsignedInteger __elementsize__ () const;

  /** Whether the list contains the value val */
  Bool contains(const NumericalPoint & val) const;

  void erase(const UnsignedInteger first,
             const UnsignedInteger last);
  void erase(const UnsignedInteger index);

  /** erase the whole sample */
  void clear();

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an NumericalSample. It is used when streaming
   * the NumericalSample or for user information.
   */
  String __repr__() const;

  String __str__(const String & offset = "") const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Size accessor */
  UnsignedInteger getSize() const;

  /** Maximum accessor */
  NumericalPoint getMax() const;

  /** Minimum accessor */
  NumericalPoint getMin() const;

  /** Method add() appends an element to the collection */
  void add(const NumericalPoint & point);

  /** Method add() appends another sample to the collection */
  void add(const NumericalSample & sample);

  /** Stack the given sample to the current one */
  void stack(const NumericalSample & sample);

  /**
   * Method split() trunk the sample before the index passed as argument
   * and returns the remainder as new sample. This method tries its best not for doubling
   * memory usage.
   */
  NumericalSample split(const UnsignedInteger index);

  /**
   * Method computeMean() gives the mean of the sample, based on the formula
   * mean = sum of the elements in the sample / size of the sample
   */
  NumericalPoint computeMean() const;

  /**
   * Method computeCovariance() gives the covariance of the sample
   */
  CovarianceMatrix computeCovariance() const;

  /**
   * Method computeStandardDeviation() gives the standard deviation of the sample
   */
  TriangularMatrix computeStandardDeviation() const;

  /**
   * Method computeVariance() gives the variance of the sample (by component)
   */
  NumericalPoint computeVariance() const;

  /**
   * Method computeStandardDeviationPerComponent() gives the standard deviation of each component of the sample
   */
  NumericalPoint computeStandardDeviationPerComponent() const;

  /**
   * Method computePearsonCorrelation() gives the Pearson correlation matrix of the sample
   */
  CorrelationMatrix computeLinearCorrelation() const;
  CorrelationMatrix computePearsonCorrelation() const;

  /**
   * Method computeSpearmanCorrelation() gives the Spearman correlation matrix of the sample
   */
  CorrelationMatrix computeSpearmanCorrelation() const;

  /**
   * Gives the Kendall correlation matrix of the sample
   */
  CorrelationMatrix computeKendallTau() const;

  /**
   * Method computeRange gives the range of the sample (by component)
   */
  NumericalPoint computeRange() const;

  /**
   * Method computeMedian() gives the median of the sample (by component)
   */
  NumericalPoint computeMedian() const;

  /**
   * Method computeSkewness() gives the skewness of the sample (by component)
   */
  NumericalPoint computeSkewness() const;

  /**
   * Method computeKurtosis() gives the kurtosis of the sample (by component)
   */
  NumericalPoint computeKurtosis() const;

  /**
   * Gives the centered moment of order k of the sample (by component)
   */
  NumericalPoint computeCenteredMoment(const UnsignedInteger k) const;

  /**
   * Gives the raw moment of order k of the sample (by component)
   */
  NumericalPoint computeRawMoment(const UnsignedInteger k) const;

  /**
   * Method computeQuantilePerComponent() gives the quantile per component of the sample
   */
  NumericalPoint computeQuantilePerComponent(const NumericalScalar prob) const;

  /**
   * Method computeQuantile() gives the N-dimension quantile of the sample
   */
  NumericalPoint computeQuantile(const NumericalScalar prob) const;

  /**
   * Get the empirical CDF of the sample
   */
  NumericalScalar computeEmpiricalCDF(const NumericalPoint & point,
                                      const Bool tail = false) const;

  /**
   * Get the position of a point in the sample.
   * Returns size+1 if the point does not belong to the sample.
   */
  UnsignedInteger find(const NumericalPoint & point) const;

  /**
   * Translate realizations in-place
   */
  NumericalSample & operator += (const NumericalScalar translation);
  NumericalSample & operator += (const NumericalPoint & translation);
  NumericalSample & operator += (const NumericalSample & translation);
  NumericalSample & operator -= (const NumericalScalar translation);
  NumericalSample & operator -= (const NumericalPoint & translation);
  NumericalSample & operator -= (const NumericalSample & translation);

  /** Translate/scale realizations */
  NumericalSample operator + (const NumericalScalar translation) const;
  NumericalSample operator + (const NumericalPoint & translation) const;
  NumericalSample operator + (const NumericalSample & translation) const;
  NumericalSample operator - (const NumericalScalar translation) const;
  NumericalSample operator - (const NumericalPoint & translation) const;
  NumericalSample operator - (const NumericalSample & translation) const;
  NumericalSample operator * (const NumericalScalar scaling) const;
  NumericalSample operator * (const NumericalPoint & scaling) const;
  NumericalSample operator * (const SquareMatrix & scaling) const;
  NumericalSample operator / (const NumericalScalar scaling) const;
  NumericalSample operator / (const NumericalPoint & scaling) const;
  NumericalSample operator / (const SquareMatrix & scaling) const;

  /**
   * Scale realizations componentwise in-place
   */
  NumericalSample & operator *= (const NumericalScalar scaling);
  NumericalSample & operator *= (const NumericalPoint & scaling);
  NumericalSample & operator *= (const SquareMatrix & scaling);
  NumericalSample & operator /= (const NumericalScalar scaling);
  NumericalSample & operator /= (const NumericalPoint & scaling);
  NumericalSample & operator /= (const SquareMatrix & scaling);

  /** Ranked sample */
  NumericalSample rank() const;

  /** Ranked component */
  NumericalSample rank(const UnsignedInteger index) const;

  /** Sorted sample */
  NumericalSample sort() const;

  /** Sorted component */
  NumericalSample sort(const UnsignedInteger index) const;

  /** Sorted according a component */
  NumericalSample sortAccordingToAComponent(const UnsignedInteger index) const;

  /** Store a sample in a temporary text file, one realization by line. Returns the file name. */
  virtual String storeToTemporaryFile() const;

  /** Get the i-th marginal sample */
  NumericalSample getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  NumericalSample getMarginal(const Indices & indices) const;

  /** Returns a pointer to the underlying implementation object */
  virtual ImplementationAsPersistentObject getImplementationAsPersistentObject() const;

  /** Sets the pointer to the underlying implementation object */
  virtual void setImplementationAsPersistentObject(const ImplementationAsPersistentObject & obj);

}; /* class NumericalSample */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NUMERICALSAMPLE_HXX */
