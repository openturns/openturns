//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of functional linear combination of vectors
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
#ifndef OPENTURNS_DUALLINEARCOMBINATIONEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_DUALLINEARCOMBINATIONEVALUATIONIMPLEMENTATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class DualLinearCombinationEvaluationImplementation
 *
 * The evaluation part of functional linear combination of vectors
 */

class OT_API DualLinearCombinationEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  typedef Collection<NumericalMathFunction>                              NumericalMathFunctionCollection;
  typedef PersistentCollection<NumericalMathFunction>                    NumericalMathFunctionPersistentCollection;

  // friend class Factory<DualLinearCombinationEvaluationImplementation>;

  /** Default constructor */
  DualLinearCombinationEvaluationImplementation();

public:
  /** Parameter constructor */
  DualLinearCombinationEvaluationImplementation(const NumericalMathFunctionCollection & functionsCollection,
      const NumericalSample & coefficients);

  /** Virtual constructor */
  virtual DualLinearCombinationEvaluationImplementation * clone() const;

  /** Description accessor */
  void setDescription(const Description & description);

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Evaluation operator */
  NumericalPoint operator () (const NumericalPoint & inP) const;
  NumericalSample operator () (const NumericalSample & inS) const;

  /** Coefficients accessor */
  NumericalSample getCoefficients() const;

  /** Functions accessor */
  NumericalMathFunctionCollection getFunctionsCollection() const;
  void setFunctionsCollectionAndCoefficients(const NumericalMathFunctionCollection & functionsCollection,
      const NumericalSample & coefficients);

  /** Input dimension accessor */
  UnsignedInteger getInputDimension() const;

  /** Output dimension accessor */
  UnsignedInteger getOutputDimension() const;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const NumericalPoint & inP) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescription getParameter() const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  // Make the gradient and the hessian friend classes of the evaluation in order to share the functions and the coefficients
  friend class DualLinearCombinationGradientImplementation;
  friend class DualLinearCombinationHessianImplementation;
  friend struct DualLinearCombinationEvaluationPointFunctor;

  // The functional coefficients of the combination
  NumericalMathFunctionPersistentCollection functionsCollection_;

  // The vectors of the combination
  NumericalSample coefficients_;
} ; /* class DualLinearCombinationEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DUALLINEARCOMBINATIONEVALUATIONIMPLEMENTATION_HXX */
