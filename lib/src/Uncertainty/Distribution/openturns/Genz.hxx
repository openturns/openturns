//                                               -*- C++ -*-
/**
 *  @brief Genz algorithm to compute rectangular probabilities of multivariate
 *         Normal and Student distributions.
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
#ifndef OPENTURNS_GENZ_HXX
#define OPENTURNS_GENZ_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"
BEGIN_NAMESPACE_OPENTURNS

OT_API Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const CovarianceMatrix & sigma,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const CovarianceMatrix & sigma,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const TriangularMatrix & L,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const TriangularMatrix & L,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const CovarianceMatrix & sigma,
    const Scalar nu,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const CovarianceMatrix & sigma,
    const Scalar nu,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const TriangularMatrix & L,
    const Scalar nu,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

OT_API Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const TriangularMatrix & L,
    const Scalar nu,
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("Genz-DefaultSampleSize"));

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_GENZ_HXX
