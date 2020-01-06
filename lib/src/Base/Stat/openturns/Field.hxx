//                                               -*- C++ -*-
/**
 *  @brief The class Field implements samples indexed by time
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
#ifndef OPENTURNS_FIELD_HXX
#define OPENTURNS_FIELD_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/FieldImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Field
 */

class OT_API Field
  : public TypedInterfaceObject<FieldImplementation>
{
  CLASSNAME

public:

  /**
   * Default constructor
   */
  Field();

  /** Constructor from a Mesh and a dimension */
  Field(const Mesh & mesh,
        const UnsignedInteger dim);

  /** Constructor from a Mesh and a sample */
  Field(const Mesh & mesh,
        const Sample & values);

  /** Constructor from implementation */
  Field(const FieldImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  Field(const Implementation & implementation);

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

  /** Accessor to values */
  Point getValueAtIndex(const UnsignedInteger index) const;
  void setValueAtIndex(const UnsignedInteger index,
                       const Point & val);

  /** Get the i-th marginal sample */
  Field getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  Field getMarginal(const Indices & indices) const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Mesh accessor */
  Mesh getMesh() const;
  RegularGrid getTimeGrid() const;

  /** Comparison operator */
  Bool operator ==(const Field & other) const;

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an Field. It is used when streaming
   * the Field or for user information.
   */
  String __repr__() const;

  String __str__(const String & offset = "") const;

  /** Size accessor */
  UnsignedInteger getSize() const;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** Return the values stored in the field as a sample */
  Sample getValues() const;
  void setValues(const Sample & values);

  /** Return the field as a defomed mesh, ie the dimension of the vertices is augmented using zero coordinates at the positions given by the first Indices, the dimension of the values is augmented the same way by adding zero values at the positions given by the second indices, then the vertices are translated by the values */
  Mesh asDeformedMesh(const Indices & addedNullCoordinates = Indices(0),
                      const Indices & addedNullValues = Indices(0)) const;

  /** Compute the spatial mean of the field */
  Point getInputMean() const;

  /** Draw a marginal of the field */
  Graph drawMarginal(const UnsignedInteger index = 0,
                     const Bool interpolate = true) const;

  Graph draw() const;

  /** VTK export */
  void exportToVTKFile(const String & fileName) const;

}; /* class Field */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELD_HXX */
