//                                               -*- C++ -*-
/**
 *  @brief Implementation of BlendedStep
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_BLENDEDSTEP_HXX
#define OPENTURNS_BLENDEDSTEP_HXX

#include "openturns/FiniteDifferenceStepImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS




class OT_API BlendedStep
  : public FiniteDifferenceStepImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  BlendedStep();

  /** Parameters constructor */
  explicit BlendedStep(const Point & epsilon,
                       const Scalar eta = ResourceMap::GetAsScalar( "BlendedStep-DefaultEta" ));

  /** Parameters constructor */
  BlendedStep(const Point & epsilon,
              const Point & eta);

  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  BlendedStep * clone() const override;

  /** Compute step */
  Point operator()(const Point & inP) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Eta accessor */
  void setEta(const Point & eta);
  Point getEta() const;

protected:
  /** Step offset */
  Point eta_;

};

END_NAMESPACE_OPENTURNS




#endif

