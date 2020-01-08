//                                               -*- C++ -*-
/**
 *  @brief ProcessSampleImplementation class
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Point.hxx"
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

  /** Partial copy constructor */
  void add(const Field & field);

  void add(const Sample & sample);

#ifndef SWIG

  /** Operators accessors */
  void setField (const Field & field, const UnsignedInteger i);
  Field getField (const UnsignedInteger i) const;
  Sample & operator[] (const UnsignedInteger i);
  const Sample & operator[] (const UnsignedInteger i) const;

#endif

  /** Virtual constructor */
  virtual ProcessSampleImplementation * clone() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

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

  /**  Method computeQuantilePerComponent() gives the quantile per component of the sample */
  Field computeQuantilePerComponent(const Scalar prob) const;
  ProcessSampleImplementation computeQuantilePerComponent(const Point & prob) const;

  /** Get the i-th marginal sample */
  ProcessSampleImplementation getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  ProcessSampleImplementation getMarginal(const Indices & indices) const;

  /** Draw a marginal of the timeSerie */
  Graph drawMarginal(const UnsignedInteger index = 0,
                     const Bool interpolate = true) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** Mesh on which the ProcessSampleImplementation focuses */
  Mesh mesh_;

  /** Sample collection of all the fields */
  SamplePersistentCollection data_;

}; /* class ProcessSampleImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROCESSSAMPLEIMPLEMENTATION_HXX */
