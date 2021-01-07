//                                               -*- C++ -*-
/**
 *  @brief ProcessSampleImplementation class
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PROCESSSAMPLEIMPLEMENTATION_HXX
#define OPENTURNS_PROCESSSAMPLEIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/GridLayout.hxx"
#include "openturns/Field.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProcessSampleImplementation
 *
 * An interface for time series
 */
class OT_API ProcessSampleImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Some typedefs to ease reading */
  typedef Collection<Sample>           SampleCollection;
  typedef PersistentCollection<Sample> SamplePersistentCollection;

  /** Default constructor */
  ProcessSampleImplementation();

  /** Constructors */
  ProcessSampleImplementation(const UnsignedInteger size,
                              const Field & field);

  ProcessSampleImplementation(const Mesh & mesh,
                              const UnsignedInteger size,
                              const UnsignedInteger dimension);

  ProcessSampleImplementation(const Mesh & mesh,
                              const SampleCollection & collection);

  /** Partial copy constructor */
  void add(const Field & field);

  void add(const Sample & sample);

  /** Field accessor */
  void setField (const Field & field, const UnsignedInteger i);
  Field getField (const UnsignedInteger i) const;

#ifndef SWIG

  /** Operators accessors */
  Sample & operator[] (const UnsignedInteger i);
  const Sample & operator[] (const UnsignedInteger i) const;

#endif

  /** Virtual constructor */
  ProcessSampleImplementation * clone() const override;

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

  /**  Method computeQuantilePerComponent() gives the quantile per component of the sample */
  Field computeQuantilePerComponent(const Scalar prob) const;
  ProcessSampleImplementation computeQuantilePerComponent(const Point & prob) const;

  /** Get the i-th marginal sample */
  ProcessSampleImplementation getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  ProcessSampleImplementation getMarginal(const Indices & indices) const;

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Translate realizations in-place */
  ProcessSampleImplementation & operator += (const Sample & translation);
  ProcessSampleImplementation & operator -= (const Sample & translation);

private:

  /** Mesh on which the ProcessSampleImplementation focuses */
  Mesh mesh_;

  /** Sample collection of all the fields */
  SamplePersistentCollection data_;

}; /* class ProcessSampleImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROCESSSAMPLEIMPLEMENTATION_HXX */
