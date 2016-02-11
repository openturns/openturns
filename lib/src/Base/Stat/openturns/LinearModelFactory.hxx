//                                               -*- C++ -*-
/**
 *  @brief Factory for linear model
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
#ifndef OPENTURNS_LINEARMODELFACTORY_HXX
#define OPENTURNS_LINEARMODELFACTORY_HXX

#include "LinearModel.hxx"
#include "NumericalSample.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelFactory
 *
 * Factory for linear model
 */

class OT_API LinearModelFactory
{
public:

  typedef PersistentCollection<NumericalScalar>    NumericalScalarPersistentCollection;

  /** Default constructor */
  LinearModelFactory();

  /** LinearModel creation */
  LinearModel build(const NumericalSample & samplePred,
                    const NumericalSample & sampleLab,
                    const NumericalScalar levelValue = ResourceMap::GetAsNumericalScalar( "LinearModelFactory-DefaultLevelValue" )) const;

}; /* class LinearModelFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELFACTORY_HXX */
