//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the CDF computation for a tri-dimensional
 *         Normal distribution
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
#ifndef OPENTURNS_NORMALCDF3D_HXX
#define OPENTURNS_NORMALCDF3D_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

NumericalScalar Normal3DCDF(const NumericalScalar x1,
                            const NumericalScalar x2,
                            const NumericalScalar x3,
                            const NumericalScalar rho12,
                            const NumericalScalar rho13,
                            const NumericalScalar rho23,
                            const Bool tail);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALCDF3D_HXX */
