//                                               -*- C++ -*-
/**
 * @brief Marginal evaluation
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

#ifndef OPENTURNS_MARGINALEVALUATION_HXX
#define OPENTURNS_MARGINALEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class MarginalEvaluation
 *
 * Marginal evaluation
 */
class OT_API MarginalEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MarginalEvaluation();

  MarginalEvaluation(const EvaluationImplementation & evaluation,
                     const Indices & indices);

#ifndef SWIG
  /** Parameters constructor */
  MarginalEvaluation(const Pointer<EvaluationImplementation> & p_evaluation,
                     const Indices & indices);
#endif

  /** Virtual constructor */
  virtual MarginalEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const MarginalEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Indices accessor */
  Indices getIndices() const;

  /* Here is the interface that all derived class must implement */

  /** Evaluation operator */
  using EvaluationImplementation::operator();
  Point operator() (const Point & point) const;

  Sample operator() (const Sample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  /** The underlying function */
  Pointer<EvaluationImplementation> p_evaluation_;

  /** The indices of the marginals */
  Indices indices_;

}; /* class MarginalEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NONOEVALUATION_HXX */
