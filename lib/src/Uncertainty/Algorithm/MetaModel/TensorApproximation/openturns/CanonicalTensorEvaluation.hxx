//                                               -*- C++ -*-
/**
 *  @brief Canonical tensor representation and evaluation
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
#ifndef OPENTURNS_CANONICALTENSOREVALUATION_HXX
#define OPENTURNS_CANONICALTENSOREVALUATION_HXX

#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"
#include "openturns/Basis.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/UniVariateFunctionEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API CanonicalTensorEvaluation : public EvaluationImplementation
{
  CLASSNAME
public:
  typedef Collection<OrthogonalUniVariateFunctionFamily>           FunctionFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariateFunctionFamily> FunctionFamilyPersistentCollection;
  typedef Collection<Function>                                     FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;
  typedef Collection<UniVariateFunction> UniVariateFunctionCollection;
  typedef PersistentCollection<UniVariateFunction> UniVariateFunctionPersistentCollection;

  CanonicalTensorEvaluation();

  CanonicalTensorEvaluation(const FunctionFamilyCollection & functionFamilies,
                            const Indices & degrees,
                            const UnsignedInteger rank = 1);

  /** Virtual constructor */
  CanonicalTensorEvaluation * clone() const override;

  /** Evaluation operator */
  Point operator() (const Point & point) const override;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const override;
  UnsignedInteger getOutputDimension() const override;

  Indices getDegrees() const;

  /** Coefficients accessor for rank m along i-th component */
  void setCoefficients(const UnsignedInteger i, const UnsignedInteger j, const Point & coefficients);
  Point getCoefficients(const UnsignedInteger i, const UnsignedInteger j) const;

  /** Basis accessor along i-th component */
  FunctionCollection getBasis(const UnsignedInteger i) const;

  void setRank(const UnsignedInteger rank);
  UnsignedInteger getRank() const;

  CanonicalTensorEvaluation getMarginalRank(const UnsignedInteger i) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // subbasis sizes
  Indices degrees_;

  // subbasis coefficients
  PersistentCollection<Sample> coefficients_;

  PersistentCollection<FunctionPersistentCollection> basis_;

  // Attribute is used internally for the evaluation of operator
  // It avoids creation of useless Points
  UniVariateFunctionPersistentCollection univariateBasis_;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CANONICALTENSOREVALUATION_HXX */
