//                                               -*- C++ -*-
/**
 *  @brief This class implements a meta-model based on mixture of experts
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
#ifndef OPENTURNS_EXPERTMIXTURE_HXX
#define OPENTURNS_EXPERTMIXTURE_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Point.hxx"
#include "openturns/Classifier.hxx"
#include "openturns/Function.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExpertMixture
 *
 * The class building chaos expansions
 */

class OT_API ExpertMixture
  : public EvaluationImplementation
{
  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Constructor */
  ExpertMixture();

  /** Constructor */
  ExpertMixture(const FunctionCollection & experts,
                const Classifier & classifier,
                const Bool supervised = true);

  /** Virtual constructor */
  ExpertMixture * clone() const override;

  /** Comparison operator */
  Bool operator ==(const ExpertMixture & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const override;
  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for local experts */
  FunctionCollection getExperts() const;
  void setExperts(const FunctionCollection & experts);

  /** Accessor for classifier */
  Classifier getClassifier() const;
  void setClassifier(const Classifier & classifier);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Point evaluateSupervised(const Point & inP) const;
  Sample evaluateSupervised(const Sample & inP) const;
  Point evaluateNonSupervised(const Point & inP) const;
  Sample evaluateNonSupervised(const Sample & inP) const;

  /** The local experts */
  FunctionPersistentCollection experts_;

  /** The classifier */
  Classifier classifier_;

  /** Supervised mode */
  Bool supervised_;

} ; /* class ExpertMixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPERTMIXTURE_HXX */
