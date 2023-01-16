
//                                               -*- C++ -*-
/**
 * @brief HSICStatImplementation implements the HSIC sensivity index for one marginal.
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
#include "openturns/Pointer.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HSICStatImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
BEGIN_NAMESPACE_OPENTURNS

static const Factory<HSICStatImplementation> Factory_HSICStatImplementation;

CLASSNAMEINIT(HSICStatImplementation)
/* Default constructor */
HSICStatImplementation::HSICStatImplementation()
  : PersistentObject()
{
  // Nothing
};

/* Virtual constructor */
HSICStatImplementation * HSICStatImplementation::clone() const
{
  return new HSICStatImplementation(*this);
}

/* Compute the HSIC index for one marginal*/
Scalar HSICStatImplementation::computeHSICIndex(const CovarianceMatrix&,
    const CovarianceMatrix&,
    const SquareMatrix&) const
{
  throw NotYetImplementedException(HERE) << "You must use a derived class such as HSICUStat or HSICVStat.";
}

/* Is compatible with a Conditional HSIC Estimator ? */
Bool HSICStatImplementation::isCompatibleWithConditionalAnalysis() const
{
  throw NotYetImplementedException(HERE) << "You must use a derived class such as HSICUStat or HSICVStat.";
}

/* Compute the asymptotic p-value */
Scalar HSICStatImplementation::computePValue(const Gamma&,
    const UnsignedInteger,
    const Scalar,
    const Scalar) const
{
  throw NotYetImplementedException(HERE) << "You must use a derived class such as HSICUStat or HSICVStat.";
}

END_NAMESPACE_OPENTURNS
