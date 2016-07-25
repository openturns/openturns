//                                               -*- C++ -*-
/**
 *  @brief This is the evaluation part of the Soize-Ghanem basis coupling factor
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SOIZEGHANEMFACTOREVALUATION_HXX
#define OPENTURNS_SOIZEGHANEMFACTOREVALUATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SoizeGhanemFactorEvaluation
 *
 * This is the orthogonal polynomial basis
 */

/* Multiplicative factor of the multivariate basis */
/* Evaluation part */
class OT_API SoizeGhanemFactorEvaluation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
 public:

  typedef Collection<Distribution>           DistributionCollection;
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;

  friend class Factory<SoizeGhanemFactorEvaluation>;

  /** Default constructor */
  SoizeGhanemFactorEvaluation();

  /** Default constructor */
  SoizeGhanemFactorEvaluation(const Distribution & measure,
                              const Collection<Distribution> & marginals,
                              const Bool useCopula);

  /** Virtual constructor */
  SoizeGhanemFactorEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const SoizeGhanemFactorEvaluation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset) const;

  /** Operator () */
  using NumericalMathEvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;
  NumericalSample operator() (const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for input point description */
  Description getInputDescription() const;

  /** Accessor for output point description */
  Description getOutputDescription() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

 protected:

 private:

  /** The underlying measure */
  Distribution measure_;

  /** Flag to tell if the evaluation is based on the copula PDF or on the joint PDF */
  Bool useCopula_;

  /** The copula of the measure */
  Distribution copula_;

  /** The 1D marginal distributions of the measure */
  DistributionPersistentCollection marginals_;

} ; /* class SoizeGhanemFactorEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOIZEGHANEMFACTOREVALUATION_HXX */
