//                                               -*- C++ -*-
/**
 *  @brief Ridgway algorithm (SMC) to compute orthant probabilities of
 *         multivariate Normal distributions.
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
#ifndef OPENTURNS_RIDGWAY_HXX
#define OPENTURNS_RIDGWAY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"
BEGIN_NAMESPACE_OPENTURNS

OT_API Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const CovarianceMatrix& sigma,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"));

OT_API Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const CovarianceMatrix& sigma,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"));

OT_API Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const TriangularMatrix& L,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"));

OT_API Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const TriangularMatrix& L,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"));

OT_API Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const TriangularMatrix& L,
    Scalar nu,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"),
    UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultStudentSampleSize"));

OT_API Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const CovarianceMatrix& sigma,
    Scalar nu,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"),
    UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultStudentSampleSize"));

OT_API Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const TriangularMatrix& L,
    Scalar nu,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"),
    UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultStudentSampleSize"));

OT_API Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const CovarianceMatrix& sigma,
    Scalar nu,
    UnsignedInteger M = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultParticleNumber"),
    Scalar alpha = ResourceMap::GetAsScalar("Ridgway-DefaultAlpha"),
    UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("Ridgway-DefaultStudentSampleSize"));

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_RIDGWAY_HXX
