//                                               -*- C++ -*-
/**
 *  @brief Basis orthonormalization algorithm
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

#ifndef OPENTURNS_BASISORTHONORMALIZATIONALGORITHM_HXX
#define OPENTURNS_BASISORTHONORMALIZATIONALGORITHM_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API BasisOrthonormalizationAlgorithm
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  BasisOrthonormalizationAlgorithm();

  /** Constructor from a source basis, a target measure and the number of
      its first elements to orthonormalize against the target measure. */
  BasisOrthonormalizationAlgorithm(const OrthogonalBasis & orthogonalBasis,
                                   const Distribution & targetMeasure,
                                   const UnsignedInteger size);

  BasisOrthonormalizationAlgorithm * clone() const override;

  void run();

  FunctionPersistentCollection getOrthonormalFunctions() const;
  SquareMatrix getCoefficients() const;

  /** Orthogonal basis whose first elements are orthonormalized */
  OrthogonalBasis getOrthogonalBasis() const;

  /** Number of first basis elements to orthonormalize */
  UnsignedInteger getSize() const;

  /** Target measure of the orthonormalization */
  Distribution getMeasure() const;

  void setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm);
  IntegrationAlgorithm getIntegrationAlgorithm() const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  static IntegrationAlgorithm buildDefaultIntegration(const Distribution & distribution);

  OrthogonalBasis orthogonalBasis_;
  Distribution targetMeasure_;
  UnsignedInteger size_ = 0;
  IntegrationAlgorithm integrationAlgorithm_;
  Bool hasIntegrationAlgorithm_ = false;

  mutable FunctionPersistentCollection orthonormalFunctions_;
  mutable SquareMatrix coefficients_;
  mutable Bool isAlreadyComputed_ = false;

};

END_NAMESPACE_OPENTURNS

#endif
