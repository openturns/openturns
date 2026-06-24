//                                               -*- C++ -*-
/**
 *  @brief QR-based orthonormalization algorithm
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

#ifndef OPENTURNS_QRORTHONORMALIZATIONALGORITHM_HXX
#define OPENTURNS_QRORTHONORMALIZATIONALGORITHM_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API QROrthonormalizationAlgorithm
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  QROrthonormalizationAlgorithm();

  QROrthonormalizationAlgorithm(const FunctionCollection & functions,
                                const Distribution & measure,
                                const WeightedExperiment & experiment = WeightedExperiment());

  QROrthonormalizationAlgorithm * clone() const override;

  void run();

  FunctionPersistentCollection getOrthonormalFunctions() const;
  SquareMatrix getCoefficients() const;
  Function getMapping() const;

  void setFunctionsCollection(const FunctionCollection & functions);
  FunctionCollection getFunctionsCollection() const;

  void setMeasure(const Distribution & measure);
  Distribution getMeasure() const;

  void setExperiment(const WeightedExperiment & experiment);
  WeightedExperiment getExperiment() const;

  String __repr__() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  Function buildMapping() const;
  static WeightedExperiment buildDefaultExperiment(const Distribution & distribution);

  FunctionPersistentCollection initialFunctions_;
  Distribution measure_;
  WeightedExperiment experiment_;

  mutable FunctionPersistentCollection orthonormalFunctions_;
  mutable SquareMatrix coefficients_;
  mutable Function mapping_;
  mutable Bool isAlreadyComputed_;

};

END_NAMESPACE_OPENTURNS

#endif
