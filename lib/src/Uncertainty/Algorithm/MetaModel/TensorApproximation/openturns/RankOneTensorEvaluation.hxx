//                                               -*- C++ -*-
/**
 *  @brief Rank-1 tensor evaluation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#ifndef OPENTURNS_RANKONETENSOREVALUATION_HXX
#define OPENTURNS_RANKONETENSOREVALUATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/CanonicalTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Rank-1 tensor evaluation
 *
 * Evaluation of a rank-1 tensor
 */
class OT_API RankOneTensorEvaluation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;

public:

  /** Parameters constructor */
  RankOneTensorEvaluation(const RankOneTensor & rankOneTensor);

  /** Virtual constructor method */
  RankOneTensorEvaluation * clone() const;

  /** Evaluation operator */
  virtual NumericalPoint operator() (const NumericalPoint & point) const;

  /** Dimension accessor */
  virtual UnsignedInteger getInputDimension() const;
  virtual UnsignedInteger getOutputDimension() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  friend class Factory<RankOneTensorEvaluation>;

  RankOneTensorEvaluation();

private:

  /** The underlying function */
  RankOneTensor rankOneTensor_;

}; /* class RankOneTensorEvaluation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANKONETENSOREVALUATION_HXX */
