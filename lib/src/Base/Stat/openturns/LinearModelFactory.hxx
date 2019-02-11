//                                               -*- C++ -*-
/**
 *  @brief Factory for linear model
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LINEARMODELFACTORY_HXX
#define OPENTURNS_LINEARMODELFACTORY_HXX

#include "openturns/LinearModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelFactory
 * @deprecated
 *
 * Factory for linear model
 */

class OT_API LinearModelFactory
{
public:

  typedef PersistentCollection<Scalar>    ScalarPersistentCollection;

  /** Default constructor */
  LinearModelFactory();

  /** LinearModel creation */
  LinearModel build(const Sample & samplePred,
                    const Sample & sampleLab,
                    const Scalar levelValue = ResourceMap::GetAsScalar( "LinearModelFactory-DefaultLevelValue" )) const;

}; /* class LinearModelFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELFACTORY_HXX */
