//                                               -*- C++ -*-
/**
 *  @brief The class FieldImplementation implements values indexed by
 *  the vertices of a Mesh
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
 *  MERCHANTABILITY or FITNESS FOR A PARTCULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_FIELDIMPLEMENTATION_HXX
#define OPENTURNS_FIELDIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/RegularGrid.hxx"

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

  /** Virtual constructor */
  FieldImplementation * clone() const override;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Mesh accessor */
  Mesh getMesh() const;
  RegularGrid getTimeGrid() const;

  /** Comparison operator */
  using PersistentObject::operator ==;
  Bool operator ==(const FieldImplementation & other) const;

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an FieldImplementation. It is used when streaming
   * the FieldImplementation or for user information.
   */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Size accessor */
  UnsignedInteger getSize() const;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** Return the values stored in the field as a sample */
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

  /** Return the field as a defomed mesh, ie the dimension of the vertices is augmented using zero coordinates at the positions given by the first Indices, the dimension of the values is augmented the same way by adding zero values at the positions given by the second indices, then the vertices are translated by the values */
  Mesh asDeformedMesh(const Indices & addedNullCoordinates = Indices(0),
                      const Indices & addedNullValues = Indices(0)) const;

  /** Compute the input mean of the field */
  Point getInputMean() const;

  /** Compute the temporal mean of the field, ie its input mean when the mesh
      is regular and of dimension 1 */
  Point getOutputMean() const;

  /** l2 norm */
  Scalar norm() const;

  /** Draw a marginal of the timeSerie */
  Graph drawMarginal(const UnsignedInteger index = 0,
                     const Bool interpolate = true) const;

  Graph draw() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** VTK export */
  void exportToVTKFile(const String & fileName) const;

protected:

  /** Compute the input mean of the field */
  void computeInputMean() const;

  /** The mesh associated to the field */
  Mesh mesh_;

  /** The values associated to the field */
  Sample values_;

  /** The description of all components */
  Description description_;

  /** The input mean */
  mutable Point inputMean_;

  /** Flag to tell if the input mean has already been computed */
  mutable Bool isAlreadyComputedInputMean_;

private:
  friend struct FieldInputMeanFunctor;
}; /* class FieldImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDIMPLEMENTATION_HXX */
