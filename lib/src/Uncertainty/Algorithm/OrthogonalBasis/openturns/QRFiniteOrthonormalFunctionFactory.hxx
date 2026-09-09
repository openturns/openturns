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
#ifndef OPENTURNS_QRFINITEORTHONORMALFUNCTIONFACTORY_HXX
#define OPENTURNS_QRFINITEORTHONORMALFUNCTIONFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QRFiniteOrthonormalFunctionFactory
 *
 * A finite set of orthonormal functions wrt a given distribution,
 * obtained by QR factorization of the weighted design matrix.
 */

class OT_API QRFiniteOrthonormalFunctionFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  friend class Factory<QRFiniteOrthonormalFunctionFactory>;

  /** Default constructor */
  QRFiniteOrthonormalFunctionFactory();

  /** Constructor */
  QRFiniteOrthonormalFunctionFactory(const FunctionCollection & functions,
                                     const Distribution & distribution,
                                     const WeightedExperiment & experiment = WeightedExperiment());

  /** Build the Function of the given index */
  using OrthogonalFunctionFactory::build;
  Function build(const UnsignedInteger index) const override;

  /** Build a minimal quadrature rule based on the n first orthonormal functions */
  Sample buildQuadrature(const UnsignedInteger n,
                         Point & weightsOut) const;

  /** Functions accessor */
  void setFunctionsCollection(const FunctionCollection & functions);
  FunctionCollection getFunctionsCollection() const;

  /** Experiment accessor */
  void setExperiment(const WeightedExperiment & experiment);
  WeightedExperiment getExperiment() const;

  /** Coefficients accessor */
  SquareMatrix getCoefficients() const;

  /** Virtual constructor */
  QRFiniteOrthonormalFunctionFactory * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** Lazy computation of the orthonormal basis */
  void computeOrthonormalBasis() const;

  /** Build the mapping from measure domain to reference domain */
  Function buildMapping() const;

  /** Build a default Gauss product experiment for the given measure */
  static WeightedExperiment buildDefaultExperiment(const Distribution & distribution);

  /** The underlying set of initial functions */
  FunctionPersistentCollection initialFunctions_;

  /** The weighted experiment used for integration */
  WeightedExperiment experiment_;

  /** Cached orthonormal functions */
  mutable FunctionPersistentCollection orthonormalFunctions_;

  /** Cached coefficients */
  mutable SquareMatrix coefficients_;

  /** Cached mapping from actual to reference domain */
  mutable Function mapping_;

  /** Flag for lazy computation */
  mutable Bool isAlreadyComputed_;

} ; /* class QRFiniteOrthonormalFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QRFINITEORTHONORMALFUNCTIONFACTORY_HXX */
