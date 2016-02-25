//                                               -*- C++ -*-
/**
 *  @brief This class implements a meta-model based on mixture of experts
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
#ifndef OPENTURNS_EXPERTMIXTURE_HXX
#define OPENTURNS_EXPERTMIXTURE_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/Indices.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Classifier.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExpertMixture
 *
 * The class building chaos expansions
 */

class OT_API ExpertMixture
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;

public:

  /** Constructor */
  ExpertMixture();

  /** Constructor */
  ExpertMixture(const Basis & experts,
                const Classifier & classifier);

  /** Virtual constructor */
  virtual ExpertMixture * clone() const;

  /** Comparison operator */
  Bool operator ==(const ExpertMixture & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using NumericalMathEvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor for local experts */
  Basis getExperts() const;
  void setExperts(const Basis & experts);

  /** Accessor for classifier */
  Classifier getClassifier() const;
  void setClassifier(const Classifier & classifier);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** The local experts */
  Basis experts_;

  /** The classifier */
  Classifier classifier_;

} ; /* class ExpertMixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPERTMIXTURE_HXX */
