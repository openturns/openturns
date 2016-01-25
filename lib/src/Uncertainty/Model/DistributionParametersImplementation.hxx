//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distribution parameters
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
#ifndef OPENTURNS_DISTRIBUTIONPARAMETERSIMPLEMENTATION_HXX
#define OPENTURNS_DISTRIBUTIONPARAMETERSIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "Distribution.hxx"
#include "Matrix.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DistributionParametersImplementation
 *
 */
class OT_API DistributionParametersImplementation
  : public PersistentObject
{
  CLASSNAME;
public:
  typedef Collection<NumericalPoint>                NumericalPointCollection;
  typedef Collection<NumericalPointWithDescription> NumericalPointWithDescriptionCollection;

  /** Default constructor */
  DistributionParametersImplementation();

  /** Virtual constructor */
  virtual DistributionParametersImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Build a distribution based on a set of native parameters */
  virtual Distribution getDistribution() const;

  /** Compute jacobian / native parameters */
  virtual Matrix gradient() const;

  /** Conversion operator */
  virtual NumericalPoint operator () (const NumericalPoint & inP) const;

  /** Conversion from native parameters */
  virtual NumericalPoint inverse(const NumericalPoint & inP) const;

  /** Parameters value and description accessor */
  virtual void setValues(const NumericalPoint & values);
  virtual NumericalPoint getValues() const;
  virtual Description getDescription() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

}; /* class DistributionParametersImplementation */
END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONPARAMETERSIMPLEMENTATION_HXX */