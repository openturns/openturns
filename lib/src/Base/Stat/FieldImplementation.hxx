//                                               -*- C++ -*-
/**
 *  @brief The class FieldImplementation implements values indexed by
 *  the vertices of a Mesh
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
 *  MERCHANTABILITY or FITNESS FOR A PARTCULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_FIELDIMPLEMENTATION_HXX
#define OPENTURNS_FIELDIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "NumericalSample.hxx"
#include "Mesh.hxx"
#include "RegularGrid.hxx"
#include "TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldImplementation
 */

class OT_API FieldImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  /**
   * Default constructor
   */
  FieldImplementation();

  /** Standard constructor */
  FieldImplementation(const Mesh & mesh,
                      const UnsignedInteger dim);

  /** Constructor from a Mesh and a sample */
  FieldImplementation(const Mesh & mesh,
                      const NumericalSample & values);

  /** Accessor to values */
  NumericalPoint getValueAtIndex(const UnsignedInteger index) const;
  void setValueAtIndex(const UnsignedInteger index,
                       const NumericalPoint & val);

  NumericalPoint getValueAtNearestPosition(const NumericalPoint & position) const;
  void setValueAtNearestPosition(const NumericalPoint & position,
                                 const NumericalPoint & val);

  /** Accessor to values */
  NumericalPoint getValueAtNearestTime(const NumericalScalar timestamp) const;
  void setValueAtNearestTime(const NumericalScalar timestamp,
                             const NumericalPoint & val);

  /** Virtual constructor */
  virtual FieldImplementation * clone() const;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Mesh accessor */
  Mesh getMesh() const;
  RegularGrid getTimeGrid() const;

  /** Comparison operator */
  Bool operator ==(const FieldImplementation & other) const;

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an FieldImplementation. It is used when streaming
   * the FieldImplementation or for user information.
   */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Size accessor */
  UnsignedInteger getSize() const;

  /** Dimension accessor */
  UnsignedInteger getSpatialDimension() const;
  UnsignedInteger getDimension() const;

  /** Return the values stored in the field as a sample */
  NumericalSample getSample() const;
  NumericalSample getValues() const;
  void setValues(const NumericalSample & values);

#ifndef SWIG
  /** Individual value accessor */
  NSI_point operator[](const UnsignedInteger index);
  NSI_const_point operator[](const UnsignedInteger index) const;
  NSI_point at(const UnsignedInteger index);
  NSI_const_point at(const UnsignedInteger index) const;
  NumericalScalar & operator() (const UnsignedInteger i,
                                const UnsignedInteger j);
  const NumericalScalar & operator() (const UnsignedInteger i,
                                      const UnsignedInteger j) const;
  NumericalScalar & at(const UnsignedInteger i,
                       const UnsignedInteger j);
  const NumericalScalar & at(const UnsignedInteger i,
                             const UnsignedInteger j) const;
#endif

  /* Method __contains__() is for Python */
  Bool __contains__(const NumericalPoint & val) const;

  /** Return the field as a sample, ie its values and positions */
  NumericalSample asSample() const;

  /** Return the field as a defomed mesh, ie its values are added to the components of the vertices if the dimensions match */
  Mesh asDeformedMesh() const;

  /** Compute the spatial mean of the field */
  NumericalPoint getSpatialMean() const;

  /** Compute the temporal mean of the field, ie its spatial mean when the mesh
      is regular and of dimension 1 */
  NumericalPoint getTemporalMean() const;

  /** Draw a marginal of the timeSerie */
  Graph drawMarginal(const UnsignedInteger index = 0,
                     const Bool interpolate = true) const;

  Graph draw() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** VTK export */
  void exportToVTKFile(const String & fileName) const;

protected:

  /** Compute the spatial mean of the field */
  void computeSpatialMean() const;

  /** The mesh associated to the field */
  Mesh mesh_;

  /** The values associated to the field */
  NumericalSample values_;

  /** The description of all components */
  Description description_;

  /** The spatial mean */
  mutable NumericalPoint spatialMean_;

  /** Flag to tell if the spatial mean has already been computed */
  mutable Bool isAlreadyComputedSpatialMean_;

private:
  friend struct FieldSpatialMeanFunctor;
}; /* class FieldImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDIMPLEMENTATION_HXX */
