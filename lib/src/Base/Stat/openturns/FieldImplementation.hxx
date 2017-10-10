//                                               -*- C++ -*-
/**
 *  @brief The class FieldImplementation implements values indexed by
 *  the vertices of a Mesh
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldImplementation
 */

class OT_API FieldImplementation
  : public PersistentObject
{
  CLASSNAME
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
                      const Sample & values);

  /** Accessor to values */
  Point getValueAtIndex(const UnsignedInteger index) const;
  void setValueAtIndex(const UnsignedInteger index,
                       const Point & val);

  Point getValueAtNearestPosition(const Point & position) const;
  void setValueAtNearestPosition(const Point & position,
                                 const Point & val);

  /** Accessor to values */
  Point getValueAtNearestTime(const Scalar timestamp) const;
  void setValueAtNearestTime(const Scalar timestamp,
                             const Point & val);

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

  /** Return the values stored in the field as a sample 
   @deprecated */
  Sample getSample() const;
  Sample getValues() const;
  void setValues(const Sample & values);

  /** Get the i-th marginal sample */
  FieldImplementation getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  FieldImplementation getMarginal(const Indices & indices) const;

#ifndef SWIG
  /** Individual value accessor */
  NSI_point operator[](const UnsignedInteger index);
  NSI_const_point operator[](const UnsignedInteger index) const;
  NSI_point at(const UnsignedInteger index);
  NSI_const_point at(const UnsignedInteger index) const;
  Scalar & operator() (const UnsignedInteger i,
                       const UnsignedInteger j);
  const Scalar & operator() (const UnsignedInteger i,
                             const UnsignedInteger j) const;
  Scalar & at(const UnsignedInteger i,
              const UnsignedInteger j);
  const Scalar & at(const UnsignedInteger i,
                    const UnsignedInteger j) const;
#endif

  /** Return the field as a sample, ie its values and positions */
  Sample asSample() const;

  /** Return the field as a defomed mesh, ie its values are added to the components of the vertices if the dimensions match */
  Mesh asDeformedMesh() const;

  /** Compute the spatial mean of the field */
  Point getSpatialMean() const;

  /** Compute the temporal mean of the field, ie its spatial mean when the mesh
      is regular and of dimension 1 */
  Point getTemporalMean() const;

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
  Sample values_;

  /** The description of all components */
  Description description_;

  /** The spatial mean */
  mutable Point spatialMean_;

  /** Flag to tell if the spatial mean has already been computed */
  mutable Bool isAlreadyComputedSpatialMean_;

private:
  friend struct FieldSpatialMeanFunctor;
}; /* class FieldImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDIMPLEMENTATION_HXX */
