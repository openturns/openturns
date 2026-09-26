//                                               -*- C++ -*-
/**
 *  @brief A finite orthonormal set of functions wrt a given distribution.
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

#ifndef OPENTURNS_FINITEORTHONORMALFUNCTIONFACTORY_HXX
#define OPENTURNS_FINITEORTHONORMALFUNCTIONFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/QROrthonormalizationAlgorithm.hxx"
#include "openturns/GaussLPQuadrature.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API FiniteOrthonormalFunctionFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  FiniteOrthonormalFunctionFactory();

  FiniteOrthonormalFunctionFactory(const FunctionCollection & functions,
                                   const Distribution & distribution,
                                   const WeightedExperiment & experiment = WeightedExperiment());

  FiniteOrthonormalFunctionFactory * clone() const override;

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

  SquareMatrix getCoefficients() const;

  void setOrthonormalizationAlgorithm(const QROrthonormalizationAlgorithm & algo);
  QROrthonormalizationAlgorithm getOrthonormalizationAlgorithm() const;

  void setGaussLPQuadrature(const GaussLPQuadrature & quad);
  GaussLPQuadrature getGaussLPQuadrature() const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  void computeOrthonormalBasis() const;

  FunctionPersistentCollection initialFunctions_;
  WeightedExperiment experiment_;
  mutable QROrthonormalizationAlgorithm orthoAlgo_;
  mutable GaussLPQuadrature quad_;
  mutable Bool isAlreadyComputed_;

};

END_NAMESPACE_OPENTURNS

#endif
