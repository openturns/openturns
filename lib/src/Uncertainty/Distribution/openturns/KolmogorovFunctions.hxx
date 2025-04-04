//                                               -*- C++ -*-
/**
 *  @brief Kolmogorov distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KOLMOGOROVFUNCTIONS_HXX
#define OPENTURNS_KOLMOGOROVFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace KolmogorovFunctions
{
Scalar _kolmogn(const UnsignedInteger n, const Scalar x, const Bool cdf);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KOLMOGOROVFUNCTIONS_HXX */
