//                                               -*- C++ -*-
/**
 * @brief Class for constant evaluation
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_CONSTANTEVALUATION_HXX
#define OPENTURNS_CONSTANTEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ConstantEvaluation
 */
class OT_API ConstantEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit ConstantEvaluation(const UnsignedInteger inputDimension = 1,
                              const Point & constant = {0.0});

  /** Virtual constructor */
  ConstantEvaluation * clone() const override;

  /** Comparison operator */
  using EvaluationImplementation::operator ==;
  Bool operator ==(const ConstantEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inP) const override;

  /** Operator () */
  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Get the function corresponding to indices components */
  using EvaluationImplementation::getMarginal;
  Evaluation getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Constant accessor */
  Point getConstant() const;

protected:


private:
  UnsignedInteger inputDimension_ = 0;
  Point constant_;

}; /* class ConstantEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_IDENTITYEVALUATION_HXX */
