//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the Student T
 *         CDF and quantile
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
#ifndef OPENTURNS_STUDENTFUNCTIONS_HXX
#define OPENTURNS_STUDENTFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace StudentFunctions
{
Scalar StudentCDF(const Scalar nu,
                  const Scalar x,
                  const Bool tail = false);

Scalar StudentQuantile(const Scalar nu,
                       const Scalar p,
                       const Bool tail = false);

Scalar StudentRealization(const Scalar nu);

Scalar NonCentralStudentCDF(const Scalar nu,
                            const Scalar delta,
                            const Scalar x,
                            const Bool tail = false);

Scalar NonCentralStudentCDFAlt0(const Scalar nu,
                                const Scalar delta,
                                const Scalar x,
                                const Bool tail = false,
                                const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));

Scalar NonCentralStudentCDFAlt1(const Scalar nu,
                                const Scalar delta,
                                const Scalar x,
                                const Bool tail = false,
                                const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"));

Scalar NonCentralStudentPDF(const Scalar nu,
                            const Scalar delta,
                            const Scalar x);

Scalar NonCentralStudentPDFAlt0(const Scalar nu,
                                const Scalar delta,
                                const Scalar x,
                                const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));

Scalar NonCentralStudentRealization(const Scalar nu,
                                    const Scalar delta);
} // StudentFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDENTFUNCTIONS_HXX */
