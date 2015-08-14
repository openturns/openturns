//                                               -*- C++ -*-
/**
 *  @brief The CompositeDistribution distribution
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
#ifndef OPENTURNS_COMPOSITEDISTRIBUTION_HXX
#define OPENTURNS_COMPOSITEDISTRIBUTION_HXX

#include "DistributionImplementation.hxx"
#include "Distribution.hxx"
#include "NumericalMathFunction.hxx"
#include "Solver.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CompositeDistribution
 *
 * The CompositeDistribution distribution.
 */
class OT_API CompositeDistribution
  : public DistributionImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  CompositeDistribution();

  /** Parameters constructor */
  CompositeDistribution(const NumericalMathFunction & function,
                        const Distribution & antecedent);

  /** Parameters constructor */
  CompositeDistribution(const NumericalMathFunction & function,
                        const Distribution & antecedent,
                        const NumericalPoint & bounds,
                        const NumericalPoint & values);

  /** Comparison operator */
  Bool operator ==(const CompositeDistribution & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual CompositeDistribution * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /* Interface specific to CompositeDistribution */

  /** Function accessor */
  void setFunction(const NumericalMathFunction & function);
  NumericalMathFunction getFunction() const;

  /** Antecedent accessor */
  void setAntecedent(const Distribution & antecedent);
  Distribution getAntecedent() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Set the solver used to perform the different computations */
  void setSolver(const Solver & solver);
  Solver getSolver() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:
  /** update all the derivative attributes */
  void update();

  /** Set the function and antecedent with check */
  void setFunctionAndAntecedent(const NumericalMathFunction & function,
                                const Distribution & antecedent);

  /** The main parameter set of the distribution */
  NumericalMathFunction function_;
  Distribution antecedent_;

  /** Usefull quantities */
  NumericalPoint bounds_;
  NumericalPoint values_;
  NumericalPoint probabilities_;
  Indices increasing_;

  /** Solver used to invert the function and to find the zeros of its derivative */
  Solver solver_;
}; /* class CompositeDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSITEDISTRIBUTION_HXX */
