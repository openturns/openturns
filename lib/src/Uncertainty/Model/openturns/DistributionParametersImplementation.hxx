//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distribution parameters
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
#ifndef OPENTURNS_DISTRIBUTIONPARAMETERSIMPLEMENTATION_HXX
#define OPENTURNS_DISTRIBUTIONPARAMETERSIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Matrix.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DistributionParametersImplementation
 *
 */
class OT_API DistributionParametersImplementation
  : public PersistentObject
{
  CLASSNAME
public:
  typedef Collection<Point>                PointCollection;
  typedef Collection<PointWithDescription> PointWithDescriptionCollection;

  /** Default constructor */
  DistributionParametersImplementation();

  /** Virtual constructor */
  DistributionParametersImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Build a distribution based on a set of native parameters */
  virtual Distribution getDistribution() const;

  /** Evaluate native parameters */
  virtual Point evaluate() const;

  /** Compute jacobian / native parameters */
  virtual Matrix gradient() const;

  /** Conversion operator */
  virtual Point operator () (const Point & inP) const;

  /** Conversion from native parameters */
  virtual Point inverse(const Point & inP) const;

  /** Parameters value and description accessor */
  virtual void setValues(const Point & values);
  virtual Point getValues() const;
  virtual Description getDescription() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

}; /* class DistributionParametersImplementation */
END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONPARAMETERSIMPLEMENTATION_HXX */

