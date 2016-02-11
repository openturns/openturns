//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_KRIGINGEVALUATION_HXX
#define OPENTURNS_KRIGINGEVALUATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/TBB.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KrigingEvaluation
 *
 * This class permits prediction on a gaussian process
 */
class OT_API KrigingEvaluation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME

public:

  typedef Collection<NumericalPoint> NumericalPointCollection;
  typedef PersistentCollection<NumericalPoint> NumericalPointPersistentCollection;
  typedef Collection<Basis> BasisCollection;
  typedef PersistentCollection<Basis> BasisPersistentCollection;

  /** Default constructor */
  KrigingEvaluation();

  /** Constructor with parameters */
  KrigingEvaluation(const BasisCollection & basis,
                    const NumericalSample & inputSample,
                    const CovarianceModel & correlationModel,
                    const NumericalPointCollection & beta,
                    const NumericalSample & gamma);

  /** Virtual constructor */
  virtual KrigingEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const KrigingEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;

  /** Operator () */
  virtual NumericalPoint operator()(const NumericalPoint & inP) const;
  virtual NumericalSample operator()(const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  // BasisCollection ==> Persistent for save attribut
  BasisPersistentCollection basis_;

  /// Training (input) sample
  NumericalSample inputSample_;

  /// Correlation model
  CovarianceModel covarianceModel_;

  /// Regression weights
  NumericalPointPersistentCollection beta_;
  NumericalSample gamma_;

private:
  friend struct KrigingEvaluationPointFunctor;
  friend struct KrigingEvaluationSampleFunctor;
}; /* class KrigingEvaluation */



END_NAMESPACE_OPENTURNS

#endif
