//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the Student T
 *         CDF and quantile
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_STUDENTFUNCTIONS_HXX
#define OPENTURNS_STUDENTFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace StudentFunctions
{
NumericalScalar StudentCDF(const NumericalScalar nu,
                           const NumericalScalar x,
                           const Bool tail = false);

NumericalScalar StudentQuantile(const NumericalScalar nu,
                                const NumericalScalar p,
                                const Bool tail = false);

NumericalScalar StudentRealization(const NumericalScalar nu);

NumericalScalar NonCentralStudentCDF(const NumericalScalar nu,
                                     const NumericalScalar delta,
                                     const NumericalScalar x,
                                     const Bool tail = false);

NumericalScalar NonCentralStudentCDFAlt0(const NumericalScalar nu,
    const NumericalScalar delta,
    const NumericalScalar x,
    const Bool tail = false,
    const NumericalScalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
    const UnsignedInteger maximumIteration = ResourceMap::GetAsScalar("DistFunc-MaximumIteration"));

NumericalScalar NonCentralStudentCDFAlt1(const NumericalScalar nu,
    const NumericalScalar delta,
    const NumericalScalar x,
    const Bool tail = false,
    const NumericalScalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"));

NumericalScalar NonCentralStudentPDF(const NumericalScalar nu,
                                     const NumericalScalar delta,
                                     const NumericalScalar x);

NumericalScalar NonCentralStudentPDFAlt0(const NumericalScalar nu,
    const NumericalScalar delta,
    const NumericalScalar x,
    const NumericalScalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
    const UnsignedInteger maximumIteration = ResourceMap::GetAsScalar("DistFunc-MaximumIteration"));

NumericalScalar NonCentralStudentRealization(const NumericalScalar nu,
    const NumericalScalar delta);
} // StudentFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDENTFUNCTIONS_HXX */
