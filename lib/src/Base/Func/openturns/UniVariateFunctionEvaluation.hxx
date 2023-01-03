//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_UNIVARIATEFUNCTIONEVALUATION_HXX
#define OPENTURNS_UNIVARIATEFUNCTIONEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"



BEGIN_NAMESPACE_OPENTURNS



/**
 * @class UniVariateFunctionEvaluation
 *
 * This is a nD function build as a product of n 1D function
 */

class OT_API UniVariateFunctionEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Constructor */
  UniVariateFunctionEvaluation(const UniVariateFunction & function);

  /** Virtual constructor */
  UniVariateFunctionEvaluation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  Point operator() (const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;



protected:

  friend class Factory<UniVariateFunctionEvaluation>;
  friend class ProductUniVariateFunctionGradient;
  friend class ProductUniVariateFunctionHessian;

  /* Default constructor */
  UniVariateFunctionEvaluation();

private:
  /** The set of 1d functions to build the nD function */
  UniVariateFunction function_;

} ; /* class UniVariateFunctionEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIVARIATEFUNCTIONEVALUATION_HXX */
