//                                               -*- C++ -*-
/**
 *  @brief A factory of orthonormal functions from a finite set of functions.
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

#ifndef OPENTURNS_BASISORTHONORMALIZATIONFACTORY_HXX
#define OPENTURNS_BASISORTHONORMALIZATIONFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/BasisOrthonormalizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API BasisOrthonormalizationFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  BasisOrthonormalizationFactory();

  BasisOrthonormalizationFactory(const FunctionCollection & functions,
                                   const Distribution & distribution);

  BasisOrthonormalizationFactory(const FunctionCollection & functions,
                                   const Distribution & distribution,
                                   const WeightedExperiment & experiment);

  BasisOrthonormalizationFactory(const FunctionCollection & functions,
                                   const Distribution & distribution,
                                   const IntegrationAlgorithm & integrationAlgorithm);

  BasisOrthonormalizationFactory * clone() const override;

  using OrthogonalFunctionFactory::build;
  Function build(const UnsignedInteger index) const override;

  Sample buildQuadrature(const UnsignedInteger n,
                         Point & weightsOut) const;

  void setFunctionsCollection(const FunctionCollection & functions);
  void setMeasureAndFunctions(const Distribution & measure,
                              const FunctionCollection & functions);
  FunctionCollection getFunctionsCollection() const;

  void setExperiment(const WeightedExperiment & experiment);
  WeightedExperiment getExperiment() const;

  void setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm);
  IntegrationAlgorithm getIntegrationAlgorithm() const;

  SquareMatrix getCoefficients() const;

  BasisOrthonormalizationAlgorithm getOrthonormalizationAlgorithm() const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  // Single source of truth for measure, functions and integration rule
  BasisOrthonormalizationAlgorithm orthoAlgo_;

};

END_NAMESPACE_OPENTURNS

#endif
