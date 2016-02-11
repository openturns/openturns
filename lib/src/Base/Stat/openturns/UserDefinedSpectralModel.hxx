//                                               -*- C++ -*-
/**
 *  @brief This class build a spectral model using a frequency grid and a spectral function
 *  given as a collection of Hermitian Matrix
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
#ifndef OPENTURNS_USERDEFINEDSPECTRALMODEL_HXX
#define OPENTURNS_USERDEFINEDSPECTRALMODEL_HXX

#include "openturns/SpectralModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/RegularGrid.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedSpectralModel
 */

class OT_API UserDefinedSpectralModel
  : public SpectralModelImplementation
{

  CLASSNAME;

public:

  typedef PersistentCollection<HermitianMatrix>           HermitianMatrixPersistentCollection;
  typedef Collection<HermitianMatrix>                     HermitianMatrixCollection;

  /** Default onstructor */
  UserDefinedSpectralModel();

  /** Standard onstructor */
  UserDefinedSpectralModel(const RegularGrid & frequencyGrid,
                           const HermitianMatrixCollection & spectralFunction);

  /** Virtual copy constructor */
  virtual UserDefinedSpectralModel * clone() const;

  /** Computation of the spectral density function */
  HermitianMatrix operator()(const NumericalScalar frequency) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** collection of spectral functions */
  HermitianMatrixPersistentCollection DSPCollection_;

} ; /* class UserDefinedSpectralModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDSPECTRALMODEL_HXX */
