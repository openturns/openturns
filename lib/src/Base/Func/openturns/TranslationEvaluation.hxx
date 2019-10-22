//                                               -*- C++ -*-
/**
 * @brief Class for a numerical math function implementation
 *        of the form y = constant + x
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_TRANSLATIONEVALUATION_HXX
#define OPENTURNS_TRANSLATIONEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TranslationEvaluation
 */
class OT_API TranslationEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  TranslationEvaluation();

  /** Parameter constructor */
  explicit TranslationEvaluation(const Point & constant);

  /** Virtual constructor */
  virtual TranslationEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const TranslationEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Accessor for the constant term */
  virtual Point getConstant() const;
  virtual void setConstant(const Point & constant);

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual Point operator() (const Point & inP) const;

  /** Operator () */
  virtual Sample operator() (const Sample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Linearity accessors */
  Bool isLinear() const;
  Bool isLinearlyDependent(const UnsignedInteger index) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  Point constant_;
}; /* class TranslationEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRANSLATIONEVALUATION_HXX */
