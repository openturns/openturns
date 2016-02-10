//                                               -*- C++ -*-
/**
 *  @brief Implementation of BlendedStep
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

#ifndef OPENTURNS_BLENDEDSTEP_HXX
#define OPENTURNS_BLENDEDSTEP_HXX

#include "FiniteDifferenceStepImplementation.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS




class OT_API BlendedStep
  : public FiniteDifferenceStepImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  BlendedStep();

  /** Parameters constructor */
  explicit BlendedStep(const NumericalPoint & epsilon,
                       const NumericalScalar eta = ResourceMap::GetAsNumericalScalar( "BlendedStep-DefaultEta" ));

  /** Parameters constructor */
  BlendedStep(const NumericalPoint & epsilon,
              const NumericalPoint & eta);

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual BlendedStep * clone() const;

  /** Compute step */
  virtual NumericalPoint operator()(const NumericalPoint & inP) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Eta accessor */
  void setEta(const NumericalPoint & eta);
  NumericalPoint getEta() const;

protected:
  /** Step offset */
  NumericalPoint eta_;

};

END_NAMESPACE_OPENTURNS




#endif

