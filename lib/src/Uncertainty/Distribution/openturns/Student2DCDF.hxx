//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the bivariate Student CDF
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_STUDENT2DCDF_HXX
#define OPENTURNS_STUDENT2DCDF_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

Scalar Student2DCDF(const Scalar nu,
                    const Scalar x1,
                    const Scalar x2,
                    const Scalar rho,
                    const Bool tail = false);

Scalar Student2DRectangularProbability(const Scalar nu,
                                       const Scalar a1,
                                       const Scalar a2,
                                       const Scalar b1,
                                       const Scalar b2,
                                       const Scalar rho);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDENT2DCDF_HXX */
