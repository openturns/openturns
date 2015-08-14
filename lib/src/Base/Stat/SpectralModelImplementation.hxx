//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a spectral model using its spectral density function
 *  This is the interface class
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX
#define OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "HermitianMatrix.hxx"
#include "RegularGrid.hxx"
#include "Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SpectralModelImplementation
 */

class OT_API SpectralModelImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  typedef Pointer<SpectralModelImplementation>    Implementation;

  /** Default constructor */
  SpectralModelImplementation();

  /** Virtual copy constructor */
  virtual SpectralModelImplementation * clone() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;
  void setDimension(const UnsignedInteger dimension);

  /** Computation of the spectral density function */
  virtual HermitianMatrix operator() (const NumericalScalar frequency) const;

  /** Frequency grid accessors */
  virtual RegularGrid getFrequencyGrid() const;
  virtual void setFrequencyGrid(const RegularGrid & frequencyGrid);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Drawing method */
  Graph draw(const UnsignedInteger rowIndex = 0,
             const UnsignedInteger columnIndex = 0,
             const Bool module = true) const;

protected:

  /** dimension parameter */
  UnsignedInteger dimension_;

  /** Frequency grid */
  RegularGrid frequencyGrid_;

} ; /* class SpectralModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX */
